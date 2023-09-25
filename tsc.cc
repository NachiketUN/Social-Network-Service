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
using csce438::SNSService;
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
  
  IReply Login();
  IReply List();
  IReply Follow(const std::string &username);
  IReply UnFollow(const std::string &username);
  void   Timeline(const std::string &username);
};


///////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////
int Client::connectTo()
{
  // ------------------------------------------------------------
  // In this function, you are supposed to create a stub so that
  // you call service methods in the processCommand/porcessTimeline
  // functions. That is, the stub should be accessible when you want
  // to call any service methods in those functions.
  // Please refer to gRpc tutorial how to create a stub.
  // ------------------------------------------------------------
    
///////////////////////////////////////////////////////////
// YOUR CODE HERE
//////////////////////////////////////////////////////////
    stub_ = SNSService::NewStub(grpc::CreateChannel(hostname+":"+port,grpc::InsecureChannelCredentials()));

    IReply loginreply = Login();

    if(!loginreply.comm_status == IStatus::SUCCESS){
      return -1;
    }

    return 1;
}

IReply Client::processCommand(std::string& input)
{
  // ------------------------------------------------------------
  // GUIDE 1:
  // In this function, you are supposed to parse the given input
  // command and create your own message so that you call an 
  // appropriate service method. The input command will be one
  // of the followings:
  //
  // FOLLOW <username>
  // UNFOLLOW <username>
  // LIST
  // TIMELINE
  // ------------------------------------------------------------
  
  // ------------------------------------------------------------
  // GUIDE 2:
  // Then, you should create a variable of IReply structure
  // provided by the client.h and initialize it according to
  // the result. Finally you can finish this function by returning
  // the IReply.
  // ------------------------------------------------------------
  
  
  // ------------------------------------------------------------
  // HINT: How to set the IReply?
  // Suppose you have "FOLLOW" service method for FOLLOW command,
  // IReply can be set as follow:
  // 
  //     // some codes for creating/initializing parameters for
  //     // service method
  //     IReply ire;
  //     grpc::Status status = stub_->FOLLOW(&context, /* some parameters */);
  //     ire.grpc_status = status;
  //     if (status.ok()) {
  //         ire.comm_status = SUCCESS;
  //     } else {
  //         ire.comm_status = FAILURE_NOT_EXISTS;
  //     }
  //      
  //      return ire;
  // 
  // IMPORTANT: 
  // For the command "LIST", you should set both "all_users" and 
  // "following_users" member variable of IReply.
  // ------------------------------------------------------------


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
    Status list_status = stub_->List(&context, list_request, &list_reply);
    ire.grpc_status = list_status;
    std::vector<std::string> all_users_list(list_reply.all_users().begin(), list_reply.all_users().end());
    ire.all_users = all_users_list;

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
    Status follow_status = stub_->Follow(&context, follow_request, &follow_reply);
    ire.grpc_status = follow_status;
    if(follow_status.ok()){
      if(follow_reply.msg() == "Invalid Username2"){
        ire.comm_status = IStatus::FAILURE_INVALID_USERNAME;
      } else if(follow_reply.msg() == "Already Exists"){
        ire.comm_status = IStatus::FAILURE_ALREADY_EXISTS;
        
      }
      else{
        ire.comm_status = IStatus::SUCCESS;
      }
    }
    else{
      ire.comm_status = IStatus::FAILURE_UNKNOWN;
    }
      
    /***
    YOUR CODE HERE
    ***/

    return ire;
}

// UNFollow Command  
IReply Client::UnFollow(const std::string& username2) {

    IReply ire;

    /***
    YOUR CODE HERE
    ***/
    ClientContext context;
    Request unfollow_request;
    unfollow_request.set_username(username);
    unfollow_request.add_arguments(username2);
    Reply unfollow_reply;
    Status status = stub_->UnFollow(&context, unfollow_request, &unfollow_reply);
    ire.grpc_status = status;

    if(unfollow_reply.msg() == "Invalid Username2"){
      ire.comm_status = IStatus::FAILURE_INVALID_USERNAME;
    }
    else if(unfollow_reply.msg() == "Not a follower"){
      ire.comm_status = IStatus::FAILURE_NOT_A_FOLLOWER;
    }
    else{
      ire.comm_status = IStatus::SUCCESS;
    }

    return ire;
}

// Login Command  
IReply Client::Login() {

    IReply ire;
  
    /***
     YOUR CODE HERE
    ***/
    ClientContext context;
    Request login_request;
    login_request.set_username(this->username);
    Reply login_reply;
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

    // ------------------------------------------------------------
    // In this function, you are supposed to get into timeline mode.
    // You may need to call a service method to communicate with
    // the server. Use getPostMessage/displayPostMessage functions 
    // in client.cc file for both getting and displaying messages 
    // in timeline mode.
    // ------------------------------------------------------------

    // ------------------------------------------------------------
    // IMPORTANT NOTICE:
    //
    // Once a user enter to timeline mode , there is no way
    // to command mode. You don't have to worry about this situation,
    // and you can terminate the client program by pressing
    // CTRL-C (SIGINT)
    // ------------------------------------------------------------
  
    ClientContext context;
    shared_ptr<ClientReaderWriter<Message, Message> > stream(stub_->Timeline(&context));
    Message init_msg;
    init_msg.set_username(username);
    stream->Write(init_msg);
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
              cout<<"out4writer\n";

    writer.join();
              cout<<"out5writer\n";

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
  while ((opt = getopt(argc, argv, "h:u:p:")) != -1){
    switch(opt) {
    case 'h':
      hostname = optarg;break;
    case 'u':
      username = optarg;break;
    case 'p':
      port = optarg;break;
    default:
      std::cout << "Invalid Command Line Argument\n";
    }
  }
      
  std::cout << "Logging Initialized. Client starting...";
  
  Client myc(hostname, username, port);
  
  myc.run();
  
  return 0;
}
