#include <glog/logging.h>
#include <google/protobuf/duration.pb.h>
#include <google/protobuf/timestamp.pb.h>
#include <google/protobuf/util/time_util.h>
#include <grpc++/grpc++.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#define log(severity, msg) \
  LOG(severity) << msg;    \
  google::FlushLogFiles(google::severity);

#include "coordinator.grpc.pb.h"

using csce438::Confirmation;
using csce438::CoordService;
using csce438::ID;
using csce438::ServerInfo;
using google::protobuf::Duration;
using google::protobuf::Timestamp;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using namespace std;
// using csce438::ServerList;
// using csce438::SynchService;

struct zNode {
  int serverid;
  int cluster;
  std::string hostname;
  std::string port;
  std::string type;
  std::time_t last_heartbeat;
  bool missed_heartbeat;
  bool isActive();
  bool is_master;
};

// potentially thread safe
std::mutex v_mutex;
std::mutex active_mutex;
std::vector<zNode *> cluster1;
std::vector<zNode *> cluster2;
std::vector<zNode *> cluster3;
std::vector<zNode *> serverList;
std::vector<zNode *> followerSyncer;


// func declarations
int findServer(std::vector<zNode> v, int id);
std::time_t getTimeNow();
void checkHeartbeat();

bool zNode::isActive() {
  bool status = false;
  if (!missed_heartbeat) {
    status = true;
  } else if (difftime(getTimeNow(), last_heartbeat) < 10) {
    status = true;
  }
  return status;
}

class CoordServiceImpl final : public CoordService::Service {
  Status Heartbeat(ServerContext *context, const ServerInfo *serverinfo,
                   Confirmation *confirmation) override {
    // Your code here
    int cluster_id = serverinfo->cluster();
    int serverid = serverinfo->serverid();
          // LOG(INFO)<<" HeartBeat from cluster: "<<cluster_id<<" server:"<<to_string(serverid)<<std::endl;
    zNode *node = nullptr;
    switch (cluster_id) {
      case 1:
        node = getServerFromCluster(cluster1, serverid);
        break;
      case 2:
        node = getServerFromCluster(cluster2, serverid);
        break;
      case 3:
        node = getServerFromCluster(cluster3, serverid);
        break;

      default:
        log(INFO, "Did not find server during heartbeat rpc");
        break;
    }
    if (node != nullptr) {
      node->last_heartbeat = getTimeNow();
      confirmation->set_status(true);
      // LOG(INFO)<<"Receieved HeartBeat from cluster: "<<cluster_id<<" server:"<<to_string(serverid)<<std::endl;
    } else {
      confirmation->set_status(false);
    }

    return Status::OK;
  }

  Status GetFollowerSyncer(ServerContext *context, const ID *id,
                   ServerInfo *serverinfo) override {
    int serverID = id->id();
    
    // cout << to_string(serverID);
    zNode *syncer = nullptr;
    // log(INFO, "Client fetching server details");
    syncer = getServerFromCluster(followerSyncer, serverID);
    if (syncer == nullptr) {
      // cout << "Couldn't get Syncer Number: "<<serverID<<"\n";
      return grpc::Status(grpc::StatusCode::NOT_FOUND,
                          "No server is alive in cluster");
    }


    serverinfo->set_serverid(syncer->serverid);
    serverinfo->set_hostname(syncer->hostname);
    serverinfo->set_port(syncer->port);
    serverinfo->set_type(syncer->type);
    serverinfo->set_cluster(serverID);

    // cout << "found server and sending back\n";
    return Status::OK;
  }

  // function returns the server information for requested client id
  // this function assumes there are always 3 clusters and has math
  // hardcoded to represent this.
  Status GetServer(ServerContext *context, const ID *id,
                   ServerInfo *serverinfo) override {
    int serverID = (id->id() % 3);
    serverID = (serverID ? serverID : 3);
    // cout << to_string(serverID);
    zNode *server = nullptr;
    // log(INFO, "Client fetching master server details from cluster: " + to_string(serverID) );
    switch (serverID) {
      case 1:
        server = getMasterFromCluster(cluster1);
        break;
      case 2:
        server = getMasterFromCluster(cluster2);
        break;
      case 3:
        server = getMasterFromCluster(cluster3);

        break;
      default:
        break;
    }
    if (server == nullptr) {
      // cout << "Couldn't find master from cluster"<<to_string(serverID)<<"\n";
      return grpc::Status(grpc::StatusCode::NOT_FOUND,
                          "No server is alive in cluster");
    }
    
    serverinfo->set_serverid(server->serverid);
    serverinfo->set_hostname(server->hostname);
    serverinfo->set_port(server->port);
    serverinfo->set_type(server->type);
    serverinfo->set_cluster(serverID);

    // cout << "found server and sending back\n";
    return Status::OK;
  }

  Status GetBackupServer(ServerContext *context, const ID *id,
                   ServerInfo *serverinfo) override {
    int clusterID = (id->id() % 3);
    clusterID = (clusterID ? clusterID : 3);
    // cout << to_string(serverID);
    zNode *server = nullptr;
    switch (clusterID) {
      case 1:
        server = getBackupFromCluster(cluster1);
        break;
      case 2:
        server = getBackupFromCluster(cluster2);
        break;
      case 3:
        server = getBackupFromCluster(cluster3);

        break;
      default:
        break;
    }
    if (server == nullptr) {
      // cout << "Couldn't find Backup from cluster"<<to_string(serverID)<<"\n";
      return grpc::Status(grpc::StatusCode::NOT_FOUND,
                          "No backup server is alive in cluster");
    }
    
    serverinfo->set_serverid(server->serverid);
    serverinfo->set_hostname(server->hostname);
    serverinfo->set_port(server->port);
    serverinfo->set_type(server->type);
    serverinfo->set_cluster(clusterID);

    // cout << "found server and sending back\n";
    return Status::OK;
  }

  Status RegisterServer(ServerContext *context, const ServerInfo *serverInfo,
                        Confirmation *confirmation) override {
    log(INFO, "In Register Service");
    // cout << "In Register Service\n";
    int cluster_id = serverInfo->cluster();
    zNode *new_node = new zNode;
    new_node->serverid = serverInfo->serverid();
    new_node->hostname = serverInfo->hostname();
    new_node->port = serverInfo->port();
    new_node->type = serverInfo->type();
    new_node->last_heartbeat = getTimeNow();
    new_node->is_master = false;
    new_node->cluster = cluster_id;
    bool newEntry = false;
    switch (cluster_id) {
      case 1:
        if (!isPresent(cluster1, new_node->serverid)) {
          if (cluster1.empty()) {
            new_node->is_master = true;
          }
          cluster1.push_back(new_node);
          serverList.push_back(new_node);
          newEntry = true;
        }
        break;
      case 2:
        if (!isPresent(cluster2, new_node->serverid)) {
          if (cluster2.empty()) {
            new_node->is_master = true;
          }
          cluster2.push_back(new_node);
          serverList.push_back(new_node);
          newEntry = true;
        }
        break;
      case 3:
        if (!isPresent(cluster3, new_node->serverid)) {
          if (cluster3.empty()) {
            new_node->is_master = true;
          }
          cluster3.push_back(new_node);
          serverList.push_back(new_node);
          newEntry = true;
        }
        break;

      default:
        log(INFO, "Wrong Cluster ID, skipping registeration");
        break;
    }

    cout<<"Printing Cluster1------------------------------------------------\n";
    printClusterServers(cluster1);
    cout<<"Printing Cluster2------------------------------------------------\n";
    printClusterServers(cluster2);
    cout<<"Printing Cluster3------------------------------------------------\n";
    printClusterServers(cluster3);
    cout<<"Printing ServerList------------------------------------------------\n";
    printClusterServers(serverList);
    if (newEntry) {
      log(INFO, "Server Registered Successfully Cluster:" +to_string(cluster_id)+" server:"+to_string(serverInfo->serverid()) );
    } else {
      log(INFO, "Server was already registered before");
    }
    // for (zNode *s : serverList) {
    //   cout << "ServerID: " << s->serverid << std::boolalpha
    //        << " master:" << s->is_master << " alive:" << s->isActive() << "\n";
    // }
    confirmation->set_status(true);
    confirmation->set_is_master(new_node->is_master);
    return Status::OK;
  }

  Status RegisterFollowerSyncer(ServerContext *context, const ServerInfo *serverInfo,
                        Confirmation *confirmation) override {
    log(INFO, "In RegisterFollowerSyncer");
    // cout << "In Register Service\n";
    int cluster_id = serverInfo->cluster();
    zNode *new_node = new zNode;
    new_node->serverid = serverInfo->serverid();
    new_node->hostname = serverInfo->hostname();
    new_node->port = serverInfo->port();
    new_node->type = serverInfo->type();
    new_node->last_heartbeat = getTimeNow();
    new_node->is_master = false;
    followerSyncer.push_back(new_node);
    // if(!isPresent(followerSyncer,new_node->serverid)){
      
    // }
    confirmation->set_status(true);
    return Status::OK;
  }

  zNode *getServerFromCluster(std::vector<zNode *> &cluster, int serverid) {
    for (auto &z : cluster) {
      if (z->serverid == serverid) return z;
    }
    return nullptr;
  }
  zNode *getBackupFromCluster(std::vector<zNode *> &cluster) {
    for (auto &z : cluster) {
      if (!z->is_master) {
          return z;
      }
    }

    return nullptr;
  }
  zNode *getMasterFromCluster(std::vector<zNode *> &cluster) {
    for (auto &z : cluster) {
        // cout<<"get master server id"<<to_string(z->serverid)<<" master:"<<z->is_master<<"alive:"<<z->isActive()<<"\n";
      if (z->is_master) {
        if (z->isActive())
          return z;
        else {
          z->is_master = false;
          break;
        }
      }
      // if(z.isActive() && z.is_master) return &z;
    }
    for (auto &z : cluster) {
      if (z->isActive()) {
        z->is_master = true;
        return z;
      }
    }
    return nullptr;
  }

  void printClusterServers(std::vector<zNode *> &cluster) {
    cout << "print cluster Servers in reg\n";
    for (auto &z : cluster) {
      cout << "printcluster:"<<z->cluster<<"ServerID: " << to_string(z->serverid) << std::boolalpha
           << " master:" << z->is_master << " alive:" << z->isActive() << "\n";
    }
  }
  bool isPresent(std::vector<zNode *> &cluster, int serverid) {
    for (auto &z : cluster) {
      // cout << "iSPresent: " << z->serverid << " adsa\n";
      if (z->serverid == serverid) return true;
    }
    return false;
  }
};

void RunServer(std::string port_no) {
  // start thread to check heartbeats
  std::thread hb(checkHeartbeat);
  // localhost = 127.0.0.1
  std::string server_address("127.0.0.1:" + port_no);
  CoordServiceImpl service;
  // grpc::EnableDefaultHealthCheckService(true);
  // grpc::reflection::InitProtoReflectionServerBuilderPlugin();
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

int main(int argc, char **argv) {
  std::string port = "3010";
  int opt = 0;
  while ((opt = getopt(argc, argv, "p:")) != -1) {
    switch (opt) {
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

void checkHeartbeat() {
  while (true) {
    // check servers for heartbeat > 10
    // if true turn missed heartbeat = true
    //  Your code below
    cout<<"----------\n";
    for (zNode *s : serverList) {
      cout<<"Check heartbeat Cluster:"<<s->cluster<<" ServerID: "<<s->serverid<<std::boolalpha<<"master:"<<s->is_master<<" alive:"<<s->isActive()<<"\n";

      if (difftime(getTimeNow(), s->last_heartbeat) > 10) {
        // LOG(INFO) <<s->serverid<<" has missed missed_heartbeat\n";

        if (!s->missed_heartbeat) {
          s->missed_heartbeat = true;
        }
      }
    }
    // cout<<"-----------------------------------------------------------------------------------\n";

    sleep(3);
  }
}

std::time_t getTimeNow() {
  return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}
