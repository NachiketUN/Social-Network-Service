/*
 *
 * Copyright 2015, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <glog/logging.h>
#include <google/protobuf/duration.pb.h>
#include <google/protobuf/timestamp.pb.h>
#include <google/protobuf/util/time_util.h>
#include <grpc++/grpc++.h>
#include <stdlib.h>
#include <unistd.h>

#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <thread>
#define log(severity, msg) \
  LOG(severity) << msg;    \
  google::FlushLogFiles(google::severity);

#include "CSVUtils.h"
#include "TimelinePostsUtils.h"
#include "coordinator.grpc.pb.h"
#include "sns.grpc.pb.h"
using csce438::Confirmation;
using csce438::CoordService;
using csce438::ListReply;
using csce438::Message;
using csce438::Reply;
using csce438::Request;
using csce438::ServerInfo;
using csce438::SNSService;
using grpc::ClientContext;

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
namespace fs = std::filesystem;

void sendHeartbeat();
struct Coordinator {
  int clusterId;
  std::string hostname;
  std::string port;
  int serverID;
  std::string serverport;
};

Coordinator CoordinatorDetails;
struct Client {
  std::string username;
  bool connected = true;
  int following_file_size = 0;
  std::vector<Client *> client_followers;
  std::vector<Client *> client_following;
  ServerReaderWriter<Message, Message> *stream = 0;
  bool operator==(const Client &c1) const { return (username == c1.username); }
};

// Structure to read "posts" from the files corresponding to each client
struct Post {
  char type;
  std::string date;
  std::string username;
  std::string content;
};

// Vector that stores every client that has been created
std::vector<Client *> client_db;
std::unique_ptr<CoordService::Stub> stub_;
std::string serverFile = "";
std::vector<UserData> userDataList;
class SNSServiceImpl final : public SNSService::Service {
  Status List(ServerContext *context, const Request *request,
              ListReply *list_reply) override {
    std::string username = request->username();
    Client *client = nullptr;

  // Find all the clients in client_db and add them to user list
  // for (Client *iter : client_db) {
  //   list_reply->add_all_users(iter->username);
  //   if (iter->username == username) {
  //     client = iter;
  //   }
  // }
  // Find all the followers of current client and add them to followers list
  // for (Client *iter : client->client_followers) {
  //   if (iter->username == request->username()) continue;
  //   list_reply->add_followers(iter->username);
  // }
  std:
    set<string> followers;
    userDataList.clear();
    readCSV(serverFile, userDataList);
    for (const auto &user : userDataList) {
      if (user.username == username) {
        followers = user.followerList;
        break;
      }
    }
    // std::sort(followers.begin(), followers.end());
    for (auto &follower : followers) {
      list_reply->add_followers(follower);
    }
    vector<string> clientNames;
    findAllUsers("./", clientNames);
    std::sort(clientNames.begin(), clientNames.end());
    for (auto &name : clientNames) list_reply->add_all_users(name);
    return Status::OK;
  }

  Status Follow(ServerContext *context, const Request *request,
                Reply *reply) override {
    std::string username1 = request->username();
    const google::protobuf::RepeatedPtrField<std::string> &argument_list =
        request->arguments();
    std::string username2 = argument_list[0];

    // If both usernames match, it means the client is trying to follow itself
    if (username1 == username2) {
      reply->set_msg("Already Exists");
      return Status::OK;
    }
    Client *user1 = nullptr, *user2 = nullptr;

    // Find user1 and user2 from client_db based on username
    for (Client *client : client_db) {
      if (user1 && user2) {
        break;
      }
      if (client->username == username1) {
        user1 = client;
      } else if (client->username == username2) {
        user2 = client;
      }
    }
    updateFollowingList(userDataList, username1, username2);
    writeCSV(serverFile, userDataList);

    if (!user2) {
      // store the invalid username in file
      // reply->set_msg("Invalid Username2"); // If user2 was not found in
      // client_db, it is invalid
      return Status::OK;
    } else  // Both user1 and user2 are found from client_db
    {
      if (std::find(user1->client_following.begin(),
                    user1->client_following.end(),
                    user2) != user1->client_following.end()) {
        reply->set_msg("Already Exists");  // User1 already follows user2
      } else {
        // Add user2 in following list of user1
        user1->client_following.push_back(user2);
        user2->client_followers.push_back(user1);
      }
      return Status::OK;
    }
  }

  Status UnFollow(ServerContext *context, const Request *request,
                  Reply *reply) override {
    std::string username1 = request->username();
    std::string username2 = request->arguments()[0];
    // Client can't unfollow itself, so if username1 matches username2 it is
    // considered invalid
    if (username1 == username2) {
      reply->set_msg("Invalid Username2");
      return Status::OK;
    }
    Client *user1 = nullptr, *user2 = nullptr;

    // Find user1 and user2 from client_db
    for (Client *client : client_db) {
      if (user1 && user2) {
        break;
      }
      if (client->username == username1) {
        user1 = client;
      } else if (client->username == username2) {
        user2 = client;
      }
    }

    if (!user2) {
      reply->set_msg("Invalid Username2");  // User2 not found in client_db
      return Status::OK;
    } else {
      vector<Client *>::iterator it1 = user1->client_following.begin();
      vector<Client *>::iterator it2 = user2->client_followers.begin();

      // Find position of user2 in user1's following list
      while (it1 != user1->client_following.end()) {
        if (*it1 == user2) break;
        it1++;
      }
      if (it1 == user1->client_following.end()) {
        reply->set_msg("Not a follower");  // If the user2 is not found in
                                           // following list of user1
        return Status::OK;
      }
      user1->client_following.erase(
          it1);  // Remove user2 from following list of user1

      // Find position of user1 in user2's follower list
      while (it2 != user2->client_followers.end()) {
        if (*it2 == user1) break;
        it2++;
      }
      user2->client_followers.erase(
          it2);  // Remove user1 from follower list of user2
      return Status::OK;
    }

    return Status::OK;
  }

  // RPC Login
  Status Login(ServerContext *context, const Request *request,
               Reply *reply) override {
    bool is_new_user = true;

    // Check if username is alreay registered
    for (const Client *client : client_db) {
      if (client->username == request->username()) {
        is_new_user = false;
        break;
      }
    }
    if (is_new_user)  // Register new User
    {
      Client *new_login_client = new Client;
      new_login_client->username = request->username();
      new_login_client->client_followers.push_back(new_login_client);
      if (std::filesystem::exists(
              "client_" + request->username() +
              ".txt")) {  // If user file exists, set its corresponding size to
                          // non zero, used to detect older messages from users
                          // file
        new_login_client->following_file_size = 1;
      }
      client_db.push_back(new_login_client);
      // const std::string fileName = "client_" + follower->username + ".txt";
      //                         cout<<fileName<<endl;

      // std::ofstream outFile(serverFile, std::ios::app);
      //         if (!outFile.is_open()) {
      //             std::cout << "Failed to open the file for writing." <<
      //             std::endl;
      //         }
      //         outFile << new_login_client->username<<"; ; ;\n";
      //         outFile.close();
      userDataList.clear();
      readCSV(serverFile, userDataList);
      addUser(userDataList, new_login_client->username);
      writeCSV(serverFile, userDataList);
      reply->set_msg("Login Success");
    } else {
      reply->set_msg("Login Success");
    }

    return Status::OK;
  }

  Status Timeline(ServerContext *context,
                  ServerReaderWriter<Message, Message> *stream) override {
    Message client_message;
    // Loop to read messages from client
    while (stream->Read(&client_message)) {
      if (!client_message.has_timestamp()) {  // Dummy first message detected,
                                              // capture the client stream and
                                              // store it in struct Client
        string username = client_message.username();
        Client *client = *find_if(
            client_db.begin(), client_db.end(),
            [&username](Client *cl) { return cl->username == username; });
        client->stream = stream;
        if (client->following_file_size)  // User file already exists, send 20
                                          // latest posts from file
        {
          std::string filePath = "client_" + client->username + ".txt";
          vector<TimelinePosts> clientPosts;
          if (std::filesystem::exists(filePath)) {
            readTimelinePosts(filePath, clientPosts);
            bool writeNeeded = false;
            for (auto &post : clientPosts) {
              if (post.clientProcessing == false) {
                // std::tm tm;
                // std::istringstream iss(post.timestamp);
                // iss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
                // std::time_t timeT = std::mktime(&tm);
                // displayPostMessage(post.username,post.content, timeT);
                post.clientProcessing = true;
                writeNeeded = true;
              }
            }
            if (writeNeeded) writeTimelinePosts(filePath, clientPosts);
          }
          // Send the latest 20 posts to client
          int n = 20;
          for (auto post = clientPosts.begin(); post != clientPosts.end(); ++post) {
            client_message.set_msg(post->content);
            std::tm tm;
            std::istringstream iss(post->timestamp);
            iss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
            google::protobuf::Timestamp *timestamp =
                new google::protobuf::Timestamp();


            int64_t seconds = mktime(&tm);

            timestamp->set_seconds(seconds);
            timestamp->set_nanos(0);
            client_message.set_allocated_timestamp(timestamp);
            client_message.set_username(post->username);
            stream->Write(client_message);
            n--;
            if (n == 0) break;
          }
        }
      } else  // Incase when the client sends a genuine message(not a dummy one)
      {
        string username = client_message.username();
        char buffer[20];
        time_t seconds = client_message.timestamp().seconds();
        struct tm timeinfo;
        gmtime_r(&seconds, &timeinfo);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
        string post_time(buffer);
        
        std::string postContent = "T " + post_time + "\nU " +
                                  client_message.username() + "\nW " +
                                  client_message.msg();
        // Find client from client_db
        Client *client = *find_if(
            client_db.begin(), client_db.end(),
            [&username](Client *cl) { return cl->username == username; });
        client->stream = stream;
        client->following_file_size = 1;
        auto follower_list = client->client_followers;
        for (auto follower : client->client_followers) {
          cout << follower->username << " followers list\n";
        }
        // Writer thread to write messages to clients followers and
        // corresponding files
        thread writer([&follower_list, &postContent, &client_message]() {
          cout << "in new thread\n";
          // so then write those messages to follower list

          for (Client *follower : follower_list) {
            string postMessage = postContent;
            if (follower->username == client_message.username()) {
              postMessage = postMessage + "S 0\nC 1\n\n";
            } else {
              postMessage = postMessage + "S 1\nC 1\n\n";
            }
            if (follower->username != client_message.username()) {
              follower->following_file_size = 1;
              if (follower->stream != 0) {
                follower->stream->Write(client_message);
              }
            }
            const std::string fileName =
                "client_" + follower->username + ".txt";
            cout << fileName << endl;

            std::ofstream outFile(fileName, std::ios::app);
            if (!outFile.is_open()) {
              std::cout << "Failed to open the file for writing." << std::endl;
            }
            outFile << postMessage;
            outFile.close();
          }
        });
        writer.join();
      }
    }
    return Status::OK;
  }
};

void RunServer(std::string port_no) {
  readCSV(serverFile, userDataList);
  // printUserDataList(userDataList);
  std::string server_address = "0.0.0.0:" + port_no;
  SNSServiceImpl service;

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  log(INFO, "Server listening on " + server_address);

  stub_ = CoordService::NewStub(grpc::CreateChannel(
      CoordinatorDetails.hostname + ":" + CoordinatorDetails.port,
      grpc::InsecureChannelCredentials()));

  while (true) {
    ClientContext context;
    ServerInfo registerServerInfo;
    Confirmation registerConfirmation;
    registerServerInfo.set_serverid(CoordinatorDetails.serverID);
    registerServerInfo.set_hostname(CoordinatorDetails.hostname);
    registerServerInfo.set_port(CoordinatorDetails.serverport);
    registerServerInfo.set_cluster(CoordinatorDetails.clusterId);
    registerConfirmation.set_status(false);
    Status status = stub_->RegisterServer(&context, registerServerInfo,
                                          &registerConfirmation);
    if (status.ok() && registerConfirmation.status()) {
      // Process the response
      std::cout << "Registered with Coordinator Cluster No:"
                << CoordinatorDetails.clusterId
                << " Server:" << CoordinatorDetails.serverID << std::endl;
      if (registerConfirmation.is_master()) {
        cout << "Got assigned as master server \n";
      } else {
        cout << "Got assigned as backup server \n";
      }
      break;
    } else {
      // Handle the error
      std::cout << "RPC failed with error code " << status.error_code() << ": "
                << status.error_message() << std::endl;
    }
    sleep(3);
  }

  std::thread hb(sendHeartbeat);

  server->Wait();
}
void sendHeartbeat() {
  int i = 1;
  while (true) {
    ClientContext context;
    ServerInfo serverInfo;
    Confirmation heartbeatConfirmation;
    heartbeatConfirmation.set_status(false);
    serverInfo.set_serverid(CoordinatorDetails.serverID);
    serverInfo.set_hostname(CoordinatorDetails.hostname);
    serverInfo.set_port(CoordinatorDetails.serverport);
    serverInfo.set_cluster(CoordinatorDetails.clusterId);

    Status status =
        stub_->Heartbeat(&context, serverInfo, &heartbeatConfirmation);

    if (status.ok() && heartbeatConfirmation.status()) {
      log(INFO, "Successfully sent heartbeat to coordinator");
      cout << "Successfully sent heartbeat to server Coordinator no: " << i
           << endl;
    } else {
      log(INFO, "Heartbeat RPC Unsuccessfull, will try in 5 secs again");
      cout << "Not available";
    }
    i++;
    sleep(5);
  }
}

int main(int argc, char **argv) {
  std::string port = "3010";

  int opt = 0;
  while ((opt = getopt(argc, argv, "c:s:h:k:p:")) != -1) {
    switch (opt) {
      case 'c':
        CoordinatorDetails.clusterId = stoi(optarg);
        break;
      case 's':
        CoordinatorDetails.serverID = stoi(optarg);
        break;
      case 'h':
        CoordinatorDetails.hostname = optarg;
        break;
      case 'k':
        CoordinatorDetails.port = optarg;
        break;

      case 'p':
        port = optarg;
        CoordinatorDetails.serverport = port;
        break;
      default:
        std::cerr << "Invalid Command Line Argument\n";
    }
  }
  serverFile = "c" + std::to_string(CoordinatorDetails.clusterId) + "_s" +
               to_string(CoordinatorDetails.serverID) + "_serverFile.csv";
  std::string log_file_name = std::string("server-") + port;
  google::InitGoogleLogging(log_file_name.c_str());
  log(INFO, "Logging Initialized. Server starting...");
  RunServer(port);

  return 0;
}
