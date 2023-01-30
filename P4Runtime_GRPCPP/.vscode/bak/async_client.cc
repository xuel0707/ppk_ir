#include <iostream>
#include <memory>
#include <string>
#include <grpc++/grpc++.h>
#include <thread>
#include "p4/v1/p4runtime.grpc.pb.h"

#include <chrono>
#include <grpc/grpc.h>

using grpc::Channel;
using grpc::ClientAsyncReader;
using grpc::ClientAsyncWriter;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;

namespace p4v1 = ::p4::v1;
using p4v1::P4Runtime;
using p4v1::StreamMessageRequest;
using p4v1::StreamMessageResponse;

class P4RuntimeClient{
    public:
    explicit P4RuntimeClient(std::shared_ptr<Channel> channel)
            : stub_(P4Runtime::NewStub(channel)) {}

        // Assembles the client's payload and sends it to the server.
    void streamchannel(const std::string& data) {
        StreamMessageRequest request;
        request.packet().set_payload(data);
        AsyncClientCall* call = new AsyncClientCall;
        call->stream =
            stub_->StreamChannel(&call->context);
        call->stream->Write(request);
    }
    // void streamchannel(const std::string& data) {
    //     // Data we are sending to the server.
    //     StreamMessageRequest request;
    //     request.packet().set_payload(data);

    //     // Call object to store rpc data
    //     AsyncClientCall* call = new AsyncClientCall;

    //     call->stream =
    //         stub_->AsyncStreamChannel(&call->context, request, &cq_);
    //     // StartCall initiates the RPC call
    //     call->stream->StartCall();

    //     // Request that, upon completion of the RPC, "reply" be updated with the
    //     // server's response; "status" with the indication of whether the operation
    //     // was successful. Tag the request with the memory address of the call object.
    //     //finish向completionqueue注册响应消息处理器和响应消息体的存储容器，响应消息处理器放入cq_中，当服务器的响应消息体到来时，被填充到call-》reply中
    //     call->stream->Finish(&call->reply, &call->status, (void*)call);

    // }        
    
    // void AsyncCompleteRpc() {
    //     void* got_tag;
    //     bool ok = false;

    //     // Block until the next result is available in the completion queue "cq".
    //     while (cq_.Next(&got_tag, &ok)) {
    //         // The tag in this example is the memory location of the call object
    //         AsyncClientCall* call = static_cast<AsyncClientCall*>(got_tag);

    //         // Verify that the request was completed successfully. Note that "ok"
    //         // corresponds solely to the request for updates introduced by Finish().
    //         GPR_ASSERT(ok);

    //         if (call->status.ok())
    //             std::cout << "packet-in: " << call->reply.packet().payload() << std::endl;
    //         else
    //             std::cout << "RPC failed" << std::endl;

    //         // Once we're complete, deallocate the call object.
    //         delete call;
    //     }
    // }

    private:
    // struct for keeping state and data information
    struct AsyncClientCall {
        // Container for the data we expect from the server.
        
        StreamMessageResponse reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // Storage for the status of the RPC upon completion.
        Status status;

        std::unique_ptr<ClientReaderWriter<StreamMessageRequest, StreamMessageResponse>> stream;
    };

    // Out of the passed in Channel comes the stub, stored here, our view of the
    // server's exposed services.
    std::unique_ptr<P4Runtime::Stub> stub_;

    // The producer-consumer queue we use to communicate asynchronously with the
    // gRPC runtime.
    CompletionQueue cq_;
}

int main(int argc, char** argv) {


    // Instantiate the client. It requires a channel, out of which the actual RPCs
    // are created. This channel models a connection to an endpoint (in this case,
    // localhost at port 50051). We indicate that the channel isn't authenticated
    // (use of InsecureChannelCredentials()).
    P4RuntimeClient p4runtime(grpc::CreateChannel(
            "127.0.0.1:50051", grpc::InsecureChannelCredentials()));

    // Spawn reader thread that loops indefinitely
    //std::thread thread_ = std::thread(&P4RuntimeClient::AsyncCompleteRpc, &p4runtime);

    for (int i = 1; i < 10; i++) {
        std::string data("data " + std::to_string(i));
        p4runtime.streamchannel(data);  // The actual RPC call!
    }

    std::cout << "Press control-c to quit" << std::endl << std::endl;
    //thread_.join();  //blocks forever

    return 0;
}
