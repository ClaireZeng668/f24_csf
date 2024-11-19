//incr_value.cpp
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
  if (argc != 6 && argc != 7) {
    std::cerr << "Usage: ./incr_value [-t] <hostname> <port> <username> <table> <key>\n";
    return 1;
  }
  
  // implementing transaction
  bool use_transaction = (argc == 7);
  std::string hostname = argv[use_transaction ? 2 : 1];
  std::string port = argv[use_transaction ? 3 : 2];
  std::string username = argv[use_transaction ? 4 : 3];
  std::string table = argv[use_transaction ? 5 : 4];
  std::string key = argv[use_transaction ? 6 : 5];
  
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

    //BEGIN
    if (use_transaction) {
      Message begin_msg(MessageType::BEGIN);
      if (!send_message(rio, fd, begin_msg)) return 1;

      Message begin_response;
      if (!receive_message(rio, begin_response) || begin_response.get_message_type() != MessageType::OK) {
        handle_error(begin_response.get_quoted_text());
      }
    }

    // GET
    Message get_msg(MessageType::GET, {table, key});
    if (!send_message(rio, fd, get_msg)) return 1;

    Message get_response;
    if (!receive_message(rio, get_response) || get_response.get_message_type() != MessageType::OK) {
      handle_error(get_response.get_quoted_text());
    }

    // PUSH
    std::string one = "1";
    Message push_msg(MessageType::PUSH, {one});
    if (!send_message(rio, fd, push_msg)) return 1;

    Message push_response;
    if (!receive_message(rio, push_response) || push_response.get_message_type() != MessageType::OK) {
      handle_error(push_response.get_quoted_text());
    }

    //ADD
    Message add_msg(MessageType::ADD);
    if (!send_message(rio, fd, add_msg)) return 1;

    Message add_response;
    if (!receive_message(rio, add_response) || add_response.get_message_type() != MessageType::OK) {
      handle_error(add_response.get_quoted_text());
    }

    // SET
    Message set_msg(MessageType::SET, {table, key});
    if (!send_message(rio, fd, set_msg)) return 1;

    Message set_response;
    if (!receive_message(rio, set_response) || set_response.get_message_type() != MessageType::OK) {
      handle_error(set_response.get_quoted_text());
    }

    //COMMIT
    if (use_transaction) {
      Message commit_msg(MessageType::COMMIT);
      if (!send_message(rio, fd, commit_msg)) return 1;

      Message commit_response;
      if (!receive_message(rio, commit_response) || commit_response.get_message_type() != MessageType::OK) {
        handle_error(commit_response.get_quoted_text());
      }
    }

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