// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: coordinator.proto

#include "coordinator.pb.h"
#include "coordinator.grpc.pb.h"

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

static const char* CoordService_method_names[] = {
  "/csce438.CoordService/Heartbeat",
  "/csce438.CoordService/GetServer",
  "/csce438.CoordService/GetBackupServer",
  "/csce438.CoordService/GetFollowerSyncer",
  "/csce438.CoordService/RegisterServer",
  "/csce438.CoordService/RegisterFollowerSyncer",
};

std::unique_ptr< CoordService::Stub> CoordService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< CoordService::Stub> stub(new CoordService::Stub(channel, options));
  return stub;
}

CoordService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_Heartbeat_(CoordService_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetServer_(CoordService_method_names[1], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetBackupServer_(CoordService_method_names[2], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetFollowerSyncer_(CoordService_method_names[3], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_RegisterServer_(CoordService_method_names[4], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_RegisterFollowerSyncer_(CoordService_method_names[5], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status CoordService::Stub::Heartbeat(::grpc::ClientContext* context, const ::csce438::ServerInfo& request, ::csce438::Confirmation* response) {
  return ::grpc::internal::BlockingUnaryCall< ::csce438::ServerInfo, ::csce438::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_Heartbeat_, context, request, response);
}

void CoordService::Stub::async::Heartbeat(::grpc::ClientContext* context, const ::csce438::ServerInfo* request, ::csce438::Confirmation* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::csce438::ServerInfo, ::csce438::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Heartbeat_, context, request, response, std::move(f));
}

void CoordService::Stub::async::Heartbeat(::grpc::ClientContext* context, const ::csce438::ServerInfo* request, ::csce438::Confirmation* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Heartbeat_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::csce438::Confirmation>* CoordService::Stub::PrepareAsyncHeartbeatRaw(::grpc::ClientContext* context, const ::csce438::ServerInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::csce438::Confirmation, ::csce438::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_Heartbeat_, context, request);
}

::grpc::ClientAsyncResponseReader< ::csce438::Confirmation>* CoordService::Stub::AsyncHeartbeatRaw(::grpc::ClientContext* context, const ::csce438::ServerInfo& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncHeartbeatRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status CoordService::Stub::GetServer(::grpc::ClientContext* context, const ::csce438::ID& request, ::csce438::ServerInfo* response) {
  return ::grpc::internal::BlockingUnaryCall< ::csce438::ID, ::csce438::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_GetServer_, context, request, response);
}

void CoordService::Stub::async::GetServer(::grpc::ClientContext* context, const ::csce438::ID* request, ::csce438::ServerInfo* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::csce438::ID, ::csce438::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetServer_, context, request, response, std::move(f));
}

void CoordService::Stub::async::GetServer(::grpc::ClientContext* context, const ::csce438::ID* request, ::csce438::ServerInfo* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetServer_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::csce438::ServerInfo>* CoordService::Stub::PrepareAsyncGetServerRaw(::grpc::ClientContext* context, const ::csce438::ID& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::csce438::ServerInfo, ::csce438::ID, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_GetServer_, context, request);
}

::grpc::ClientAsyncResponseReader< ::csce438::ServerInfo>* CoordService::Stub::AsyncGetServerRaw(::grpc::ClientContext* context, const ::csce438::ID& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncGetServerRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status CoordService::Stub::GetBackupServer(::grpc::ClientContext* context, const ::csce438::ID& request, ::csce438::ServerInfo* response) {
  return ::grpc::internal::BlockingUnaryCall< ::csce438::ID, ::csce438::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_GetBackupServer_, context, request, response);
}

void CoordService::Stub::async::GetBackupServer(::grpc::ClientContext* context, const ::csce438::ID* request, ::csce438::ServerInfo* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::csce438::ID, ::csce438::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetBackupServer_, context, request, response, std::move(f));
}

void CoordService::Stub::async::GetBackupServer(::grpc::ClientContext* context, const ::csce438::ID* request, ::csce438::ServerInfo* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetBackupServer_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::csce438::ServerInfo>* CoordService::Stub::PrepareAsyncGetBackupServerRaw(::grpc::ClientContext* context, const ::csce438::ID& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::csce438::ServerInfo, ::csce438::ID, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_GetBackupServer_, context, request);
}

::grpc::ClientAsyncResponseReader< ::csce438::ServerInfo>* CoordService::Stub::AsyncGetBackupServerRaw(::grpc::ClientContext* context, const ::csce438::ID& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncGetBackupServerRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status CoordService::Stub::GetFollowerSyncer(::grpc::ClientContext* context, const ::csce438::ID& request, ::csce438::ServerInfo* response) {
  return ::grpc::internal::BlockingUnaryCall< ::csce438::ID, ::csce438::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_GetFollowerSyncer_, context, request, response);
}

void CoordService::Stub::async::GetFollowerSyncer(::grpc::ClientContext* context, const ::csce438::ID* request, ::csce438::ServerInfo* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::csce438::ID, ::csce438::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetFollowerSyncer_, context, request, response, std::move(f));
}

void CoordService::Stub::async::GetFollowerSyncer(::grpc::ClientContext* context, const ::csce438::ID* request, ::csce438::ServerInfo* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetFollowerSyncer_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::csce438::ServerInfo>* CoordService::Stub::PrepareAsyncGetFollowerSyncerRaw(::grpc::ClientContext* context, const ::csce438::ID& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::csce438::ServerInfo, ::csce438::ID, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_GetFollowerSyncer_, context, request);
}

::grpc::ClientAsyncResponseReader< ::csce438::ServerInfo>* CoordService::Stub::AsyncGetFollowerSyncerRaw(::grpc::ClientContext* context, const ::csce438::ID& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncGetFollowerSyncerRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status CoordService::Stub::RegisterServer(::grpc::ClientContext* context, const ::csce438::ServerInfo& request, ::csce438::Confirmation* response) {
  return ::grpc::internal::BlockingUnaryCall< ::csce438::ServerInfo, ::csce438::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_RegisterServer_, context, request, response);
}

void CoordService::Stub::async::RegisterServer(::grpc::ClientContext* context, const ::csce438::ServerInfo* request, ::csce438::Confirmation* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::csce438::ServerInfo, ::csce438::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_RegisterServer_, context, request, response, std::move(f));
}

void CoordService::Stub::async::RegisterServer(::grpc::ClientContext* context, const ::csce438::ServerInfo* request, ::csce438::Confirmation* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_RegisterServer_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::csce438::Confirmation>* CoordService::Stub::PrepareAsyncRegisterServerRaw(::grpc::ClientContext* context, const ::csce438::ServerInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::csce438::Confirmation, ::csce438::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_RegisterServer_, context, request);
}

::grpc::ClientAsyncResponseReader< ::csce438::Confirmation>* CoordService::Stub::AsyncRegisterServerRaw(::grpc::ClientContext* context, const ::csce438::ServerInfo& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncRegisterServerRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status CoordService::Stub::RegisterFollowerSyncer(::grpc::ClientContext* context, const ::csce438::ServerInfo& request, ::csce438::Confirmation* response) {
  return ::grpc::internal::BlockingUnaryCall< ::csce438::ServerInfo, ::csce438::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_RegisterFollowerSyncer_, context, request, response);
}

void CoordService::Stub::async::RegisterFollowerSyncer(::grpc::ClientContext* context, const ::csce438::ServerInfo* request, ::csce438::Confirmation* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::csce438::ServerInfo, ::csce438::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_RegisterFollowerSyncer_, context, request, response, std::move(f));
}

void CoordService::Stub::async::RegisterFollowerSyncer(::grpc::ClientContext* context, const ::csce438::ServerInfo* request, ::csce438::Confirmation* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_RegisterFollowerSyncer_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::csce438::Confirmation>* CoordService::Stub::PrepareAsyncRegisterFollowerSyncerRaw(::grpc::ClientContext* context, const ::csce438::ServerInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::csce438::Confirmation, ::csce438::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_RegisterFollowerSyncer_, context, request);
}

::grpc::ClientAsyncResponseReader< ::csce438::Confirmation>* CoordService::Stub::AsyncRegisterFollowerSyncerRaw(::grpc::ClientContext* context, const ::csce438::ServerInfo& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncRegisterFollowerSyncerRaw(context, request, cq);
  result->StartCall();
  return result;
}

CoordService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      CoordService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< CoordService::Service, ::csce438::ServerInfo, ::csce438::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](CoordService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::csce438::ServerInfo* req,
             ::csce438::Confirmation* resp) {
               return service->Heartbeat(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      CoordService_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< CoordService::Service, ::csce438::ID, ::csce438::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](CoordService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::csce438::ID* req,
             ::csce438::ServerInfo* resp) {
               return service->GetServer(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      CoordService_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< CoordService::Service, ::csce438::ID, ::csce438::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](CoordService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::csce438::ID* req,
             ::csce438::ServerInfo* resp) {
               return service->GetBackupServer(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      CoordService_method_names[3],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< CoordService::Service, ::csce438::ID, ::csce438::ServerInfo, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](CoordService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::csce438::ID* req,
             ::csce438::ServerInfo* resp) {
               return service->GetFollowerSyncer(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      CoordService_method_names[4],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< CoordService::Service, ::csce438::ServerInfo, ::csce438::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](CoordService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::csce438::ServerInfo* req,
             ::csce438::Confirmation* resp) {
               return service->RegisterServer(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      CoordService_method_names[5],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< CoordService::Service, ::csce438::ServerInfo, ::csce438::Confirmation, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](CoordService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::csce438::ServerInfo* req,
             ::csce438::Confirmation* resp) {
               return service->RegisterFollowerSyncer(ctx, req, resp);
             }, this)));
}

CoordService::Service::~Service() {
}

::grpc::Status CoordService::Service::Heartbeat(::grpc::ServerContext* context, const ::csce438::ServerInfo* request, ::csce438::Confirmation* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status CoordService::Service::GetServer(::grpc::ServerContext* context, const ::csce438::ID* request, ::csce438::ServerInfo* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status CoordService::Service::GetBackupServer(::grpc::ServerContext* context, const ::csce438::ID* request, ::csce438::ServerInfo* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status CoordService::Service::GetFollowerSyncer(::grpc::ServerContext* context, const ::csce438::ID* request, ::csce438::ServerInfo* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status CoordService::Service::RegisterServer(::grpc::ServerContext* context, const ::csce438::ServerInfo* request, ::csce438::Confirmation* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status CoordService::Service::RegisterFollowerSyncer(::grpc::ServerContext* context, const ::csce438::ServerInfo* request, ::csce438::Confirmation* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace csce438

