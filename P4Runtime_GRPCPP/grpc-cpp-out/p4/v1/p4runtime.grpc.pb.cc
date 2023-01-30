// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: p4/v1/p4runtime.proto

#include "p4/v1/p4runtime.pb.h"
#include "p4/v1/p4runtime.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace p4 {
namespace v1 {

static const char* P4Runtime_method_names[] = {
  "/p4.v1.P4Runtime/Write",
  "/p4.v1.P4Runtime/Read",
  "/p4.v1.P4Runtime/SetForwardingPipelineConfig",
  "/p4.v1.P4Runtime/GetForwardingPipelineConfig",
  "/p4.v1.P4Runtime/StreamChannel",
  "/p4.v1.P4Runtime/Capabilities",
};

std::unique_ptr< P4Runtime::Stub> P4Runtime::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< P4Runtime::Stub> stub(new P4Runtime::Stub(channel));
  return stub;
}

P4Runtime::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_Write_(P4Runtime_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Read_(P4Runtime_method_names[1], ::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  , rpcmethod_SetForwardingPipelineConfig_(P4Runtime_method_names[2], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetForwardingPipelineConfig_(P4Runtime_method_names[3], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_StreamChannel_(P4Runtime_method_names[4], ::grpc::internal::RpcMethod::BIDI_STREAMING, channel)
  , rpcmethod_Capabilities_(P4Runtime_method_names[5], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status P4Runtime::Stub::Write(::grpc::ClientContext* context, const ::p4::v1::WriteRequest& request, ::p4::v1::WriteResponse* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Write_, context, request, response);
}

void P4Runtime::Stub::experimental_async::Write(::grpc::ClientContext* context, const ::p4::v1::WriteRequest* request, ::p4::v1::WriteResponse* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Write_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::p4::v1::WriteResponse>* P4Runtime::Stub::AsyncWriteRaw(::grpc::ClientContext* context, const ::p4::v1::WriteRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::p4::v1::WriteResponse>::Create(channel_.get(), cq, rpcmethod_Write_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::p4::v1::WriteResponse>* P4Runtime::Stub::PrepareAsyncWriteRaw(::grpc::ClientContext* context, const ::p4::v1::WriteRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::p4::v1::WriteResponse>::Create(channel_.get(), cq, rpcmethod_Write_, context, request, false);
}

::grpc::ClientReader< ::p4::v1::ReadResponse>* P4Runtime::Stub::ReadRaw(::grpc::ClientContext* context, const ::p4::v1::ReadRequest& request) {
  return ::grpc::internal::ClientReaderFactory< ::p4::v1::ReadResponse>::Create(channel_.get(), rpcmethod_Read_, context, request);
}

::grpc::ClientAsyncReader< ::p4::v1::ReadResponse>* P4Runtime::Stub::AsyncReadRaw(::grpc::ClientContext* context, const ::p4::v1::ReadRequest& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::p4::v1::ReadResponse>::Create(channel_.get(), cq, rpcmethod_Read_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::p4::v1::ReadResponse>* P4Runtime::Stub::PrepareAsyncReadRaw(::grpc::ClientContext* context, const ::p4::v1::ReadRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::p4::v1::ReadResponse>::Create(channel_.get(), cq, rpcmethod_Read_, context, request, false, nullptr);
}

::grpc::Status P4Runtime::Stub::SetForwardingPipelineConfig(::grpc::ClientContext* context, const ::p4::v1::SetForwardingPipelineConfigRequest& request, ::p4::v1::SetForwardingPipelineConfigResponse* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_SetForwardingPipelineConfig_, context, request, response);
}

void P4Runtime::Stub::experimental_async::SetForwardingPipelineConfig(::grpc::ClientContext* context, const ::p4::v1::SetForwardingPipelineConfigRequest* request, ::p4::v1::SetForwardingPipelineConfigResponse* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_SetForwardingPipelineConfig_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::p4::v1::SetForwardingPipelineConfigResponse>* P4Runtime::Stub::AsyncSetForwardingPipelineConfigRaw(::grpc::ClientContext* context, const ::p4::v1::SetForwardingPipelineConfigRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::p4::v1::SetForwardingPipelineConfigResponse>::Create(channel_.get(), cq, rpcmethod_SetForwardingPipelineConfig_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::p4::v1::SetForwardingPipelineConfigResponse>* P4Runtime::Stub::PrepareAsyncSetForwardingPipelineConfigRaw(::grpc::ClientContext* context, const ::p4::v1::SetForwardingPipelineConfigRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::p4::v1::SetForwardingPipelineConfigResponse>::Create(channel_.get(), cq, rpcmethod_SetForwardingPipelineConfig_, context, request, false);
}

::grpc::Status P4Runtime::Stub::GetForwardingPipelineConfig(::grpc::ClientContext* context, const ::p4::v1::GetForwardingPipelineConfigRequest& request, ::p4::v1::GetForwardingPipelineConfigResponse* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_GetForwardingPipelineConfig_, context, request, response);
}

void P4Runtime::Stub::experimental_async::GetForwardingPipelineConfig(::grpc::ClientContext* context, const ::p4::v1::GetForwardingPipelineConfigRequest* request, ::p4::v1::GetForwardingPipelineConfigResponse* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_GetForwardingPipelineConfig_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::p4::v1::GetForwardingPipelineConfigResponse>* P4Runtime::Stub::AsyncGetForwardingPipelineConfigRaw(::grpc::ClientContext* context, const ::p4::v1::GetForwardingPipelineConfigRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::p4::v1::GetForwardingPipelineConfigResponse>::Create(channel_.get(), cq, rpcmethod_GetForwardingPipelineConfig_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::p4::v1::GetForwardingPipelineConfigResponse>* P4Runtime::Stub::PrepareAsyncGetForwardingPipelineConfigRaw(::grpc::ClientContext* context, const ::p4::v1::GetForwardingPipelineConfigRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::p4::v1::GetForwardingPipelineConfigResponse>::Create(channel_.get(), cq, rpcmethod_GetForwardingPipelineConfig_, context, request, false);
}

::grpc::ClientReaderWriter< ::p4::v1::StreamMessageRequest, ::p4::v1::StreamMessageResponse>* P4Runtime::Stub::StreamChannelRaw(::grpc::ClientContext* context) {
  return ::grpc::internal::ClientReaderWriterFactory< ::p4::v1::StreamMessageRequest, ::p4::v1::StreamMessageResponse>::Create(channel_.get(), rpcmethod_StreamChannel_, context);
}

::grpc::ClientAsyncReaderWriter< ::p4::v1::StreamMessageRequest, ::p4::v1::StreamMessageResponse>* P4Runtime::Stub::AsyncStreamChannelRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderWriterFactory< ::p4::v1::StreamMessageRequest, ::p4::v1::StreamMessageResponse>::Create(channel_.get(), cq, rpcmethod_StreamChannel_, context, true, tag);
}

::grpc::ClientAsyncReaderWriter< ::p4::v1::StreamMessageRequest, ::p4::v1::StreamMessageResponse>* P4Runtime::Stub::PrepareAsyncStreamChannelRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderWriterFactory< ::p4::v1::StreamMessageRequest, ::p4::v1::StreamMessageResponse>::Create(channel_.get(), cq, rpcmethod_StreamChannel_, context, false, nullptr);
}

::grpc::Status P4Runtime::Stub::Capabilities(::grpc::ClientContext* context, const ::p4::v1::CapabilitiesRequest& request, ::p4::v1::CapabilitiesResponse* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Capabilities_, context, request, response);
}

void P4Runtime::Stub::experimental_async::Capabilities(::grpc::ClientContext* context, const ::p4::v1::CapabilitiesRequest* request, ::p4::v1::CapabilitiesResponse* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Capabilities_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::p4::v1::CapabilitiesResponse>* P4Runtime::Stub::AsyncCapabilitiesRaw(::grpc::ClientContext* context, const ::p4::v1::CapabilitiesRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::p4::v1::CapabilitiesResponse>::Create(channel_.get(), cq, rpcmethod_Capabilities_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::p4::v1::CapabilitiesResponse>* P4Runtime::Stub::PrepareAsyncCapabilitiesRaw(::grpc::ClientContext* context, const ::p4::v1::CapabilitiesRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::p4::v1::CapabilitiesResponse>::Create(channel_.get(), cq, rpcmethod_Capabilities_, context, request, false);
}

P4Runtime::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      P4Runtime_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< P4Runtime::Service, ::p4::v1::WriteRequest, ::p4::v1::WriteResponse>(
          std::mem_fn(&P4Runtime::Service::Write), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      P4Runtime_method_names[1],
      ::grpc::internal::RpcMethod::SERVER_STREAMING,
      new ::grpc::internal::ServerStreamingHandler< P4Runtime::Service, ::p4::v1::ReadRequest, ::p4::v1::ReadResponse>(
          std::mem_fn(&P4Runtime::Service::Read), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      P4Runtime_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< P4Runtime::Service, ::p4::v1::SetForwardingPipelineConfigRequest, ::p4::v1::SetForwardingPipelineConfigResponse>(
          std::mem_fn(&P4Runtime::Service::SetForwardingPipelineConfig), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      P4Runtime_method_names[3],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< P4Runtime::Service, ::p4::v1::GetForwardingPipelineConfigRequest, ::p4::v1::GetForwardingPipelineConfigResponse>(
          std::mem_fn(&P4Runtime::Service::GetForwardingPipelineConfig), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      P4Runtime_method_names[4],
      ::grpc::internal::RpcMethod::BIDI_STREAMING,
      new ::grpc::internal::BidiStreamingHandler< P4Runtime::Service, ::p4::v1::StreamMessageRequest, ::p4::v1::StreamMessageResponse>(
          std::mem_fn(&P4Runtime::Service::StreamChannel), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      P4Runtime_method_names[5],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< P4Runtime::Service, ::p4::v1::CapabilitiesRequest, ::p4::v1::CapabilitiesResponse>(
          std::mem_fn(&P4Runtime::Service::Capabilities), this)));
}

P4Runtime::Service::~Service() {
}

::grpc::Status P4Runtime::Service::Write(::grpc::ServerContext* context, const ::p4::v1::WriteRequest* request, ::p4::v1::WriteResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status P4Runtime::Service::Read(::grpc::ServerContext* context, const ::p4::v1::ReadRequest* request, ::grpc::ServerWriter< ::p4::v1::ReadResponse>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status P4Runtime::Service::SetForwardingPipelineConfig(::grpc::ServerContext* context, const ::p4::v1::SetForwardingPipelineConfigRequest* request, ::p4::v1::SetForwardingPipelineConfigResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status P4Runtime::Service::GetForwardingPipelineConfig(::grpc::ServerContext* context, const ::p4::v1::GetForwardingPipelineConfigRequest* request, ::p4::v1::GetForwardingPipelineConfigResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status P4Runtime::Service::StreamChannel(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::p4::v1::StreamMessageResponse, ::p4::v1::StreamMessageRequest>* stream) {
  (void) context;
  (void) stream;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status P4Runtime::Service::Capabilities(::grpc::ServerContext* context, const ::p4::v1::CapabilitiesRequest* request, ::p4::v1::CapabilitiesResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace p4
}  // namespace v1

