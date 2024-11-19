//set_value.cpp
#include "csapp.h"
#include "message.h"
#include <iostream>
#include "message_serialization.h"

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
  if (argc != 7) {
    std::cerr << "Usage: ./set_value <hostname> <port> <username> <table> <key> <value>\n";
    return 1;
  }

  std::string hostname = argv[1], port = argv[2], username = argv[3];
  std::string table = argv[4], key = argv[5], value = argv[6];

  int fd = open_clientfd(hostname.c_str(), port.c_str());
  if (fd < 0) handle_error("Couldn't connect to server");

  rio_t rio;
  rio_readinitb(&rio, fd);

  try {
    // LOGIN
    Message login_msg(MessageType::LOGIN, {username});
    if (!send_message(rio, fd, login_msg)) throw std::runtime_error("Login message failed to send");
    
    Message login_response;
    if (!receive_message(rio, login_response)) throw std::runtime_error("Login response failed");
    if (login_response.get_message_type() != MessageType::OK) {
      throw std::runtime_error(login_response.get_quoted_text());
    }

    Message login_msg(MessageType::LOGIN, {username});
    if (!send_message(rio, fd, login_msg)) return 1;

    Message server_response;
    if (!receive_message(rio, server_response) || server_response.get_message_type() != MessageType::OK) {
      handle_error(server_response.get_quoted_text());
    }

    // SET
    Message set_msg(MessageType::SET, {table, key, value});
    if (!send_message(rio, fd, set_msg)) throw std::runtime_error("Set message failed to send");
    Message set_response;
    if (!receive_message(rio, set_response)) throw std::runtime_error("Set response failed");
    if (set_response.get_message_type() != MessageType::OK) {
      throw std::runtime_error(set_response.get_quoted_text());
    }

    // BYE
    Message bye_msg(MessageType::BYE);
    if (!send_message(rio, fd, bye_msg)) return 1;
    Message bye_response;
    if (!receive_message(rio, bye_response) || bye_response.get_message_type() != MessageType::DATA) {
      handle_error(bye_response.get_quoted_text());
    }
  } catch (const std::exception &e) {
    handle_error(e.what());
  }

  close(fd);
  return 0;
}