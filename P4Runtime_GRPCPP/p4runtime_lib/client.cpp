#include "p4runtime.grpc.pb.h"
#include "p4runtime.pb.h"
#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>
#include "gnmi.h"
#include "google/rpc/code.pb.h"
#include <string>
#include <iostream>
#include "uint128.h"
#include <cstdio>
#include "PI/proto/pi_server.h"
#include "device_mgr.h"

using grpc::ClientContext;
using grpc::Channel;
using grpc::Status;
using grpc::CompletionQueue;
using grpc::ClientAsyncResponseReader;

class P4rtClient {
	public:
		explicit P4rtClient(std::shared_ptr<Channel> channel)
		:stb_(P4Runtime::NewStub(channel)){}

	int Write(::google::protobuf::uint64 dev_id,::google::protobuf::uint64 ro_id,::p4::v1::Uint128& ele_id,int up_index,::p4::v1::WriteRequest_Atomicity ato_index,
		::p4::v1::Update_Type type_value,::p4::v1::Entity* entity_,
){
		WriteRequest wrequest;
		Update updates_ob;
		Entity entity_ob;
		ExternEntry extern_entry_ob;		wrequest.set_device_id(dev_id);
		wrequest.set_role_id(ro_id);
		wrequest.set_election_id(ele_id);
		wrequest.updates(up_index);
		wrequest.set_atomicity(ato_index);
		update_ob.set_type(type_value);
		update_ob.set_allocated_entity(entity_->extern_entry());
	

		WriteResponse wreply;
		ClientContext context;
		CompletionQueue cq;
		grpc::Status status;

		std::unique_ptr<ClientAsyncResponseReader<WriteResponse>>rpc(
			stub_->AsyncWrite(&context,wrequest,&cq));

		rpc->Finish(&wreply,&stutus,(void)*1);

		void* got_tag;
		bool ok=false;

		GPR_ASSERT(cq.NEXT(&got_tag,&ok));
		GPR_ASSERT(got_tag == (void)*1);
		GPR_ASSERT(ok);

		if(status.ok()){
			return 1;
		}else{
			retuen 2;
		}
	}
	private:
		std::unique_ptr<WriteService::Stub> stub_;

};

int main(int argc, char *argv)
{
	P4rtClient client(grpc::CreateChannel("localhost:50051",grpc::InsecureChannelCredentials()));
	::google::protobuf::uint64 dev_id(11);
	::google::protobuf::uint64 ro_id(12);
	::p4::v1::Uint128& ele_id(13);
	int up_index(1);
	::p4::v1::WriteRequest_Atomicity ato_index(0);
	::p4::v1::Update_Type type_value(INSERT);
	::p4::v1::Entity* entity_;
	::p4::v1::ExternEntry* extern_entry_;
	::google::protobuf::Any* any_entry_;
	::google::protobuf::uint32 extern_ty_id(14);
	::google::protobuf::uint32 extern_id(15);


	::google::protobuf::uint64 wreply= client.Write(dev_id,ro_id,el_id,up_index,ato_index,type_value,entity_);
	std::cout<<"dev_id:"<<wreply<<"up_index"<<up_index;
	retuen 0;

}