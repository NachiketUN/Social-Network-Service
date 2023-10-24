#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <string>
#include <unistd.h>
#include <csignal>
#include <sstream>
#include <thread>


#include <grpc++/grpc++.h>
#include <google/protobuf/util/time_util.h>

#include "client.h"

#include "coordinator.grpc.pb.h"
#include "sns.grpc.pb.h"
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using csce438::Message;
using csce438::ListReply;
using csce438::Request;
using csce438::Reply;
using csce438::ServerInfo;
using csce438::Confirmation;
using csce438::Request;
using csce438::ID;
using csce438::SNSService;
using csce438::CoordService;
using namespace std;

void sig_ignore(int sig) {
  std::cout << "\nSignal caught "<< sig<<endl;
  exit(sig);
}

Message MakeMessage(const std::string& username, const std::string& msg) {
    Message m;
    m.set_username(username);
    m.set_msg(msg);
    google::protobuf::Timestamp* timestamp = new google::protobuf::Timestamp();
    timestamp->set_seconds(time(NULL));
    timestamp->set_nanos(0);
    m.set_allocated_timestamp(timestamp);
    return m;
}


class Client : public IClient
{
public:
  Client(const std::string& hname,
	 const std::string& uname,
	 const std::string& p)
    :hostname(hname), username(uname), port(p) {}

  
protected:
  virtual int connectTo();
  virtual IReply processCommand(std::string& input);
  virtual void processTimeline();

private:
  std::string hostname;
  std::string username;
  std::string port;
  
  // You can have an instance of the client stub
  // as a member variable.
  std::unique_ptr<SNSService::Stub> stub_;
  std::unique_ptr<CoordService::Stub> coordstub_;

  IReply Login();
  IReply List();
  IReply Follow(const std::string &username);
  IReply UnFollow(const std::string &username);
  void   Timeline(const std::string &username);
};


int Client::connectTo()
{
    coordstub_ = CoordService::NewStub(grpc::CreateChannel(hostname+":"+port,grpc::InsecureChannelCredentials()));
    ID clientid;
    clientid.set_id(std::stoi(username));
    string serverhost = "", serverport ="";
    while(true){
      ClientContext context;
      ServerInfo serverInfo;
      Status status = coordstub_->GetServer(&context, clientid, &serverInfo);
      if(status.ok()){
        serverhost = serverInfo.hostname();
        serverport = serverInfo.port();
        break;
      }
      else{
        std::cout << "gRPC call failed with status code: " << status.error_code()
                  << " - " << status.error_message() << std::endl;
      }
      std::cout<<"Server is not up, will ping coordinator again"<<endl;
      sleep(5);
    }
    
    stub_ = SNSService::NewStub(grpc::CreateChannel(serverhost+":"+serverport,grpc::InsecureChannelCredentials()));
    IReply loginreply = Login();

    if(!loginreply.comm_status == IStatus::SUCCESS){
      return -1;
    }

    return 1;
}

IReply Client::processCommand(std::string& input)
{

    std::istringstream iss(input);

    // Read the first part (FOLLOW)
    std::string command;
    iss >> command;
    if(command == "FOLLOW"){
      std::string username2;
      iss>>username2;
      return Follow(username2);
      
    } else if (command == "UNFOLLOW"){
      std::string username2;
      iss>>username2;
      return UnFollow(username2);
    } else if (command == "LIST"){
      return List();
    } else if (command == "TIMELINE"){
      IReply ire;
      ire.grpc_status = grpc::Status::OK;
      ire.comm_status = IStatus::SUCCESS;
      processTimeline();
      return ire;
    }
    IReply ire;
    std::cout<<"Invalid Command\n";
    ire.comm_status = IStatus::FAILURE_INVALID;
    return ire;
    
}


void Client::processTimeline()
{
    Timeline(username);
}

// List Command
IReply Client::List() {

    IReply ire;
    ClientContext context;
    Request list_request;
    ListReply list_reply;

    list_request.set_username(username);
    // Make RPC call LIST()
    Status list_status = stub_->List(&context, list_request, &list_reply);
    ire.grpc_status = list_status;

    // Create vector of strings with all usernames
    std::vector<std::string> all_users_list(list_reply.all_users().begin(), list_reply.all_users().end());
    ire.all_users = all_users_list;

    // Create vector of strings with followers usernames
    std::vector<std::string> followers_list(list_reply.followers().begin(), list_reply.followers().end());
    ire.followers = followers_list;

    ire.comm_status = IStatus::SUCCESS;
    return ire;
}

// Follow Command        
IReply Client::Follow(const std::string& username2) {

    IReply ire; 
    ClientContext context;
    Request follow_request;
    follow_request.set_username(username);
    follow_request.add_arguments(username2);
    Reply follow_reply;
    // Make RPC call FOLLOW()
    Status follow_status = stub_->Follow(&context, follow_request, &follow_reply);
    ire.grpc_status = follow_status;

    if(follow_status.ok()){
      //If second user name ie username2 is invalid set IStatus
      if(follow_reply.msg() == "Invalid Username2"){
        ire.comm_status = IStatus::FAILURE_INVALID_USERNAME;
      } else if(follow_reply.msg() == "Already Exists"){
        //If user2 is already follower of user1
        ire.comm_status = IStatus::FAILURE_ALREADY_EXISTS;
        
      }
      else{
        ire.comm_status = IStatus::SUCCESS;
      }
    }
    else{
      ire.comm_status = IStatus::FAILURE_UNKNOWN;
    }
      
    return ire;
}

// UNFollow Command  
IReply Client::UnFollow(const std::string& username2) {

    IReply ire;

    ClientContext context;
    Request unfollow_request;
    unfollow_request.set_username(username);
    unfollow_request.add_arguments(username2);
    Reply unfollow_reply;

    // Make RPC call UnFollow() and send username1 and username2
    Status status = stub_->UnFollow(&context, unfollow_request, &unfollow_reply);
    ire.grpc_status = status;

    if(unfollow_reply.msg() == "Invalid Username2"){
      ire.comm_status = IStatus::FAILURE_INVALID_USERNAME;  // if user2 is invalid
    } 
    else if(unfollow_reply.msg() == "Not a follower"){
      ire.comm_status = IStatus::FAILURE_NOT_A_FOLLOWER; // if user2 is not a follower of user1
    }
    else{
      ire.comm_status = IStatus::SUCCESS;
    }

    return ire;
}

// Login Command  
IReply Client::Login() {

    IReply ire;
    ClientContext context;
    Request login_request;
    login_request.set_username(this->username);
    Reply login_reply;

    //Make RPC Login() to register new user
    Status login_status = stub_->Login(&context, login_request, &login_reply);
    ire.grpc_status = login_status;

    if(login_status.ok()){
      if(login_reply.msg() == "Login Success"){
        ire.comm_status = IStatus::SUCCESS;
      }
      else
        ire.comm_status = IStatus::FAILURE_INVALID_USERNAME;

    }
    else{
      ire.comm_status = IStatus::FAILURE_UNKNOWN;
    }

    return ire;
}

// Timeline Command
void Client::Timeline(const std::string& username) {
  
    ClientContext context;
    // Create a stream for bidirectional RPC
    shared_ptr<ClientReaderWriter<Message, Message> > stream(stub_->Timeline(&context));
    Message init_msg;
    // Send dummy message to help the server store the stream on its end
    init_msg.set_username(username);
    stream->Write(init_msg);

    // Create a child thread to delegate writing and sending of messages to server
    thread writer([stream, &username](){
      while(1){
        string getPost = getPostMessage();
        if(!getPost.empty()){
          Message post = MakeMessage(username,getPost);
          stream->Write(post);
         }   
      }
    });
    Message server_message;

    // In parent thread, we continue with the task of reading messages from server if any
    while(1){
      stream->Read(&server_message);
      google::protobuf::Timestamp timestamp = server_message.timestamp();
      std::chrono::seconds seconds(timestamp.seconds());
      std::chrono::nanoseconds nanos(timestamp.nanos());

      // Calculate the total duration in nanoseconds
      std::chrono::nanoseconds totalDuration = seconds + nanos;

      // Convert the duration to std::time_t
      time_t posttime = std::chrono::duration_cast<std::chrono::seconds>(totalDuration).count();
      displayPostMessage(server_message.username(),server_message.msg(), posttime);
    }

    // Wait for child thread and continue
    writer.join();

    Status status = stream->Finish();
    if (!status.ok()) {
      std::cout << "Timeline rpc failed." << std::endl;
    }

}



//////////////////////////////////////////////
// Main Function
/////////////////////////////////////////////
int main(int argc, char** argv) {
  signal(SIGINT, sig_ignore);


  std::string hostname = "localhost";
  std::string username = "default";
  std::string port = "3010";
    
  int opt = 0;
  while ((opt = getopt(argc, argv, "h:k:u:")) != -1){
    switch(opt) {
    case 'h':
      hostname = optarg;break;
    case 'k':
    port = optarg;break;
      
    case 'u':
      username = optarg;break;
    default:
      std::cout << "Invalid Command Line Argument\n";
    }
  }
      
  std::cout << "Logging Initialized. Client starting...";
  
  Client myc(hostname, username, port);
  
  myc.run();
  
  return 0;
}
