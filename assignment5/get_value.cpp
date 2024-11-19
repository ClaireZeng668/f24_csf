//get_value.cpp
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"
#include <iostream>
#include <sstream>
#include <string>

const int RESPONSE_BUFFER_SIZE = 1024;

void handle_error(const std::string &msg) {
  std::cerr << "Error: " << msg << "\n";
  exit(1);
}

bool send_message(rio_t &rio, int fd, const Message &message) {
  std::string serialized_message;
  MessageSerialization::encode(message, serialized_message);
  if (rio_writen(fd, serialized_message.data(), serialized_message.size()) != serialized_message.size()) {
    handle_error("Failed to send message.");
    return false;
  }
  return true;
}

bool receive_message(rio_t &rio, Message &response) {
  char response_buf[RESPONSE_BUFFER_SIZE + 1] = {0};
  response.clear_args();
  int bytes_read = rio_readlineb(&rio, response_buf, RESPONSE_BUFFER_SIZE);
  if (bytes_read <= 0) {
    handle_error("Failed to read server response.");
    return false;
  }

  try {
    std::string response_str(response_buf);
    MessageSerialization::decode(response_str, response);
  } catch (const std::exception &e) {
    handle_error("Failed to decode server response: " + std::string(e.what()));
    return false;
  }
  return true;
}


int main(int argc, char **argv) {
  if (argc != 6) {
    std::cerr << "Usage: ./get_value <hostname> <port> <username> <table> <key>\n";
    return 1;
  }

  std::string hostname = argv[1];
  std::string port = argv[2];
  std::string username = argv[3];
  std::string table = argv[4];
  std::string key = argv[5];

  int fd = open_clientfd(hostname.c_str(), port.c_str());
  if (fd < 0) handle_error("Couldn't connect to server");

  rio_t rio;
  rio_readinitb(&rio, fd);

  try {
    // LOGIN
    Message login_msg(MessageType::LOGIN, {username});
    if (!send_message(rio, fd, login_msg)) return 1;

    Message login_response;
    if (!receive_message(rio, login_response) || login_response.get_message_type() != MessageType::OK) {
      handle_error(login_response.get_quoted_text());
    }

    // GET
    Message get_msg(MessageType::GET, {table, key});
    if (!send_message(rio, fd, get_msg)) return 1;

    Message get_response;
    if (!receive_message(rio, get_response) || get_response.get_message_type() != MessageType::OK) {
      handle_error(get_response.get_quoted_text());
    }

    //TOP
    Message top_msg(MessageType::TOP);
    if (!send_message(rio, fd, top_msg)) return 1;
    
    Message top_response;
    if (!receive_message(rio, top_response) || top_response.get_message_type() != MessageType::DATA) {
      handle_error(top_response.get_quoted_text());
    }

    std::cout << top_response.get_value() << '\n';

    // BYE
    Message bye_msg(MessageType::BYE);
    if (!send_message(rio, fd, bye_msg)) return 1;

    Message bye_response;
    if (!receive_message(rio, bye_response) || bye_response.get_message_type() != MessageType::OK) {
      handle_error(bye_response.get_quoted_text());
    }
  } catch (const std::exception &e) {
    handle_error(e.what());
  }
  close(fd);
  return 0;
}