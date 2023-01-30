#ifndef __DEVICE_MGR_H__
#define __DEVICE_MGR_H__

#include <stdint.h>
//#include "google/rpc/code.grpc.pb.h"
#include "p4/v1/p4runtime.grpc.pb.h"
#include "p4/config/v1/p4info.grpc.pb.h"
#include "utils/map.h"
#include "utils/table.h"
/*
TODO by Ian 
更改"handlers.h"、"messages.h"里定义
*/
#include "handlers.h" /* T4P4S*/
#include "messages.h" /* T4P4S*/
/*
TODO by Ian 
#include <grpc++/grpc++.h>
#include <grpcpp/grpcpp.h>
两个头文件导入调换
*/
#include <grpc++/grpc++.h>
//#include <grpcpp/grpcpp.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::ServerReaderWriter;
using grpc::Status;
using grpc::StatusCode;
using grpc::ServerCompletionQueue;
using grpc::ServerAsyncReaderWriter;

namespace p4v1 = ::p4::v1;
using p4v1::P4Runtime;
using p4v1::StreamMessageResponse;
using p4v1::StreamMessageRequest;
using StreamChannelReaderWriter = grpc::ServerReaderWriter<p4v1::StreamMessageResponse, p4v1::StreamMessageRequest>;
// using pi::fe::proto;

typedef void (*GenericFnPtr)();
typedef void (*PIPacketInCb)(const char *pkt, size_t size,
                             void *cb_cookie);

typedef struct device_map_s {
  void *_map;
} device_map_t;

typedef struct {
	GenericFnPtr cb;
	void *cookie;
} cb_data_t;

typedef struct {
	device_map_t device_cbs;
	cb_data_t default_cb;
}cb_mgr_t;

typedef struct device_mgr_t {
	uint64_t device_id;
	/*TODO: ADD P4Info and other meta information */
	map_t id_map;
	table_entries_t id_table_entries;
	p4_msg_callback cb;
	p4_reg_read read_register_by_name;
	p4_reg_write write_register_by_name;
	int has_p4info;
	::p4::config::v1::P4Info p4info;
} device_mgr_t;

typedef enum {
	P4IDS_TABLE = 0,
	P4IDS_REGISTER = 1,
} p4_ids;

// class DeviceMgrImp;
// class DeviceMgr {
//  public:
//   using device_id_t = uint64_t;
//   using Status = grpc::Status;
//   using StreamMessageResponseCb = std::function<void(
//       device_id_t, p4::v1::StreamMessageResponse *msg, void *cookie)>;
//   explicit DeviceMgr(device_id_t device_id);

//   ~DeviceMgr();

//   // New pipeline_config_set and pipeline_config_get methods to replace init,
//   // update_start and update_end
//   Status pipeline_config_set(
//       p4::v1::SetForwardingPipelineConfigRequest::Action action,
//       const p4::v1::ForwardingPipelineConfig &config);

//   Status pipeline_config_get(
//       p4::v1::GetForwardingPipelineConfigRequest::ResponseType response_type,
//       p4::v1::ForwardingPipelineConfig *config);

//   Status write(const p4::v1::WriteRequest &request);

//   Status read(const p4::v1::ReadRequest &request,
//               p4::v1::ReadResponse *response) const;
//   Status read_one(const p4::v1::Entity &entity,
//                   p4::v1::ReadResponse *response) const;

//   Status stream_message_request_handle(
//       const p4::v1::StreamMessageRequest &request);

//   void stream_message_response_register_cb(StreamMessageResponseCb cb,
//                                            void *cookie);

//   static Status init();

//  private:
//   // PIMPL design
//   std::unique_ptr<DeviceMgrImp> pimp;
// };
// extern cb_mgr_t packet_cb_mgr_t
// static StreamChannelReaderWriter stream_;

extern StreamChannelReaderWriter *stream_store;

// extern StreamChannelReaderWriter *stream_store;

extern device_mgr_t *dev_mgr_ptr;

extern cb_mgr_t *packet_cb_mgr_ptr;

grpc::Status table_insert(device_mgr_t *dm, const ::p4::v1::TableEntry &table_entry);

grpc::Status table_modify(device_mgr_t *dm, const ::p4::v1::TableEntry &table_entry);

grpc::Status table_delete(device_mgr_t *dm, const ::p4::v1::TableEntry &table_entry);

grpc::Status table_write(device_mgr_t *dm, ::p4::v1::Update::Type update, const ::p4::v1::TableEntry &table_entry);

grpc::Status dev_mgr_write(device_mgr_t *dm, const ::p4::v1::WriteRequest &request, ::p4::v1::WriteResponse *response);

grpc::Status dev_mgr_read(device_mgr_t *dm, const ::p4::v1::ReadRequest &request, ::p4::v1::ReadResponse *response);

grpc::Status dev_mgr_set_pipeline_config(device_mgr_t *dm, ::p4::v1::SetForwardingPipelineConfigRequest_Action action, const ::p4::v1::ForwardingPipelineConfig config);

grpc::Status dev_mgr_get_pipeline_config(device_mgr_t *dm, ::p4::v1::GetForwardingPipelineConfigRequest::ResponseType response_type, ::p4::v1::ForwardingPipelineConfig *config);

extern "C" {
  void dev_mgr_init(device_mgr_t *dev_mgr_ptr);
  void dev_mgr_init_with_t4p4s(device_mgr_t *dev_mgr_ptr, p4_msg_callback cb, p4_reg_read read_register_by_name, p4_reg_write write_register_by_name, uint64_t device_id);
  void async_packetin_data(uint8_t* data, int len);
}
/*
TODO by Ian 
构造server端的CALL父类和grpc链接结构体
// */
// struct AsyncServerData {
//   std::string server_address;
//   int server_port;
//   p4v1::P4Runtime::AsyncService service;
//   ServerBuilder builder;
//   std::unique_ptr<Server> server;
//   std::unique_ptr<ServerCompletionQueue> cq;
// };
// extern AsyncServerData *asyn_server_data_in;
// extern AsyncServerData *asyn_server_data_out;

// class CommonAsyncCallData
// {
// 	public:
//     // The means of communication with the gRPC runtime for an asynchronous
//     // server.
//     p4v1::P4Runtime::AsyncService* service_;
//     // The producer-consumer queue where for asynchronous server notifications.
//     ServerCompletionQueue* cq_;
//     // Context for the rpc, allowing to tweak aspects of it such as the use
//     // of compression, authentication, as well as to send metadata back to the
//     // client.
//     ServerContext ctx_;
//     // What we get from the client.
//     p4v1 ::StreamMessageRequest request_;
//     // What we send back to the client.
//     p4v1 ::StreamMessageResponse reply_;
// 	// Let's implement a tiny state machine with the following states.
//     enum CallStatus { CREATE, PROCESS, FINISH };
//     CallStatus status_;  // The current serving state.
//     uint8_t *data_;

// 	public:
// 	explicit CommonAsyncCallData(p4v1::P4Runtime::AsyncService* service, ServerCompletionQueue* cq, uint8_t *data, bool bool_unuse, bool new_responder_created_bool):
// 						service_(service), cq_(cq),status_(CREATE),data_(data)
// 	{}

// 	virtual ~CommonAsyncCallData()
// 	{
// //		std::cout << "CommonCallData destructor" << std::endl;
// 	}
// 	virtual void Proceed(bool = true) = 0;
// };

// class StreamChannelImpl: public CommonAsyncCallData
// {
// 	//响应者、计数器packetin、读写状态标志、标记已创建新的响应者
//     ServerAsyncReaderWriter<StreamMessageResponse, StreamMessageRequest> responder_;
// 	unsigned mcounter;
// 	bool writing_mode_;
// 	bool new_responder_created;
// 	int len_;
// 	public:
// 	StreamChannelImpl(p4v1::P4Runtime::AsyncService* service, ServerCompletionQueue* cq, uint8_t* data, bool write_status, bool new_responder_created_bool, int len):
// 		CommonAsyncCallData(service, cq, data, write_status, new_responder_created_bool), responder_(&ctx_), mcounter(0), writing_mode_(write_status), new_responder_created(new_responder_created_bool), len_(len){Proceed();}

// 	virtual void Proceed(bool ok = true) override
// 	{
// 		if(status_ == CREATE)
// 		{
// 			//std::cout << "[Server]: CREATE-Create New StreamChannelImpl" << std::endl;
// 			status_ = PROCESS ;
// 			service_->RequestStreamChannel(&ctx_, &responder_, cq_, cq_, this);
// 		}
// 		else if(status_ == PROCESS)
// 		{
// 			if(!new_responder_created)
// 			{
// 				//创建下一个请求的响应器,但是好像我不需要接受packetout再响应,不知道这个需不需要?
// 				char new_data[] = "hello,client!!!!!";
// 				uint8_t* u_data = (uint8_t*)new_data;
// 				int len;
// 				len = sizeof(new_data)/sizeof(char);
// 				new StreamChannelImpl(service_, cq_, u_data, false, false, len);
// 				new_responder_created = true;
// 			}
// 			if(!writing_mode_)//reading mode
// 			{
// 				if(!ok)
// 				{
// 					writing_mode_ = true;
// 					ok = true;
// 					// std::cout << "[server]: changing state to writing" << std::endl;
// 				}
// 				else
// 				{
// 					responder_.Read(&request_, (void*)this);
// 					std::string payload = request_.packet().payload();
// 					// std::cout << "[tang]: " << payload << std::endl;
// 					int len = payload.length();
// 					std::cout << "[Server]: PROCESS-Receive [" << payload << "] from Client and length [" << len << "]" << std::endl;
// 					struct p4_ctrl_msg ctrl_m;
// 					ctrl_m.type = P4T_PACKET_OUT;
// 					//ctrl_m.packet = const_cast<char*>(payload.data());
// 					ctrl_m.packet = (uint8_t*)(payload.data());
// 					// ctrl_m.packet = (char*)payload.data();
// 					ctrl_m.len = len;
// 					dev_mgr_ptr->cb(&ctrl_m);
// 					// std::cout << "[tang]: done dm->cb---------" <<len<< "\n\n\n\n"<< std::endl;
// 				}
// 			}
// 			if(writing_mode_)//writing mode,因为设想packetin与packetout无关,就不改变writing_mod的状态了
// 			{
//  				std::vector<std::string> vector_data = {"client"};
// 				// if(!ok || mcounter>= vector_data.size() || ctx_.IsCancelled())
// 				if(!ok || mcounter>= vector_data.size())
// 				{
// 					std::cout << "[Server]: PROCESS-This streamflow is over" << std::endl;
// 					status_ = FINISH;
// 					responder_.Finish(Status(), (void*)this);
// 				}
// 				else
// 				{
// 				std::string packetdata( (char*) data_, len_);
// 				auto packet = reply_.mutable_packet();
// 				packet->set_payload(packetdata);
// 				responder_.Write(reply_, (void*)this);
// 				++mcounter;
// 				// std::cout << "[server]: Write-Done" <<mcounter<< std::endl;
// 				}
// 			}
// 		}
// 		else
// 		{
// 			// std::cout << "[Server]: FINISH-Delete it\n\n" << std::endl;
// 			delete this;
// 		}
// 	}
// };



#endif /* __DEVICE_MGR_H__ */
