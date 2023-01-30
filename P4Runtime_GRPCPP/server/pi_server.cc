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
#include <vector>
#include <iostream>
#include "uint128.h"
#include <cstdio>
#include <typeinfo>
#include "PI/proto/pi_server.h"
#include "device_mgr.h"
#include <arpa/inet.h>

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
    auto success = stream_store->Write(*msg);
    if (msg->has_packet() && success) {
      // std::cout << "PACKET IN\n";
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

      auto metadata = packet_in->add_metadata();
      metadata->set_metadata_id(1);

uint16_t prepend;
      memcpy(&prepend,pkt,nbytes);
      prepend = ntohs(prepend);
      prepend = prepend >> 7;
      prepend = htons(prepend);
      metadata->set_value(&prepend,2);

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

grpc::Status no_pipeline_config_status() {
  return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION,
                      "No forwarding pipeline config set for this device");
}

grpc::Status not_master_status() {
  return grpc::Status(grpc::StatusCode::PERMISSION_DENIED, "Not master");
}

void cb_mgr_add(cb_mgr_t *cb_mgr, GenericFnPtr cb,
                void *cb_cookie) {
  cb_mgr->default_cb.cb = cb;
  cb_mgr->default_cb.cookie = cb_cookie;
}

void notify_one(StreamChannelReaderWriter *stream, uint64_t device_id, Uint128 election_id) {
    stream_store = stream;
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
      // SIMPLELOG << "MASTER\n"; 
      status->set_code(::google::rpc::Code::OK);
      status->set_message("Is master");
    } else {
      // SIMPLELOG << "SLAVE\n";
      status->set_code(::google::rpc::Code::ALREADY_EXISTS);
      status->set_message("Is slave");
    }
    stream->Write(response);
}

grpc::Status pi_packetout_send(const std::string &pkt,size_t size) {
    struct p4_ctrl_msg ctrl_m;
		ctrl_m.type = P4T_PACKET_OUT;
    std::vector<char> myVector(pkt.begin(), pkt.end());
    ctrl_m.packet = &myVector[0];
		ctrl_m.len = size;
		dev_mgr_ptr->cb(&ctrl_m);
    return grpc::Status::OK;
}
bool packet_out_mutate(device_mgr_t *dm, const p4v1::PacketOut &packet_out, std::string &pkt){
    // struct p4_ctrl_msg *ctrl_m = new p4_ctrl_msg;
    std::string raw_packet;
    int nbytes = 2;
    struct p4_ctrl_msg ctrl_m;
    raw_packet.clear();
    const auto &payload = packet_out.payload();
    raw_packet.reserve(payload.size());
    raw_packet.append(payload);
    auto len = payload.length();

    for (const auto &metadata : packet_out.metadata()){
      int i = 0;
      auto metadata_id = metadata.metadata_id();
      ctrl_m.metadata[i] = new char[3];
      memcpy(ctrl_m.metadata[i], metadata.value().data(), 3);
      i++;
    }
    auto meta_size = packet_out.metadata_size();
    const auto &metadata = packet_out.metadata(0).value();
    auto metadata_id = packet_out.metadata(0).metadata_id();
  

   

    ctrl_m.packet = new char[len];
    // ctrl_m.metadata[0] = new char[10];
    memcpy(ctrl_m.packet, raw_packet.data(), len);
	  ctrl_m.len = len;
    ctrl_m.type = P4T_PACKET_OUT;
    dev_mgr_ptr->cb(&ctrl_m);
    return true;

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
      if (request.has_packet() != true)
      // SIMPLELOG << "PACKET OUT\n";
      // else
      SIMPLELOG << "has_packet is none\n";
    }
  auto packet = request.packet();
  std::string raw_packet;
  auto status = packet_out_mutate(dm,packet,raw_packet);

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
    		// SIMPLELOG << "P4Runtime Write\n";
    		// SIMPLELOG << request->DebugString();
    		(void) rep;
    		// SIMPLELOG << "Device ID:: " << request->device_id() << std::endl;
    		if (dm.has_p4info == 0) return no_pipeline_config_status();

		auto status = dev_mgr_write(&dm, *request, rep);
    		return status;
  	}

	grpc::Status Read(ServerContext *context,
               const p4v1::ReadRequest *request,
               ServerWriter<p4v1::ReadResponse> *writer) override {
    		// SIMPLELOG << "P4Runtime Read\n";
    		// SIMPLELOG << request->DebugString();
    		p4v1::ReadResponse response;
		// SIMPLELOG << "Device ID:: " << request->device_id() << std::endl;
                if (dm.has_p4info == 0) {
			return no_pipeline_config_status();
		}
    		auto status = dev_mgr_read(&dm, *request, &response);
    		writer->Write(response);
    		return status;
  	}
	
	grpc::Status SetForwardingPipelineConfig(ServerContext *context,
	       const p4v1::SetForwardingPipelineConfigRequest *request,
	       p4v1::SetForwardingPipelineConfigResponse *rep) override {
    		// SIMPLELOG << "P4Runtime SetForwardingPipelineConfig\n";
    		(void) rep;
		// SIMPLELOG << "Device ID:: " << request->device_id() << std::endl;
		dm.device_id = request->device_id();
    		auto status = dev_mgr_set_pipeline_config(&dm, request->action(), request->config());
    		return status;
  	}

	grpc::Status GetForwardingPipelineConfig(ServerContext *context,
               const p4v1::GetForwardingPipelineConfigRequest *request,
               p4v1::GetForwardingPipelineConfigResponse *rep) override {
		// SIMPLELOG << "P4Runtime GetForwardingPipelineConfig\n";
		// SIMPLELOG << request->DebugString();
		// SIMPLELOG << "Device ID:: " << request->device_id() << std::endl;
    		if (dm.has_p4info == 0) {
			// SIMPLELOG << " -- No pipeline config status\n";
			return no_pipeline_config_status();
		}
		auto status = dev_mgr_get_pipeline_config(&dm, request->response_type(), rep->mutable_config());
    		return status;
  	}

	grpc::Status StreamChannel(ServerContext *context,
               StreamChannelReaderWriter *stream) override {             
		p4v1::StreamMessageRequest request;
		// SIMPLELOG << "P4Runtime StreamChannel\n";
		while (stream->Read(&request)) {
			// SIMPLELOG << request.DebugString();
      			switch (request.update_case()) {
        		 case p4v1::StreamMessageRequest::kArbitration:
          		 {
            		  auto device_id = request.arbitration().device_id();
            		  if (dm.device_id != device_id) {
				// SIMPLELOG << "Device ID:: " << device_id << " expected: " << dm.device_id << std::endl;
              			return grpc::Status(StatusCode::FAILED_PRECONDITION,
                                "Invalid device id");
            		  }
			  auto election_id = convert_u128(request.arbitration().election_id());
			  notify_one(stream, device_id, election_id);
                         }
          		 break;
        		 case p4v1::StreamMessageRequest::kPacket:{
                  process_stream_message_request(&dm, request);      
             }
             break;
        		 case p4v1::StreamMessageRequest::kDigestAck:
          		 {

          		 }
          		 break;
        		 default:
           		 break;
      			}
    		}
    		return Status::OK;
       }

       static Uint128 convert_u128(const p4v1::Uint128 &from) {
	return Uint128(from.high(), from.low());
       }
	

};

struct ServerData {
  std::string server_address;
  int server_port;
  P4RuntimeServiceImpl pi_service;
  ServerBuilder builder;
  std::unique_ptr<Server> server;
};

ServerData *server_data;
switch_info_t switch_info;

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
  builder.SetMaxReceiveMessageSize(256*1024*1024);  // 256MB

  switch_info.device_id = dm.device_id;
  switch_info.grpc_server = server_data->server_address;
  switch_info.grpc_port = server_data->server_port;

  server_data->server = builder.BuildAndStart();
  std::cout << "Server listening on " << server_data->server_address << "\n" << std::endl;
}


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
