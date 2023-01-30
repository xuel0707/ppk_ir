#include <functional>
#include <stdexcept>
#include <boost/utility.hpp>
#include <boost/type_traits.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <grpc++/grpc++.h>
#include <thread>
#include "p4/v1/p4runtime.grpc.pb.h"

#include <chrono>
#include <grpc/grpc.h>

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientAsyncReaderWriter;
using grpc::ClientAsyncReader;
using grpc::ClientAsyncWriter;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;
using grpc::ClientReaderWriter;

namespace p4v1 = ::p4::v1;
using p4v1::P4Runtime;
using p4v1::StreamMessageRequest;
using p4v1::StreamMessageResponse;

class AbstractAsyncClientCall
{
public:
	enum CallStatus { PROCESS, FINISH, DESTROY };

	explicit AbstractAsyncClientCall():callStatus(PROCESS){}
	virtual ~AbstractAsyncClientCall(){}
	StreamMessageResponse reply;
	ClientContext context;
	Status status;
	CallStatus callStatus ;
	void printReply()
	{
        std::string reply_data = reply.packet().payload();
		std::cout << "[Client]: Recive reply message from Server "<< reply_data << std::endl;
	}
	virtual void Proceed(bool = true) = 0;
};

class AsyncClientCallMM : public AbstractAsyncClientCall
{
	std::unique_ptr< ClientAsyncReaderWriter<StreamMessageRequest,StreamMessageResponse> > responder;
	unsigned mcounter;
	bool writing_mode_;
public:
	AsyncClientCallMM(CompletionQueue& cq_, std::unique_ptr<P4Runtime::Stub>& stub_):
	AbstractAsyncClientCall(), mcounter(0), writing_mode_(true)
	{
		std::cout << "[Client]: Create New AsyncClientCallMM" << std::endl;
    	responder = stub_->AsyncStreamChannel(&context, &cq_, (void*)this);
		callStatus = PROCESS ;
	}
	virtual void Proceed(bool ok = true) override
	{
		if(callStatus == PROCESS)
		{
			if(writing_mode_)
			{
                static std::vector<std::string> vector_data = {"Hello, server!",
    	                                    	"Glad to see you!",
        	                                	"Haven't seen you for thousand years!",
            	                            	"I'm client now. Call me later."};
                if(mcounter < vector_data.size())
    			{
        			StreamMessageRequest request;
                    auto packet = request.mutable_packet();
                    packet->set_payload(vector_data.at(mcounter));
                    std::string test = request.packet().payload();
                    std::cout << "[tang]:  = " << test << std::endl;
           			responder->Write(request, (void*)this);
        			++mcounter;
                }
                else
                {
                    responder->WritesDone((void*)this);
                    writing_mode_ = false;
                }
                    return;		
			}
			else //reading mode
			{
                if(!ok){
                    std::cout << "[Client]: WriteDone payload" <<mcounter << std::endl;
					callStatus = FINISH;
					responder->Finish(&status, (void*)this);
					return;
                }
                responder->Read(&reply, (void*)this);
				printReply();
			}
            return;
		}
		else if(callStatus == FINISH)
		{
			std::cout << "[Client]: callStatus == FINISH" << std::endl;
			delete this;
		}
	}
};

class P4RuntimeClient
{
public:
    explicit P4RuntimeClient(std::shared_ptr<Channel> channel)
            :stub_(P4Runtime::NewStub(channel))
	{}
	void BothGladToSee()
	{
    	new AsyncClientCallMM(cq_, stub_);
	}

	void AsyncCompleteRpc()
	{
		void* got_tag;
    	bool ok = false;
		while(cq_.Next(&got_tag, &ok))
		{
        	AbstractAsyncClientCall* call = static_cast<AbstractAsyncClientCall*>(got_tag);
			call->Proceed(ok);
    	}
		std::cout << "Completion queue is shutting down." << std::endl;
	}

private:
    // Out of the passed in Channel comes the stub, stored here, our view of the
    // server's exposed services.
    std::unique_ptr<P4Runtime::Stub> stub_;

    // The producer-consumer queue we use to communicate asynchronously with the
    // gRPC runtime.
    CompletionQueue cq_;
};


int main(int argc, char* argv[])
{
	P4RuntimeClient client(grpc::CreateChannel("127.0.0.1:50052", grpc::InsecureChannelCredentials()));
	std::thread thread_ = std::thread(&P4RuntimeClient::AsyncCompleteRpc, &client);
	// client.SayHello("world");
	// client.GladToSeeMe("client");
	// client.GladToSeeYou();
	client.BothGladToSee();
	thread_.join();
}