#include <ctime>

#include <google/protobuf/timestamp.pb.h>
#include <google/protobuf/duration.pb.h>
#include <chrono>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <stdlib.h>
#include <unistd.h>
#include <google/protobuf/util/time_util.h>
#include <grpc++/grpc++.h>

#include <glog/logging.h>
#define log(severity, msg) \
  LOG(severity) << msg;    \
  google::FlushLogFiles(google::severity);

#include "coordinator.grpc.pb.h"

using google::protobuf::Timestamp;
using google::protobuf::Duration;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using csce438::CoordService;
using csce438::ServerInfo;
using csce438::Confirmation;
using csce438::ID;
using namespace std;
// using csce438::ServerList;
// using csce438::SynchService;

struct zNode{
    int serverid;
    std::string hostname;
    std::string port;
    std::string type;
    std::time_t last_heartbeat;
    bool missed_heartbeat;
    bool isActive();

};

//potentially thread safe
std::mutex v_mutex;
std::vector<zNode> cluster1;
std::vector<zNode> cluster2;
std::vector<zNode> cluster3;
std::vector<zNode*> serverList;

//func declarations
int findServer(std::vector<zNode> v, int id);
std::time_t getTimeNow();
void checkHeartbeat();


bool zNode::isActive(){
    bool status = false;
    if(!missed_heartbeat){
        status = true;
    }else if(difftime(getTimeNow(),last_heartbeat) < 10){
        status = true;
    }
    return status;
}

class CoordServiceImpl final : public CoordService::Service {

  
  Status Heartbeat(ServerContext* context, const ServerInfo* serverinfo, Confirmation* confirmation) override {

    // Your code here
    int cluster_id = serverinfo->cluster();
    int serverid = serverinfo->serverid();
    zNode* node = nullptr;
    switch (cluster_id)
    {
      case 1:
        node = getServerFromCluster(&cluster1, serverid);
        break;
      case 2:
        node = getServerFromCluster(&cluster1, serverid);
      break;
      case 3:
              node = getServerFromCluster(&cluster1, serverid);
      break;
    
      default:
          log(INFO, "Did not find server during heartbeat rpc");
      break;
    }
    if(node != nullptr){
          node->last_heartbeat = getTimeNow();
          confirmation->set_status(true);
          LOG(INFO)<<"  Receieved HeartBeat from "<<to_string(serverid)<<std::endl;
    }
    else{
      confirmation->set_status(false);
    }
    
    return Status::OK;
  }
  
  //function returns the server information for requested client id
  //this function assumes there are always 3 clusters and has math
  //hardcoded to represent this.
  Status GetServer(ServerContext* context, const ID* id, ServerInfo* serverinfo) override {
    int serverID = (id->id()%3);
    serverID = (serverID?serverID:3);
    cout<<to_string(serverID);
    zNode *server = nullptr;
    switch (serverID)
    {
      case 1:
      for(auto z: cluster1){
        if(z.isActive()) {
          server = &z;
          break;
        }
      }
      break;
      case 2:
      for(auto z: cluster2){
        if(z.isActive()) {
          server = &z;
          break;
        }
      }
      break;
      case 3:
      for(auto z: cluster3){
        if(z.isActive()) {
          server = &z;
          break;
        }
      }
      break;
    
    default:

      break;
    }
    if(server == nullptr){
      cout<<"nullptr\n";
      return grpc::Status(grpc::StatusCode::NOT_FOUND, "No server is alive in cluster");
    }

    serverinfo->set_serverid(server->serverid);
    serverinfo->set_hostname(server->hostname);
    serverinfo->set_port(server->port);
    serverinfo->set_type(server->type);
    serverinfo->set_cluster(serverID);

    // Your code here
    // If server is active, return serverinfo
     cout<<"found server and sending back";
    return Status::OK;
  }

  Status RegisterServer(ServerContext* context, const ServerInfo* serverInfo, Confirmation* confirmation) override {
    int cluster_id = serverInfo->cluster();
    zNode new_node;
    new_node.serverid = serverInfo->serverid();
    new_node.hostname = serverInfo->hostname();
    new_node.port = serverInfo->port();
    new_node.type = serverInfo->type();
    new_node.last_heartbeat = getTimeNow();
    bool newEntry = false;
    switch (cluster_id)
    {
      case 1:
        if(!isPresent(&cluster1, new_node.serverid)){
          cluster1.push_back(new_node);
          newEntry = true;
        }
        break;
      case 2:
      if(!isPresent(&cluster2, new_node.serverid)){
          cluster2.push_back(new_node);
          newEntry = true;

        }
      break;
      case 3:
      if(!isPresent(&cluster3, new_node.serverid)){
          cluster3.push_back(new_node);
          newEntry = true;

        }
      break;
    
      default:
          log(INFO, "Wrong Cluster ID, skipping registeration");
      break;
    }
    if(newEntry){
      serverList.push_back(&new_node);
      log(INFO, "Server Registered Successfully");
    }
    else{
      log(INFO, "Server was already registered before");
    }
    for(auto z:cluster1){
      cout<<z.serverid<<endl;
    }
    return Status::OK;
  }

  zNode* getServerFromCluster(std::vector<zNode>* cluster, int serverid){
    for(auto& z: *cluster){
      if(z.serverid == serverid) return &z;
    }
    return nullptr;
  }
  bool isPresent(std::vector<zNode>* cluster, int serverid){
    for(auto z: *cluster){
      if(z.serverid == serverid) return true;
    }
    return false;
  }
  

};

void RunServer(std::string port_no){
  //start thread to check heartbeats
  std::thread hb(checkHeartbeat);
  //localhost = 127.0.0.1
  std::string server_address("127.0.0.1:"+port_no);
  CoordServiceImpl service;
  //grpc::EnableDefaultHealthCheckService(true);
  //grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  log(INFO, "Server starting...");

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  
  std::string port = "3010";
  int opt = 0;
  while ((opt = getopt(argc, argv, "p:")) != -1){
    switch(opt) {
      case 'p':
          port = optarg;
          break;
      default:
	std::cerr << "Invalid Command Line Argument\n";
    }
  }
  RunServer(port);
  return 0;
}



void checkHeartbeat(){
    while(true){
      //check servers for heartbeat > 10
      //if true turn missed heartbeat = true
      // Your code below
      for(auto& s : serverList){
      if(difftime(getTimeNow(),s->last_heartbeat)>10){
        if(!s->missed_heartbeat){
          s->missed_heartbeat = true;
        }
      }
      }
      
      sleep(3);
    }
}


std::time_t getTimeNow(){
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

