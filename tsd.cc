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

#include <ctime>

#include <google/protobuf/timestamp.pb.h>
#include <google/protobuf/duration.pb.h>

#include <thread>
#include <filesystem>

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <google/protobuf/util/time_util.h>
#include <grpc++/grpc++.h>
#include <glog/logging.h>
#define log(severity, msg) \
  LOG(severity) << msg;    \
  google::FlushLogFiles(google::severity);

#include "sns.grpc.pb.h"

using csce438::ListReply;
using csce438::Message;
using csce438::Reply;
using csce438::Request;
using csce438::SNSService;
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

struct Client
{
  std::string username;
  bool connected = true;
  int following_file_size = 0;
  std::vector<Client *> client_followers;
  std::vector<Client *> client_following;
  ServerReaderWriter<Message, Message> *stream = 0;
  bool operator==(const Client &c1) const
  {
    return (username == c1.username);
  }
};

struct Post
{
  char type;
  std::string date;
  std::string username;
  std::string content;
};

// Vector that stores every client that has been created
std::vector<Client *> client_db;

class SNSServiceImpl final : public SNSService::Service
{

  Status List(ServerContext *context, const Request *request, ListReply *list_reply) override
  {
    std::string username = request->username();
    Client *client = nullptr;
    for (Client *iter : client_db)
    {
      list_reply->add_all_users(iter->username);
      if (iter->username == username)
      {
        client = iter;
      }
    }
    for (Client *iter : client->client_followers)
    {
      if(iter->username == request->username()) continue; 
      list_reply->add_followers(iter->username);
    }
    return Status::OK;
  }

  Status Follow(ServerContext *context, const Request *request, Reply *reply) override
  {
    std::string username1 = request->username();
    const google::protobuf::RepeatedPtrField<std::string> &argument_list = request->arguments();
    std::string username2 = argument_list[0];

    if(username1 == username2){
      reply->set_msg("Already Exists");
      return Status::OK;
    }
    Client *user1 = nullptr, *user2 = nullptr;
    for (Client *client : client_db)
    {
      if (user1 && user2)
      {
        break;
      }
      if (client->username == username1)
      {
        user1 = client;
      }
      else if (client->username == username2)
      {
        user2 = client;
      }
    }

    if (!user2)
    {
      reply->set_msg("Invalid Username2");
      return Status::OK;
    }
    else
    {
      if (std::find(user1->client_following.begin(), user1->client_following.end(), user2) != user1->client_following.end())
      {
        reply->set_msg("Already Exists");
      }
      else
      {
        user1->client_following.push_back(user2);
        user2->client_followers.push_back(user1);
      }
      return Status::OK;
    }
  }

  Status UnFollow(ServerContext *context, const Request *request, Reply *reply) override
  {

    std::string username1 = request->username();
    std::string username2 = request->arguments()[0];
    if(username1 == username2){
      reply->set_msg("Invalid Username2");
      return Status::OK;
    }
    Client *user1 = nullptr, *user2 = nullptr;
    for (Client *client : client_db)
    {
      if (user1 && user2)
      {
        break;
      }
      if (client->username == username1)
      {
        user1 = client;
      }
      else if (client->username == username2)
      {
        user2 = client;
      }
    }

    if (!user2)
    {
      reply->set_msg("Invalid Username2");
      return Status::OK;
    }
    else
    {
      vector<Client *>::iterator it1 = user1->client_following.begin();
      vector<Client *>::iterator it2 = user2->client_followers.begin();
      while (it1 != user1->client_following.end())
      {
        if (*it1 == user2)
          break;
        it1++;
      }
      if(it1 == user1->client_following.end()){
        reply->set_msg("Not a follower");
        return Status::OK;
      }
      user1->client_following.erase(it1);

      while (it2 != user2->client_followers.end())
      {
        if (*it2 == user1)
          break;
        it2++;
      }
      user2->client_followers.erase(it2);
      return Status::OK;
    }


    return Status::OK;
  }

  // RPC Login
  Status Login(ServerContext *context, const Request *request, Reply *reply) override
  {

    bool is_new_user = true;
    for (const Client *client : client_db)
    {
      if (client->username == request->username())
      {
        is_new_user = false;
        break;
      }
    }
    if (is_new_user)
    {
      Client *new_login_client = new Client;
      new_login_client->username = request->username();
      new_login_client->client_followers.push_back(new_login_client);
      if (std::filesystem::exists(request->username() + ".txt"))
      {
        new_login_client->following_file_size = 1;
      }
      client_db.push_back(new_login_client);
      reply->set_msg("Login Success");
    }
    else
    {
      reply->set_msg("Login Fail: Duplicate User");
    }

    return Status::OK;
  }

  Status Timeline(ServerContext *context,
                  ServerReaderWriter<Message, Message> *stream) override
  {

    Message client_message;
    while (stream->Read(&client_message))
    {
      if (!client_message.has_timestamp())
      {
        string username = client_message.username();
        Client *client = *find_if(client_db.begin(), client_db.end(), [&username](Client *cl)
                                  { return cl->username == username; });
        client->stream = stream;
        if (client->following_file_size)
        {
          std::ifstream file(client->username + ".txt");
          cout << "reading file";
          if (!file.is_open())
          {
            std::cout << "Error opening file." << std::endl;
          }
          else
          {
            int n = 20;
            std::vector<Post> posts;
            std::string line;
            while (n > 0 && std::getline(file, line))
            {
              Post post;
              int line_no = 0;
              while (line_no < 3)
              {
                if (line.empty())
                  break;
                post.type = line[0];
                switch (post.type)
                {
                case 'T':
                  post.date = line.substr(2);
                  break;
                case 'U':
                  post.username = line.substr(2);
                  break;
                case 'W':
                  post.content = line.substr(2);
                  break;
                default:
                  // Invalid post type, skip this post
                  break;
                }
                line_no++;
                std::getline(file, line);
              }
              if (line_no == 3)
                posts.push_back(post);
              n--;
            }
            cout<<posts.size()<<endl;
            for (auto post : posts)
            {
              client_message.set_msg(post.content);
              google::protobuf::Timestamp *timestamp = new google::protobuf::Timestamp();
              struct tm tm_time;
              if (strptime(post.date.c_str(), "%Y-%m-%d %H:%M:%S", &tm_time) == nullptr)
              {
                cout << post.date << endl;
                std::cout << "Error parsing datetime string." << std::endl;
              }

              int64_t seconds = mktime(&tm_time);

              timestamp->set_seconds(seconds);
              timestamp->set_nanos(0);
              client_message.set_allocated_timestamp(timestamp);
              stream->Write(client_message);
            }
          }
          file.close();
        }
      }
      else
      {
        string username = client_message.username();
        char buffer[20];
        time_t seconds = client_message.timestamp().seconds();
        struct tm timeinfo;
        gmtime_r(&seconds, &timeinfo);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
        string post_time(buffer);
        std::string postContent = "T " + post_time + "\nU " + client_message.username() + "\nW " + client_message.msg() + "\n";

        Client *client = *find_if(client_db.begin(), client_db.end(), [&username](Client *cl)
                                  { return cl->username == username; });
        client->stream = stream;
        client->following_file_size = 1;
        auto follower_list = client->client_followers;

        thread writer([&follower_list, &postContent, &client_message]()
                      {
            for(Client* follower: follower_list){
              if(follower->username == client_message.username()) continue;
              follower->following_file_size=1;
              const std::string fileName = follower->username+ ".txt";
              if(follower->stream != 0){
                follower->stream->Write(client_message);
              }
              std::ofstream outFile(fileName, std::ios::app);
              if (!outFile.is_open()) {
                  std::cout << "Failed to open the file for writing." << std::endl;
              }
              outFile << postContent;
              outFile.close();
            } });
        writer.join();
      }
    }
    return Status::OK;
  }
};

void RunServer(std::string port_no)
{
  std::string server_address = "0.0.0.0:" + port_no;
  SNSServiceImpl service;

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  log(INFO, "Server listening on " + server_address);

  server->Wait();
}

int main(int argc, char **argv)
{

  std::string port = "3010";

  int opt = 0;
  while ((opt = getopt(argc, argv, "p:")) != -1)
  {
    switch (opt)
    {
    case 'p':
      port = optarg;
      break;
    default:
      std::cerr << "Invalid Command Line Argument\n";
    }
  }

  std::string log_file_name = std::string("server-") + port;
  google::InitGoogleLogging(log_file_name.c_str());
  log(INFO, "Logging Initialized. Server starting...");
  RunServer(port);

  return 0;
}
