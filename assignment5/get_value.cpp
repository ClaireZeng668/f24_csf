#include "csapp.h"
#include "message.h"
#include <iostream>
#include <sstream>
#include "message_serialization.h"

int check_response (Message server_response) {
  MessageType return_type = server_response.get_message_type();
  if (return_type == MessageType::FAILED || return_type == MessageType::ERROR) {
    std::cerr << server_response.get_quoted_text();
    return 1;
  }
  return 0;
}

int main(int argc, char **argv)
{
  if ( argc != 6 ) {
    std::cerr << "Usage: ./get_value <hostname> <port> <username> <table> <key>\n";
    return 1;
  }

  std::string hostname = argv[1];
  std::string port = argv[2];
  std::string username = argv[3];
  std::string table = argv[4];
  std::string key = argv[5];

  // TODO: implement
  int fd = open_clientfd(hostname.c_str(), port.c_str());
  if (fd < 0) {
    std::cerr << "Error: Couldn't connect to server";
    return 1;
  }

  rio_t rio;
  rio_readinitb(&rio, fd);
  
  //LOGIN
  Message login_message;
  login_message.set_message_type(MessageType::LOGIN);
  std::string client_username = argv[3];
  login_message.push_arg(client_username);
  std::string client_request;
  MessageSerialization::encode(login_message, client_request);  //encode converts Message -> string

  std::cout << client_request;

  int write_success = rio_writen(fd, client_request.data(), client_request.size());
  if (write_success != client_request.size()) {
    std::cerr << "Error: rio_writen error\n";
    return 1;
  }
  char response_buf[1025];
  int bytes_read = rio_readlineb(&rio, response_buf, 1025);
  if (bytes_read < 0) {
    std::cerr << "Error: rio_readlineb error\n";
    return 1;
  }
  std::string response = response_buf;
  Message server_response;
  MessageSerialization::decode(response, server_response); //decode converts string -> Message
  std::cout << "login response " << response << std::endl;
  int not_success = check_response(server_response);
  if (not_success) {return 1;}
  

  //GET
  Message get_request;
  get_request.set_message_type(MessageType::GET);
  std::string request_table = argv[4];
  std::string request_key = argv[5];
  get_request.push_arg(request_table);
  get_request.push_arg(request_key);
  MessageSerialization::encode(get_request, client_request);

  std::cout << client_request;

  write_success = rio_writen(fd, client_request.data(), client_request.size());
  if (write_success != client_request.size()) {
    std::cerr << "Error: rio_writen error\n";
    return 1;
  }
  response.clear();
  memset(response_buf, '\0', 1025);
  bytes_read = rio_readlineb(&rio, response_buf, 1024);
  if (bytes_read < 0) {
    std::cerr << "Error: Rio_readlineb error";
    return 1;
  }
  response = response_buf;
  MessageSerialization::decode(response, server_response);
  std::cout << "get response " << response << std::endl;
  not_success = check_response(server_response);
  if (not_success) {return 1;}


  //TOP
  Message top_request;
  top_request.set_message_type(MessageType::TOP);
  MessageSerialization::encode(top_request, client_request);


  // TODO: figure our why this is giving seg fault
  write_success = rio_writen(fd, client_request.data(), client_request.size());
  std::cout << "writen reached";
  if (write_success != client_request.size()) {
    std::cerr << "Error: rio_writen error\n";
    return 1;
  }
  
  memset(response_buf, '\0', 1025);
  bytes_read = rio_readlineb(&rio, &response, 1024);
  if (bytes_read < 0) {
    std::cerr << "Error: Rio_readlineb error";
    return 1;
  }
  response.clear();
  response = response_buf;
  MessageSerialization::decode(response, server_response);
  not_success = check_response(server_response);
  if (not_success) {return 1;}
  std::cout << server_response.get_value();// << "\n";
  return 0;
}
