#include <glog/logging.h>
#include <google/protobuf/duration.pb.h>
#include <google/protobuf/timestamp.pb.h>
#include <google/protobuf/util/time_util.h>
#include <grpc++/grpc++.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
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

#include "CSVUtils.h"
#include "TimelinePostsUtils.h"
#include "coordinator.grpc.pb.h"
#include "sns.grpc.pb.h"
#include "synchronizer.grpc.pb.h"

namespace fs = std::filesystem;

using csce438::Confirmation;
using csce438::CoordService;
using csce438::ID;
using csce438::ServerInfo;
using csce438::TimeInfo;
using csce438::UserInfo;
using google::protobuf::Duration;
using google::protobuf::Timestamp;
using grpc::ClientContext;
using grpc::ClientWriter;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
// using csce438::ServerList;
using csce438::SynchService;
// using csce438::AllUsers;
// using csce438::TLFL;
using namespace std;
int synchID = 1;
// string serverF
std::vector<std::string> get_lines_from_file(std::string);
void run_synchronizer(std::string, std::string, std::string, int);
std::vector<std::string> get_all_users_func(int);
std::vector<std::string> get_tl_or_fl(int, int, bool);
std::vector<UserData> userDataList;
std::unique_ptr<CoordService::Stub> coord_stub;
class SynchServiceImpl final : public SynchService::Service {
  Status SendFollowingList(ServerContext* context, const UserInfo* info,
                           Confirmation* c) {
    cout << "In SendFollowingList Func" << endl;
    vector<UserData> otherClusterDataList;
    convertStringToUserDataList(info->following(), otherClusterDataList);
    // printUserDataList(otherClusterDataList);
    for (auto& u : otherClusterDataList) {
      for (auto i : u.followingList) {
        string otheruser = i;
        if (containsUser(userDataList, i)) {
          updateFollowerList(userDataList, i, u.username);
        }
      }
    }
    ID clientid;
    clientid.set_id(synchID);
    string serverID = to_string(synchID);
    while (true) {
      ClientContext context;
      ServerInfo serverInfo;
      Status status = coord_stub->GetServer(&context, clientid, &serverInfo);
      if (status.ok()) {
        serverID = serverInfo.serverid();
        break;
      } else {
        LOG(INFO) << "gRPC call failed with status code: "
                  << status.error_code() << " - " << status.error_message()
                  << "\n";
        serverID = to_string(synchID);
        break;
      }
      std::cout << "Server is not up, will ping coordinator again" << endl;
      sleep(3);
    }
    string filePath =
        "c" + to_string(synchID) + "_s" + serverID + "_serverFile.csv";
    // printUserDataList(userDataList);
    writeCSV(filePath, userDataList);

    return Status::OK;
  }

  Status SendTimeline(ServerContext* context, const TimeInfo* info,
                      Confirmation* c) {
    cout << " In Send Timeline" << endl;
    cout << info->timeline() << endl;
    vector<TimelinePosts> postsList;
    string tl = info->timeline();
    convertStringToTimelinePosts(tl, postsList);
    printTimelinePostsList(postsList);
    vector<string> usernames;
    getUsernamesWithFollowingList(userDataList, info->username(), usernames);
    for (auto& user : usernames) {
      cout << "appending file of" << user << endl;
      appendTimelinePosts("client_" + user + ".txt", postsList);
    }

    return Status::OK;
  }

  // Status ResynchServer(ServerContext* context, const ServerInfo* serverinfo,
  // Confirmation* c){
  //     // std::cout<<serverinfo->type()<<"("<<serverinfo->serverid()<<") just
  //     restarted and needs to be resynched with counterpart"<<std::endl;
  //     // std::string backupServerType;

  //     // YOUR CODE HERE

  //     return Status::OK;
  // }
};

void RunServer(std::string coordIP, std::string coordPort, std::string port_no,
               int synchID) {
  // localhost = 127.0.0.1
  std::string server_address("127.0.0.1:" + port_no);
  SynchServiceImpl service;
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
  coord_stub = std::unique_ptr<CoordService::Stub>(
      CoordService::NewStub(grpc::CreateChannel(
          coordIP + ":" + coordPort, grpc::InsecureChannelCredentials())));
  std::thread t1(run_synchronizer, coordIP, coordPort, port_no, synchID);

  /*
  TODO List:
    -Implement service calls
    -Set up initial single heartbeat to coordinator
    -Set up thread to run synchronizer algorithm
  */

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  int opt = 0;
  std::string coordIP;
  std::string coordPort;
  std::string port = "3029";

  while ((opt = getopt(argc, argv, "h:k:p:i:")) != -1) {
    switch (opt) {
      case 'h':
        coordIP = optarg;
        break;
      case 'k':
        coordPort = optarg;
        break;
      case 'p':
        port = optarg;
        break;
      case 'i':
        synchID = std::stoi(optarg);
        break;
      default:
        std::cerr << "Invalid Command Line Argument\n";
    }
  }

  RunServer(coordIP, coordPort, port, synchID);
  return 0;
}

void run_synchronizer(std::string coordIP, std::string coordPort,
                      std::string port, int synchID) {
  // setup coordinator stub
  // std::cout<<"synchronizer stub"<<std::endl;
  std::string target_str = coordIP + ":" + coordPort;
  std::unique_ptr<CoordService::Stub> coord_stub_;
  vector<std::unique_ptr<SynchService::Stub> > sync_stubs;
  coord_stub_ = std::unique_ptr<CoordService::Stub>(CoordService::NewStub(
      grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials())));
  // std::cout<<"MADE STUB"<<std::endl;

  ServerInfo msg;
  Confirmation c;

  msg.set_serverid(synchID);
  msg.set_hostname("127.0.0.1");
  msg.set_port(port);
  msg.set_type("follower");

  ClientContext context;
  while (true) {
    Status status = coord_stub_->RegisterFollowerSyncer(&context, msg, &c);

    if (status.ok()) {
      // Process the response
      std::cout << "Successful registration with coordinator by Syncer: "
                << synchID << std::endl;
      break;
    } else {
      // Handle the error
      std::cout << "Couldn't connect with coordinator " << status.error_code()
                << ": " << status.error_message() << std::endl;
    }
    sleep(3);
  }
  sleep(60);

  // send init heartbeat
  cout << "hello in synch \n";
  // TODO: begin synchronization process
  string serverhost = "", serverport = "";
  while (true) {
    // change this to 30 eventually
    sleep(30);
    if (sync_stubs.empty()) {
      for (int i = 1; i <= 3; i++) {
        if (i == synchID) continue;
        ClientContext context;
        ServerInfo serverInfo;
        ID clientid;
        clientid.set_id(i);
        // To Do ADD Retry Mechanism add three or four retries
        Status status =
            coord_stub_->GetFollowerSyncer(&context, clientid, &serverInfo);
        if (status.ok()) {
          serverhost = serverInfo.hostname();
          serverport = serverInfo.port();
          cout << serverhost << " " << serverport << endl;
          sync_stubs.push_back(SynchService::NewStub(
              grpc::CreateChannel(serverhost + ":" + serverport,
                                  grpc::InsecureChannelCredentials())));
        } else {
          std::cout
              << "fetching syncer stub gRPC call failed with status code: "
              << status.error_code() << " - " << status.error_message()
              << std::endl;
        }
      }
    }
    cout << "herer after stubs sync\n";
    ID clientid;
    clientid.set_id(synchID);
    string serverID = "";  // put it to ""
    while (true) {
      ClientContext context;
      ServerInfo serverInfo;
      Status status = coord_stub_->GetServer(&context, clientid, &serverInfo);
      if (status.ok() && serverInfo.serverid()) {
        serverID = to_string(serverInfo.serverid());
        cout << "fetcing master from cluster:" << synchID << " but got"
             << serverID << endl;

        break;
      } else {
        LOG(INFO) << "master fetch failed with status code: "
                  << status.error_code() << " - " << status.error_message()
                  << "\n";
        // serverID=to_string(synchID); break;
      }
      std::cout << "Server is not up, will ping coordinator again" << endl;
      sleep(3);
    }
    string filePath =
        "c" + to_string(synchID) + "_s" + serverID + "_serverFile.csv";
    cout << filePath << endl;
    if (fs::exists(filePath)) {
      if (!userDataList.empty()) userDataList.clear();
      readCSV(filePath, userDataList);
      string result = readCSVToString(filePath);
      for (int i = 0; i < 3; i++) {
        if (i >= sync_stubs.size()) break;
        ClientContext context;
        UserInfo info;
        info.set_following(result);
        Confirmation confirm;
        Status status =
            sync_stubs[i]->SendFollowingList(&context, info, &confirm);
        if (status.ok()) {
          cout << "Successfully sent followerList \n";
        } else {
          std::cout << "followerList gRPC call failed with status code: "
                    << status.error_code() << " - " << status.error_message()
                    << std::endl;
        }
      }
    }
    cout << " after stubs sync\n";
    if (!userDataList.empty()) {
      for (auto& u : userDataList) {
        string client_file = "client_" + u.username + ".txt";
        cout << client_file << endl;
        if (!u.followerList.empty() && fs::exists(client_file)) {
          std::vector<TimelinePosts> postsList;
          readTimelinePosts(client_file, postsList);
          // printTimelinePostsList(postsList);
          string TL = processTimelinePosts(postsList);
          // cout<<TL<<endl;
          //           printTimelinePostsList(postsList);
          writeTimelinePosts(client_file, postsList);
          if (TL.empty()) break;
          for (int i = 0; i < 3; i++) {
            if (i >= sync_stubs.size()) break;
            ClientContext context;
            TimeInfo info;
            info.set_username(u.username);
            info.set_timeline(TL);
            Confirmation confirm;
            Status status =
                sync_stubs[i]->SendTimeline(&context, info, &confirm);
            if (status.ok()) {
              cout << "Successfully sent timeline \n";
            } else {
              std::cout << "Sending Timeline to other Syncers failed from "
                        << synchID << " : " << status.error_code() << " - "
                        << status.error_message() << std::endl;
            }
          }
        }
      }
    }
  }
  return;
}

