#include <iostream>
#include <memory>
#include <string>
#include <iterator>
#include <sstream>
#include <vector>
#include <algorithm>

#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "myproto.grpc.pb.h"
#else
#include "myproto.grpc.pb.h"
#endif

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using myprogram::Request;
using myprogram::Reply;
using myprogram::Greeter;

class data
{
    int size;
    int size_b;
    int count = 0;
    const int zero = 0;
    const int one = 1;
    int size_valid;
    std::string remainder_cin;
    char select;
    bool valid = false;
 public:
    std::string s;
    void input();
};

class GreeterClient : data {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}

  std::string Say(const std::string& user) {
    Request request;
    request.set_name(user);
    Reply reply;
    ClientContext context;
    Status status = stub_->Say(&context, request, &reply);
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }
  
 private:
  std::unique_ptr<Greeter::Stub> stub_;
};


void data::input()
{
    std::cout << "Enter size of the array" << std::endl;
    while(!(std::cin >> size) || size < zero) {
      std::cin.clear();
      while ( std::cin.get() != '\n' ) ;
      std::cout << "Incorrect data entry, try again: " << std::endl;
    }
    int a[size],i; 
    std::cout << "Enter array elements:" << std::endl;
    for (i=0; i < size; i++) 
    while(!(std::cin >> a[i]) || a[i] < zero || one < a[i]) {
      std::cin.clear();
      while ( std::cin.get() != '\n' ) ;
      std::cout << "Incorrect data entry, try again: " << std::endl;
    }
    std::cout << std::endl;
    getline(std::cin, remainder_cin);
    std::stringstream ss(remainder_cin);
    std::vector<int>v;
    std::copy(std::istream_iterator<int>(ss), {}, back_inserter(v));
    v.shrink_to_fit();
    size_valid = v.size();
    if (size_valid != zero ) {
      std::cout << "Array: " << std::endl;
      for(i=0; i < size; i++) {
        std::cout << a[i] <<' ';
      }
      std::cout << "The number of elements is greater than size of the array"
                << '\n' << "Following elements will be lost: " ;
      std::cout << remainder_cin << std::endl;
      std::cout << "Would you like to send these elements too? Y/N" <<std::endl;
      while ( valid != true ) {
        std::cin >> select;
        if (select == 'n' || select == 'N') {
          for(i=0; i < size; i++) {
          s += std::to_string(a[i]) + ' ';
          }
          valid = true;
        }
        else {
          if (select == 'y' || select == 'Y') {
            for(i=0; i < size; i++) {
            s += std::to_string(a[i]) + ' ';
            }
            s = s + ' ' + remainder_cin;
            valid = true;
          }
          else {
            valid = false;
            std::cout << "Incorrect data entry, try again: " << std::endl;
          }
        }
      }
    }
    else {
      std::cout << "Array: " << std::endl;
      for(i=0; i < size; i++) {
        std::cout << a[i] <<' ';
        s += std::to_string(a[i]) + ' ';
      }
      std::cout << std::endl;
    }
}

int main(int argc, char** argv) {
  
  std::string target_str;
  std::string arg_str("--target");
  if (argc > 1) {
    std::string arg_val = argv[1];
    size_t start_pos = arg_val.find(arg_str);
    if (start_pos != std::string::npos) {
      start_pos += arg_str.size();
      if (arg_val[start_pos] == '=') {
        target_str = arg_val.substr(start_pos + 1);
      } else {
        std::cout << "The only correct argument syntax is --target=" << std::endl;
        return 0;
      }
    } else {
      std::cout << "The only acceptable argument is --target=" << std::endl;
      return 0;
    }
  } else {
    target_str = "localhost:50051";
  }
  
  data cobj;
  cobj.input();
  
  GreeterClient greeter(grpc::CreateChannel(
      target_str, grpc::InsecureChannelCredentials()));
  std::string user(cobj.s);
  std::string reply = greeter.Say(user);
  std::cout << "Greeter received: " << reply << std::endl;
 
  return 0;
}
