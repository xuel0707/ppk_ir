/*
TODO by Ian 
//#include <grpc++/grpc++.h>
//#include <grpcpp/grpcpp.h>
两个头文件导入调换
//#include "cpp-out/p4/v1/p4runtime.pb.h"
*/
#include <grpc++/grpc++.h>
#include <grpc/support/log.h>
#include "cpp-out/p4/v1/p4runtime.pb.h"
//#include <grpcpp/grpcpp.h>
#include "gnmi.h"
#include "google/rpc/code.pb.h"
//#include "p4/server/v1/config.grpc.pb.h"
#include "p4/v1/p4runtime.grpc.pb.h"
#include <string>
#include <iostream>
#include "uint128.h"
#include <cstdio>
#include <typeinfo>
#include "PI/proto/pi_server.h"
#include "device_mgr.h"

#define SIMPLELOG std::cout

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::ServerReaderWriter;
using grpc::Status;
using grpc::StatusCode;

namespace p4v1 = ::p4::v1;
/*
void gnmi__g_nmi__capabilities_cb (grpc_c_context_t *context)
{
	 printf("gnmi__g_nmi__capabilities_cb\n");
}

void gnmi__g_nmi__get_cb (grpc_c_context_t *context)
{
	printf("gnmi__g_nmi__get_cb\n");
}

void gnmi__g_nmi__set_cb (grpc_c_context_t *context)
{
	printf("gnmi__g_nmi__set_cb");
}

void gnmi__g_nmi__subscribe_cb (grpc_c_context_t *context)
{
	printf("gnmi__g_nmi__set_cb");
}
*/
StreamChannelReaderWriter *stream_store;
static cb_mgr_t packet_cb_mgr_t;
cb_mgr_t *packet_cb_mgr_ptr = &packet_cb_mgr_t;

static device_mgr_t dm;

device_mgr_t *dev_mgr_ptr = &dm;
char *tmp_test = NULL;

Status SetErrorDetails(const ::google::rpc::Status& from, grpc::Status* to) {
  using grpc::Status;
  using grpc::StatusCode;
  if (to == nullptr) {
    return Status(StatusCode::FAILED_PRECONDITION, "");
  }
  StatusCode code = StatusCode::UNKNOWN;
  if (from.code() >= StatusCode::OK && from.code() <= StatusCode::DATA_LOSS) {
    code = static_cast<StatusCode>(from.code());
  }
  *to = Status(code, from.message(), from.SerializeAsString());
  return Status::OK;
}

void send_stream_message(p4v1::StreamMessageResponse *msg) {
    // auto master = get_master();
    // if (master == nullptr) return;
    // auto stream = master->stream();
	// StreamChannelReaderWriter *stream = New StreamChannelReaderWriter;
    auto success = stream_store->Write(*msg);
    if (msg->has_packet() && success) {
      std::cout << "PACKET IN\n";
    //   pkt_in_count++;
    }
}

std::string bytestring_pi_to_p4rt(const char *str, size_t n) {
  size_t i = 0;
  for (; i < n; i++) {
    if (str[i] != 0) break;
  }
  if (i == n) return std::string(1, 0);
  return std::string(str + i, n - i);
}

bool packet_in_mutate(const char *pkt, size_t size,
                   p4v1::PacketIn *packet_in) {
    int nbytes = 2;
    if (size < nbytes) return false;
    packet_in->set_payload(pkt + nbytes, size - nbytes);
    int bit_offset = 0;
    std::vector<char> buffer(2);
    // for (const auto &metadata_info : metadata_hdr.metadata()) {
      auto metadata = packet_in->add_metadata();
      metadata->set_metadata_id(1);
      // auto bitwidth = metadata_info.bitwidth();
      // buffer.resize((bitwidth + 7) / 8);
      // buffer[0] = 0;
      memcpy(buffer.data(), pkt, nbytes);
      // generic_extract(pkt, bit_offset, bitwidth, buffer.data());
      // bit_offset += (bitwidth % 8);
      // pkt += (bitwidth / 8);
      metadata->set_value(bytestring_pi_to_p4rt(buffer.data(), buffer.size()));
    // }
    return true;
}

void packet_in_cb(const char *pkt, size_t size,
                          void *cookie) {
  p4v1::StreamMessageResponse msg;
  auto *packet_in = msg.mutable_packet();
  auto success = packet_in_mutate(pkt, size, packet_in);
    if (!success) return;
   else {
    // packet_in->set_payload(pkt, size);
  }
  send_stream_message(&msg);
}

// void packet_in_register_cb(StreamMessageResponseCb cb, void *cookie) {
//   cb_ = std::move(cb);
//   cookie_ = cookie;
//   pi_packetin_register_cb(device_id, &PacketIOMgr::packet_in_cb,
//                           static_cast<void *>(this));
// }

grpc::Status no_pipeline_config_status() {
  return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION,
                      "No forwarding pipeline config set for this device");
}

grpc::Status not_master_status() {
  return grpc::Status(grpc::StatusCode::PERMISSION_DENIED, "Not master");
}

void cb_mgr_add(cb_mgr_t *cb_mgr, GenericFnPtr cb,
                void *cb_cookie) {
  // if (cb_mgr->default_cb == (cb_data_t)NULL) {
    // cb_mgr->default_cb = malloc(sizeof(cb_data_t));
  // }
  cb_mgr->default_cb.cb = cb;
  cb_mgr->default_cb.cookie = cb_cookie;
}

void notify_one(StreamChannelReaderWriter *stream, uint64_t device_id, Uint128 election_id) {
    stream_store = stream;
    // packet_in_register_cb(stream_message_response_cb, NULL);
    cb_mgr_add(packet_cb_mgr_ptr, (GenericFnPtr)packet_in_cb, NULL);
    
    auto is_master = true;
    p4v1::StreamMessageResponse response;
    auto arbitration = response.mutable_arbitration();
    arbitration->set_device_id(device_id);
    auto convert_u128 = [](const Uint128 &from, p4v1::Uint128 *to) {
      to->set_high(from.high());
      to->set_low(from.low());
    };
    convert_u128(election_id,
                 arbitration->mutable_election_id());
    auto status = arbitration->mutable_status();
    if (is_master) {
      SIMPLELOG << "MASTER\n"; 
      status->set_code(::google::rpc::Code::OK);
      status->set_message("Is master");
    } else {
      SIMPLELOG << "SLAVE\n";
      status->set_code(::google::rpc::Code::ALREADY_EXISTS);
      status->set_message("Is slave");
    }
    stream->Write(response);
}

grpc::Status pi_packetout_send(const char *pkt,size_t size) {
    struct p4_ctrl_msg ctrl_m;
		ctrl_m.type = P4T_PACKET_OUT;
					//ctrl_m.packet = const_cast<char*>(payload.data());
		ctrl_m.packet = const_cast<char*>(pkt);
    std::cout<<"ctl_m.packet:"<<ctrl_m.packet<<std::endl;
    std::cout<<"pkt:"<<pkt<<std::endl;
    					// ctrl_m.packet = (char*)payload.data();
		ctrl_m.len = size;
		dev_mgr_ptr->cb(&ctrl_m);
    return grpc::Status::OK;
}
bool packet_out_mutate(device_mgr_t *dm, const p4v1::PacketOut &packet_out, std::string pkt){
    // struct p4_ctrl_msg *ctrl_m = new p4_ctrl_msg;
    int nbytes = 2;
    pkt.clear();
    const auto &payload = packet_out.payload();
    const auto &metadata = packet_out.metadata(0).value();
    pkt.reserve(nbytes + payload.size());
    // pkt->append(nbytes, metadata);
    pkt.append(payload);
    // struct p4_ctrl_msg ctrl_m;
    // auto len = payload.length();
    std::cout << "[Server]: PROCESS-Receive [" << payload << "] from Client and length [" << payload.size() << "]" << std::endl;

    // std::string str ="C++ Strings";
    // const char* p =str.data();

    // std::cout << reinterpret_cast<void*>(const_cast<char*>(str.data())) << std::endl;
    // std::cout << &str << std::endl;
    // std::cout<<"String contains:"<<p<<std::endl;

    // ctrl_m.packet = const_cast<char*>(payload.data());
    // std::cout <<"payload.data():"<< reinterpret_cast<void*>(const_cast<char*>(payload.data())) << std::endl;
    // std::cout <<"payload[0]:"<<  &payload[0] << std::endl;
    // std::cout <<"payload:"<<  &payload << std::endl;
    // char *tmp;
    // tmp = new char[payload.size()+1];
    // std::copy(payload.begin(),payload.end(),tmp);
    // tmp[payload.size()] = '\0';
    // printf("aaaaaaaaaaaaaaaaaaaaaaaaaa--%s",payload.c_str());
    
    // std::cout <<"tmp:"<<  tmp << std::endl;
    // std::cout << "[tmp]: ==payload[0]:" << reinterpret_cast<void*>(const_cast<char*>(tmp)) <<"!!!"<<std::endl;

	  // ctrl_m.len = len;
    // ctrl_m.type = P4T_PACKET_OUT;

    //   // strcpy((char*)(ctrl_m.metadata[0]),metadata.value().c_str());
    // auto metadata = packet_out.metadata(0);
    // ctrl_m.metadata[0] = reinterpret_cast<uint8_t*>(const_cast<char*>(metadata.value().c_str()));
	  // dm->cb(&ctrl_m);
      return true;
    // } else {
		//   ctrl_m.type = P4T_PACKET_OUT;
		// 			//ctrl_m.packet = const_cast<char*>(payload.data());
		//   ctrl_m.packet = (uint8_t*)(payload.data());
		// 			// ctrl_m.packet = (char*)payload.data();
		//   ctrl_m.len = len;
		//   dev_mgr_ptr->cb(&ctrl_m);
    //   return false;
    // }
    //   auto offset_it = id2offset.find(metadata.metadata_id());
    //   if (offset_it == id2offset.end()) {
    //     RETURN_ERROR_STATUS(Code::INVALID_ARGUMENT,
    //                         "Unknown metadata id in PacketOut message");
    //   }
    //   const auto &offset = offset_it->second;
    //   generic_deparse(metadata.value().data(), offset.bitwidth,
    //                   &(*pkt)[offset.byte_offset], offset.bit_offset);
    // }
    // pkt->append(payload);
    // RETURN_OK_STATUS();
}

grpc::Status packet_out_send(device_mgr_t *dm,const p4v1::PacketOut &packet){
  std::string raw_packet;
  auto status = packet_out_mutate(dm,packet,raw_packet);
  if (status) {
      return grpc::Status::OK;
  } else {
      return grpc::Status(grpc::StatusCode::UNKNOWN,
                      "without egreesport");
  }
}

grpc::Status stream_message_request_handle(device_mgr_t *dm,const p4v1::StreamMessageRequest &request) {
  auto status = packet_out_send(dm,request.packet());
  return status;
}

void process_stream_message_request(device_mgr_t *dm,const p4v1::StreamMessageRequest &request) {
    if (request.update_case() == p4v1::StreamMessageRequest::kPacket) {
      SIMPLELOG << "PACKET OUT\n";
      //pkt_out_count++;
    }
  // stream_message_request_handle(dm,request);
  auto packet = request.packet();
  std::string raw_packet;
  std::cout<<"raw_packet size:"<< raw_packet.size()<<std::endl;
  auto status = packet_out_mutate(dm,packet,raw_packet);
  std::cout<<"raw_packet2 size:"<< raw_packet.size()<<std::endl;
  if(status == true){
    auto pi_status = pi_packetout_send(raw_packet.data(),raw_packet.size());
  }
}

class ConnectionId {
 public:
  using Id = Uint128;

  static Id get() {
    auto &instance_ = instance();
    return instance_.current_id++;
  }

 private:
  ConnectionId()
      : current_id(0, 1) { }

  static ConnectionId &instance() {
    static ConnectionId instance;
    return instance;
  }

  Id current_id;
};

class Connection {
 public:
  static std::unique_ptr<Connection> make(const Uint128 &election_id,
                                          StreamChannelReaderWriter *stream,
                                          ServerContext *context) {
    (void) context;
    return std::unique_ptr<Connection>(
        new Connection(ConnectionId::get(), election_id, stream));
  }

  const ConnectionId::Id &connection_id() const { return connection_id_; }
  const Uint128 &election_id() const { return election_id_; }
  StreamChannelReaderWriter *stream() const { return stream_; }

  void set_election_id(const Uint128 &election_id) {
    election_id_ = election_id;
  }

 private:
  Connection(ConnectionId::Id connection_id, const Uint128 &election_id,
             StreamChannelReaderWriter *stream)
      : connection_id_(connection_id), election_id_(election_id),
        stream_(stream) { }

  ConnectionId::Id connection_id_{0};
  Uint128 election_id_{0};
  StreamChannelReaderWriter *stream_{nullptr};
};

class P4RuntimeServiceImpl : public p4v1::P4Runtime::Service {
	private:
	grpc::Status Write(ServerContext *context,
               const p4v1::WriteRequest *request,
               p4v1::WriteResponse *rep) override {
    		SIMPLELOG << "P4Runtime Write\n";
    		SIMPLELOG << request->DebugString();
    		(void) rep;
    		SIMPLELOG << "Device ID:: " << request->device_id() << std::endl;
    		if (dm.has_p4info == 0) return no_pipeline_config_status();

		auto status = dev_mgr_write(&dm, *request, rep);
    		return status;
  	}

	grpc::Status Read(ServerContext *context,
               const p4v1::ReadRequest *request,
               ServerWriter<p4v1::ReadResponse> *writer) override {
    		SIMPLELOG << "P4Runtime Read\n";
    		//SIMPLELOG << request->DebugString();
    		p4v1::ReadResponse response;
		SIMPLELOG << "Read Device ID:: " << request->device_id() << std::endl;
                if (dm.has_p4info == 0) {
			return no_pipeline_config_status();
		}
    		auto status = dev_mgr_read(&dm, *request, &response);
                SIMPLELOG << response.DebugString();
    		writer->Write(response);
    		return status;
  	}
	
	grpc::Status SetForwardingPipelineConfig(ServerContext *context,
	       const p4v1::SetForwardingPipelineConfigRequest *request,
	       p4v1::SetForwardingPipelineConfigResponse *rep) override {
    		SIMPLELOG << "P4Runtime SetForwardingPipelineConfig\n";
        // SIMPLELOG << request->DebugString();
    		(void) rep;
		SIMPLELOG << "Set config Device ID:: " << request->device_id() << std::endl;
		dm.device_id = request->device_id();
    		auto status = dev_mgr_set_pipeline_config(&dm, request->action(), request->config());
        // stream_message_response_register_cb(stream_message_response_cb, NULL);
    		// device_mgr->stream_message_response_register_cb(stream_message_response_cb, NULL);
    		return status;
  	}

	grpc::Status GetForwardingPipelineConfig(ServerContext *context,
               const p4v1::GetForwardingPipelineConfigRequest *request,
               p4v1::GetForwardingPipelineConfigResponse *rep) override {
		SIMPLELOG << "P4Runtime GetForwardingPipelineConfig\n";
		SIMPLELOG << request->DebugString();
		SIMPLELOG << "Get config Device ID:: " << request->device_id() << std::endl;
    		if (dm.has_p4info == 0) {
			SIMPLELOG << " -- No pipeline config status\n";
			return no_pipeline_config_status();
		}
		auto status = dev_mgr_get_pipeline_config(&dm, request->response_type(), rep->mutable_config());
    		return status;
  	}

	grpc::Status StreamChannel(ServerContext *context,
               StreamChannelReaderWriter *stream) override {
    
    // struct Connection_Status {
    //                explicit Connection_Status(ServerContext *context)
    //                 : context(context) { }
    //                 ~Connection_Status(){
    //                 }
                    
    //                 ServerContext *context;
    //                 std::unique_ptr<Connection> connection{nullptr};
    //                 DeviceMgr::device_id_t device_id{0};
    //              };
    //              Connection_Status connection_status(context);
                 
		p4v1::StreamMessageRequest request;
		SIMPLELOG << "P4Runtime StreamChannel\n";
		while (stream->Read(&request)) {
			SIMPLELOG << request.DebugString();
      			switch (request.update_case()) {
        		 case p4v1::StreamMessageRequest::kArbitration:
          		 {
            //       auto device_id = request.arbitration().device_id();
            // auto election_id = convert_u128(
            //     request.arbitration().election_id());
            // // TODO(antonin): a lot of existing code will break if 0 is not
            // // valid anymore
            // // if (election_id == 0) {
            // //   return Status(StatusCode::INVALID_ARGUMENT,
            // //                 "Invalid election id value");
            // // }
            // auto connection = connection_status.connection.get();
            // if (connection != nullptr &&
            //     connection_status.device_id != device_id) {
            //   return Status(StatusCode::FAILED_PRECONDITION,
            //                 "Invalid device id");
            // }
            // if (connection == nullptr) {
            //   connection_status.connection = Connection::make(
            //       election_id, stream, context);
            //   connection_status.device_id = device_id;
            // } 
            		  auto device_id = request.arbitration().device_id();
            		  if (dm.device_id != device_id) {
				SIMPLELOG << "Device ID:: " << device_id << " expected: " << dm.device_id << std::endl;
              			return grpc::Status(StatusCode::FAILED_PRECONDITION,
                                "Invalid device id");
            		  }
			  auto election_id = convert_u128(request.arbitration().election_id());
			  notify_one(stream, device_id, election_id);
              		  // return grpc::Status::OK; //, "Invalid");
                         }
          		 break;
        		 case p4v1::StreamMessageRequest::kPacket:{
                  process_stream_message_request(&dm, request);      
             }
             break;
        		 case p4v1::StreamMessageRequest::kDigestAck:
          		 {
            		 //auto device_id = connection_status.device_id;
            		 //Devices::get(device_id)->process_stream_message_request(
                	 //connection_status.connection.get(), request);
                  //  process_stream_message_request(request);
          		 }
          		 break;
        		 default:
           		 break;
      			}
    		}
    		return Status::OK;
       }

        // Status Capabilities(ServerContext* context,
        //             const p4v1::CapabilitiesRequest *request,
        //             p4v1::CapabilitiesResponse *rep) override {
        // (void) request;
        //  rep->set_p4runtime_api_version(p4runtime_api_version);
        //   return Status::OK;
        // }

       static Uint128 convert_u128(const p4v1::Uint128 &from) {
	return Uint128(from.high(), from.low());
       }
	

};

/*
class ServerConfigServiceImpl : public p4serverv1::ServerConfig::Service {
 private:
  Status Set(ServerContext *context,
             const p4serverv1::SetRequest *request,
             p4serverv1::SetResponse *response) override {
	SIMPLELOG << "ServerConfigServiceImpl::Set\n";
    	//(void) response;
    	//auto device = Devices::get(request->device_id());
    	return Status::OK; //device->set_server_config(request->config());
  }

  Status Get(ServerContext *context,
             const p4serverv1::GetRequest *request,
             p4serverv1::GetResponse *response) override {
	SIMPLELOG << "ServerConfigServiceImpl::Set\n";
    	//auto device = Devices::get(request->device_id());
    	//response->mutable_config()->CopyFrom(device->get_server_config());
    	return Status::OK;
  }
};
*/
// class DeviceState;
// void DeviceState::send_stream_message(p4v1::StreamMessageResponse *msg) {
//     // auto master = get_master();
//     // if (master == nullptr) return;
//     auto stream = master->stream();
//     auto success = stream->Write(*msg);
//     if (msg->has_packet() && success) {
//       SIMPLELOG << "PACKET IN\n";
//       pkt_in_count++;
//     }
//   }
struct ServerData {
  std::string server_address;
  int server_port;
  P4RuntimeServiceImpl pi_service;
  //std::unique_ptr<gnmi::gNMI::Service> gnmi_service;
  //ServerConfigServiceImpl server_config_service;
  ServerBuilder builder;
  std::unique_ptr<Server> server;
};

ServerData *server_data;
/*
TODO by Ian 
实现server端的grpc链接及创建CALL实例
*/
// AsyncServerData *asyn_server_data_in = new AsyncServerData();
// AsyncServerData *asyn_server_data_out = new AsyncServerData();
extern "C" {

void PIGrpcServerRunAddrGnmi(const char *server_address, void *gnmi_service) 
{
  server_data = new ServerData();
  server_data->server_address = std::string(server_address);
  auto &builder = server_data->builder;
  builder.AddListeningPort(
    server_data->server_address, grpc::InsecureServerCredentials(),
    &server_data->server_port);
  builder.RegisterService(&server_data->pi_service);
// builder.RegisterService(server_data->gnmi_service.get());
//  builder.RegisterService(&server_data->server_config_service);
  builder.SetMaxReceiveMessageSize(256*1024*1024);  // 256MB

  server_data->server = builder.BuildAndStart();
  std::cout << "Server listening on " << server_data->server_address << "\n" << std::endl;
}

// void * handleRpc_out (void *arg)//这里是自己定义的要执行的方法它是指针函数    1，返回类型void 2，参数类型为void *
// {
// 	char new_data1[] = "hello,client";
//     uint8_t* data1 = (uint8_t*)new_data1;
// 	int len;
// 	len = sizeof(new_data1)/sizeof(char);
// 	//如果测试packetin，则应该关闭这个，因为这个是read model，cq里的类型是读
//     new StreamChannelImpl(&(asyn_server_data_out->service), asyn_server_data_out->cq.get(), data1, false, false, len);
//    	void* tag; // uniquely identifies a request.
//     bool ok;
//     while (true)
//     {
// 		// GPR_ASSERT(asyn_server_data_out->cq->Next(&tag, &ok));
// 		asyn_server_data_out->cq->Next(&tag, &ok);
// 		// SIMPLELOG << "OK == " << ok << std::endl;
// 		static_cast<CommonAsyncCallData*>(tag)->Proceed(ok);				
//     }
// 	return NULL;
// }

// void * handleRpc_in (void *arg)//这里是自己定义的要执行的方法它是指针函数    1，返回类型void 2，参数类型为void *
// {
//    	void* tag; // uniquely identifies a request.
//     bool ok;
//     while (true)
//     {
// 		// GPR_ASSERT(asyn_server_data_in->cq->Next(&tag, &ok));
// 		asyn_server_data_in->cq->Next(&tag, &ok);
// 		static_cast<CommonAsyncCallData*>(tag)->Proceed(ok);				
//     }
// 	return NULL;
// }

// void PIGrpcServerRunAddrin(const char *server_address) {
//   asyn_server_data_in->server_address = std::string(server_address);
//   auto &builder = asyn_server_data_in->builder;
//   builder.AddListeningPort(
//     asyn_server_data_in->server_address, grpc::InsecureServerCredentials(),
//     &asyn_server_data_in->server_port);
//   asyn_server_data_in->cq = builder.AddCompletionQueue();
//   builder.RegisterService(&asyn_server_data_in->service);
// //  builder.RegisterService(&server_data->server_config_service);
//   builder.SetMaxReceiveMessageSize(256*1024*1024);  // 256MB
//   asyn_server_data_in->server = builder.BuildAndStart();
// //   CallStatus status = CREATE;
// //   uint8_t* data;
// //   new StreamChannelImpl(&asyn_server_data_in->service, cq.get(), data, status);
// 	SIMPLELOG << "Server listening on " << asyn_server_data_in->server_address << " \n " << std::endl;
// 	// std::thread thread_ = std::thread(&P4RuntimeClient::AsyncCompleteRpc, &client);
	
// 	int ret_in = 0;
//     pthread_t tid_in;//pthread_t ==> long
//     //1.线程的创建 ---线程创建之后 当前程序中就有两个执行体
//     //               (1).主线程 -- 即:main函数中代表的执行流
//     //               (2).子线程 -- 即:执行函数代表的执行流                                        
//     ret_in = pthread_create(&tid_in,NULL,handleRpc_in,NULL); //attr == NULL表示默认属性(可结合属性)
//     if(ret_in!=0)
//     {
//         errno = ret_in;//设置错误码
//         perror("pthread_create_in fail");
//         exit(EXIT_SUCCESS);
//     }
// }

// void PIGrpcServerRunAddrout(const char *server_address) {
//   asyn_server_data_out->server_address = std::string(server_address);
//   auto &builder = asyn_server_data_out->builder;
//   builder.AddListeningPort(
//     asyn_server_data_out->server_address, grpc::InsecureServerCredentials(),
//     &asyn_server_data_out->server_port);
//   asyn_server_data_out->cq = builder.AddCompletionQueue();
//   builder.RegisterService(&asyn_server_data_out->service);
// //  builder.RegisterService(&server_data->server_config_service);
//   builder.SetMaxReceiveMessageSize(256*1024*1024);  // 256MB
//   asyn_server_data_out->server = builder.BuildAndStart();
// //   CallStatus status = CREATE;
// //   uint8_t* data;
// //   new StreamChannelImpl(&asyn_server_data_out->service, cq.get(), data, status);
// 	SIMPLELOG << "Server listening on " << asyn_server_data_out->server_address << " \n " << std::endl;
// 	// std::thread thread_ = std::thread(&P4RuntimeClient::AsyncCompleteRpc, &client);
	
// 	int ret_out = 0;
//     pthread_t tid_out;//pthread_t ==> long
//     //1.线程的创建 ---线程创建之后 当前程序中就有两个执行体
//     //               (1).主线程 -- 即:main函数中代表的执行流
//     //               (2).子线程 -- 即:执行函数代表的执行流                                        
//     ret_out = pthread_create(&tid_out,NULL,handleRpc_out,NULL); //attr == NULL表示默认属性(可结合属性)
//     if(ret_out!=0)
//     {
//         errno = ret_out;//设置错误码
//         perror("pthread_create fail");
//         exit(EXIT_SUCCESS);
//     }
// }

void PIGrpcServerRunAddr(const char *server_address) {
  PIGrpcServerRunAddrGnmi(server_address, 0);
}

void PIGrpcServerRun() {
  PIGrpcServerRunAddrGnmi("127.0.0.1:50051", 0);
}

int PIGrpcServerGetPort() {
  return server_data->server_port;
}

uint64_t PIGrpcServerGetPacketInCount(uint64_t device_id) {
  return 0;
}

uint64_t PIGrpcServerGetPacketOutCount(uint64_t device_id) {
  return 0;
}

void PIGrpcServerWait() {
  server_data->server->Wait();
}

void PIGrpcServerShutdown() {
  server_data->server->Shutdown();
}

void PIGrpcServerForceShutdown(int deadline_seconds) {
  using clock = std::chrono::system_clock;
  auto deadline = clock::now() + std::chrono::seconds(deadline_seconds);
  server_data->server->Shutdown(deadline);  
}

void PIGrpcServerCleanup() {
  delete server_data;
}

} // extern "C"
