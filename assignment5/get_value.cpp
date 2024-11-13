#include "csapp.h"
#include <iostream>
#include <sstream>

int check_response (std::string resp_command, std::string resp_args) {
  if (resp_command == "FAILED") {
    std::cerr << resp_args;
    return 1;
  } else if (resp_command == "ERROR") {
    std::cerr << resp_args;
    return 1;
  }
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
  char login[] = "LOGIN ";
  char* command = strcat(login, argv[3]);
  strcat(command, "\n");
  rio_writen(fd, command, (strlen(command)));
  char response[1023];
  int bytes_read = rio_readlineb(&rio, response, 1024);
  if (bytes_read < 0) {
    std::cerr << "Error: Rio_readlineb error\n";
    return 1;
  }
  std::string resp = response;
  std::stringstream ss(resp);
  std::string resp_command;
  std::string resp_args;
  ss >> resp_command;
  ss >> resp_args;
  int not_success = check_response(resp_command, resp_args);
  if (not_success) {return 1;}

  //GET
  char get[] = "GET ";
  command = strcat(get, argv[4]);
  command = strcat(command, argv[5]);
  command = strcat(command, "\n");
  rio_writen(fd, command, (5+table.length()+key.length()));
  bytes_read = rio_readlineb(&rio, response, 1024);
  if (bytes_read < 0) {
    std::cerr << "Error: Rio_readlineb error";
    return 1;
  }
  resp = response;
  ss.str("");
  ss << resp;
  ss >> resp_command;
  ss >> resp_args;
  not_success = check_response(resp_command, resp_args);
  if (not_success) {return 1;}

  //TOP
  char top[] = "TOP";
  command = strcat(get, argv[4]);
  command = strcat(command, argv[5]);
  command = strcat(command, "\n");
  rio_writen(fd, command, (3));
  bytes_read = rio_readlineb(&rio, response, 1024);
  if (bytes_read < 0) {
    std::cerr << "Error: Rio_readlineb error";
    return 1;
  }
  resp = response;
  ss.str("");
  ss << resp;
  ss >> resp_command;
  ss >> resp_args;
  not_success = check_response(resp_command, resp_args);
  if (not_success) {return 1;}
  std::cout << resp_args;
  return 0;
}
