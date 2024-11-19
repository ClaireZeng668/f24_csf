//incr_value.cpp
//reverted to old implementation

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

  /*
  std::string hostname = argv[count++];
  std::string port = argv[count++];
  std::string username = argv[count++];
  std::string table = argv[count++];
  std::string key = argv[count++];
  */

  
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
    if (!send_message(rio, fd, login_msg)) throw std::runtime_error("Login message failed to send");
    Message login_response;
    if (!receive_message(rio, login_response)) throw std::runtime_error("Login response failed");
    if (login_response.get_message_type() != MessageType::OK) {
      throw std::runtime_error(login_response.get_quoted_text());
    }

    if (use_transaction) {
      Message begin_msg(MessageType::BEGIN);
      if (!send_message(rio, fd, begin_msg)) throw std::runtime_error("Begin transaction message failed to send");
      Message begin_response;
      if (!receive_message(rio, begin_response)) throw std::runtime_error("Begin response failed");
      if (begin_response.get_message_type() != MessageType::OK) {
        throw std::runtime_error(begin_response.get_quoted_text());
      }
    }

    // GET
    Message get_msg(MessageType::GET, {table, key});
    if (!send_message(rio, fd, get_msg)) throw std::runtime_error("Get message failed to send");
    Message get_response;
    if (!receive_message(rio, get_response)) throw std::runtime_error("Get response failed");
    if (get_response.get_message_type() != MessageType::DATA) {
      throw std::runtime_error(get_response.get_quoted_text());
    }

    //int current_value = std::stoi(get_response.get_value());

    // PUSH
    Message push_msg(MessageType::PUSH, {table, key, std::to_string(current_value + 1)});
    if (!send_message(rio, fd, set_msg)) throw std::runtime_error("Set message failed to send");
    Message push_response;
    if (!receive_message(rio, push_response)) throw std::runtime_error("Set response failed");
    if (push_response.get_message_type() != MessageType::OK) {
      throw std::runtime_error(push_response.get_quoted_text());
    }

    // if (use_transaction) {
    //   Message commit_msg(MessageType::COMMIT);
    //   send_message(rio, fd, commit_msg);
    // }

    //ADD
    Message add_msg(MessageType::ADD);
    if (!send_message(rio, fd, add_msg)) throw std::runtime_error("add message failed to send");
    Message add_response;
    if (!receive_message(rio, add_response)) throw std::runtime_error("add response failed");
    if (add_response.get_message_type() != MessageType::OK) {
      throw std::runtime_error(add_response.get_quoted_text());
    }


    // SET
    Message set_msg(MessageType::SET, {table, key, std::to_string(current_value + 1)});
    send_message(rio, fd, set_msg);
    Message set_response;
    receive_message(rio, set_response);
    if (set_response.get_message_type() != MessageType::OK) {
      handle_error(set_response.get_quoted_text());
    }

    if (use_transaction) {
      Message commit_msg(MessageType::COMMIT);
      send_message(rio, fd, commit_msg);
      Message commit_response;
      receive_message(rio, commit_response);
      if (commit_response.get_message_type() != MessageType::OK) {
          handle_error(commit_response.get_quoted_text());
      }
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