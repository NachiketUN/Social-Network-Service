// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: synchronizer.proto

#include "synchronizer.pb.h"
#include "synchronizer.grpc.pb.h"

#include <functional>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/impl/channel_interface.h>
#include <grpcpp/impl/client_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/rpc_service_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/sync_stream.h>
namespace csce438 {

static const char* SynchService_method_names[] = {
  "/csce438.SynchService/SendFollowingList",
  "/csce438.SynchService/SendTimeline",
  "/csce438.SynchService/ResynchServer",
};

std::unique_ptr< SynchService::Stub> SynchService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< SynchService::Stub> stub(new SynchService::Stub(channel, options));
  return stub;
}

SynchService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_SendFollowingList_(SynchService_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_SendTimeline_(SynchService_method_names[1], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_ResynchServer_(SynchService_method_names[2], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status SynchService::Stub::SendFollowingList(::grpc::ClientContext* context, const ::csce438::UserInfo& request, ::csce438::Confirmation* response) {
  return ::grpc::internal::BlockingUnaryCall< ::csce438::UserInfo, ::csce438::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_SendFollowingList_, context, request, response);
}

void SynchService::Stub::async::SendFollowingList(::grpc::ClientContext* context, const ::csce438::UserInfo* request, ::csce438::Confirmation* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::csce438::UserInfo, ::csce438::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_SendFollowingList_, context, request, response, std::move(f));
}

void SynchService::Stub::async::SendFollowingList(::grpc::ClientContext* context, const ::csce438::UserInfo* request, ::csce438::Confirmation* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_SendFollowingList_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::csce438::Confirmation>* SynchService::Stub::PrepareAsyncSendFollowingListRaw(::grpc::ClientContext* context, const ::csce438::UserInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::csce438::Confirmation, ::csce438::UserInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_SendFollowingList_, context, request);
}

::grpc::ClientAsyncResponseReader< ::csce438::Confirmation>* SynchService::Stub::AsyncSendFollowingListRaw(::grpc::ClientContext* context, const ::csce438::UserInfo& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncSendFollowingListRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status SynchService::Stub::SendTimeline(::grpc::ClientContext* context, const ::csce438::TimeInfo& request, ::csce438::Confirmation* response) {
  return ::grpc::internal::BlockingUnaryCall< ::csce438::TimeInfo, ::csce438::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_SendTimeline_, context, request, response);
}

void SynchService::Stub::async::SendTimeline(::grpc::ClientContext* context, const ::csce438::TimeInfo* request, ::csce438::Confirmation* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::csce438::TimeInfo, ::csce438::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_SendTimeline_, context, request, response, std::move(f));
}

void SynchService::Stub::async::SendTimeline(::grpc::ClientContext* context, const ::csce438::TimeInfo* request, ::csce438::Confirmation* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_SendTimeline_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::csce438::Confirmation>* SynchService::Stub::PrepareAsyncSendTimelineRaw(::grpc::ClientContext* context, const ::csce438::TimeInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::csce438::Confirmation, ::csce438::TimeInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_SendTimeline_, context, request);
}

::grpc::ClientAsyncResponseReader< ::csce438::Confirmation>* SynchService::Stub::AsyncSendTimelineRaw(::grpc::ClientContext* context, const ::csce438::TimeInfo& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncSendTimelineRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status SynchService::Stub::ResynchServer(::grpc::ClientContext* context, const ::csce438::ServerInfo& request, ::csce438::Confirmation* response) {
  return ::grpc::internal::BlockingUnaryCall< ::csce438::ServerInfo, ::csce438::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_ResynchServer_, context, request, response);
}

void SynchService::Stub::async::ResynchServer(::grpc::ClientContext* context, const ::csce438::ServerInfo* request, ::csce438::Confirmation* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::csce438::ServerInfo, ::csce438::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_ResynchServer_, context, request, response, std::move(f));
}

void SynchService::Stub::async::ResynchServer(::grpc::ClientContext* context, const ::csce438::ServerInfo* request, ::csce438::Confirmation* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_ResynchServer_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::csce438::Confirmation>* SynchService::Stub::PrepareAsyncResynchServerRaw(::grpc::ClientContext* context, const ::csce438::ServerInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::csce438::Confirmation, ::csce438::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_ResynchServer_, context, request);
}

::grpc::ClientAsyncResponseReader< ::csce438::Confirmation>* SynchService::Stub::AsyncResynchServerRaw(::grpc::ClientContext* context, const ::csce438::ServerInfo& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncResynchServerRaw(context, request, cq);
  result->StartCall();
  return result;
}

SynchService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      SynchService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< SynchService::Service, ::csce438::UserInfo, ::csce438::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](SynchService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::csce438::UserInfo* req,
             ::csce438::Confirmation* resp) {
               return service->SendFollowingList(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      SynchService_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< SynchService::Service, ::csce438::TimeInfo, ::csce438::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](SynchService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::csce438::TimeInfo* req,
             ::csce438::Confirmation* resp) {
               return service->SendTimeline(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      SynchService_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< SynchService::Service, ::csce438::ServerInfo, ::csce438::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](SynchService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::csce438::ServerInfo* req,
             ::csce438::Confirmation* resp) {
               return service->ResynchServer(ctx, req, resp);
             }, this)));
}

SynchService::Service::~Service() {
}

::grpc::Status SynchService::Service::SendFollowingList(::grpc::ServerContext* context, const ::csce438::UserInfo* request, ::csce438::Confirmation* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status SynchService::Service::SendTimeline(::grpc::ServerContext* context, const ::csce438::TimeInfo* request, ::csce438::Confirmation* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status SynchService::Service::ResynchServer(::grpc::ServerContext* context, const ::csce438::ServerInfo* request, ::csce438::Confirmation* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace csce438

