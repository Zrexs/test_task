#include <iostream>
#include <memory>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <vector>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#ifdef BAZEL_BUILD
#include "myproto.grpc.pb.h"
#else
#include "myproto.grpc.pb.h"
#endif

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using myprogram::Request;
using myprogram::Reply;
using myprogram::Greeter;

class server_logic 
{
    const int zero = 0;
    int size;
    int count_one;
    int sum;
    bool valid;
    int tmp;
    std::string s;
  public:
    int max;
    int foo(std::string s);
};

class GreeterServiceImpl final : public Greeter::Service, server_logic {
    
  Status Say(ServerContext* context, const Request* request,
                  Reply* reply){

  std::string s(request->name());
  foo(s);
  reply->set_message(std::to_string(max)); 
  return Status::OK;
  }

};

int server_logic::foo(std::string s) {

    count_one = zero;
    sum = zero;
    valid = true;
    tmp = zero;
    max = zero;
    
    std::stringstream ss(s);
    std::vector<int>v;
    std::copy(std::istream_iterator<int>(ss), {}, back_inserter(v));
    size = v.size();
    v.shrink_to_fit();
    for(int i = 0; i < size; i++) {
        if(v[i] != zero && i < size) {
            count_one++;
        }
        else
        {
            if(valid == true)
            {
                sum = count_one + tmp;
                tmp = count_one;
                if(max < sum)
                {
                    max = sum;
                }
                count_one = zero;
                valid = false;
            }
            else
            {
                sum = count_one + tmp;
                tmp = count_one;
                if(max < sum)
                {
                    max = sum;
                }
                count_one = zero;
                valid = true;
            }
        }
    }
    sum = tmp + count_one;
    if(max < sum) {
        max = sum;
    }
    
    return 0;
}

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  GreeterServiceImpl service;
  
  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();

  return 0;
}
