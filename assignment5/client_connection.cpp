#include <iostream>
#include <cassert>
#include <string>
#include "csapp.h"
#include "guard.h"
#include "value_stack.h"
#include "message.h"
#include "message_serialization.h"
#include "server.h"
#include "exceptions.h"
#include "client_connection.h"

ClientConnection::ClientConnection( Server *server, int client_fd )
  : m_server( server )
  , m_client_fd( client_fd )
{
  rio_readinitb( &m_fdbuf, m_client_fd );
}

ClientConnection::~ClientConnection()
{
  // TODO: implement
   Close(m_client_fd);
}

bool ClientConnection::receive_message(rio_t &rio, Message &response) {
  char response_buf[1024 + 1] = {0};
  response.clear_args();
  int bytes_read = rio_readlineb(&rio, response_buf, 1024);
  if (bytes_read <= 0) {
    throw CommException("Failed to read server response.");
    return false;
  }

  try {
    std::string response_str(response_buf);
    MessageSerialization::decode(response_str, response); //string -> message
  } catch (const std::exception &e) {
    throw InvalidMessage("Failed to decode server response: " + std::string(e.what()));
    return false;
  }
  if (!response.is_valid()) {
    throw InvalidMessage("Message couldn't be processed because of missing or invalid data");
  }
  return true;
}

bool ClientConnection::send_message(rio_t &rio, int fd, const Message &message) {
  std::string serialized_message;
  MessageSerialization::encode(message, serialized_message);
  if (rio_writen(fd, serialized_message.data(), serialized_message.size()) != serialized_message.size()) {
    throw CommException("Failed to send message.");
  }
  return true;
}


void ClientConnection::chat_with_client()
{
  Message server_response(MessageType::OK);
  ValueStack values;
  bool sent_message = false;
  // TODO: implement
  try {
    Message login_response;
    if (!receive_message(m_fdbuf, login_response) || login_response.get_message_type() != MessageType::LOGIN) {
      server_response.set_message_type(MessageType::ERROR);
      throw InvalidMessage("First request must be LOGIN");
    }
    while (server_response.get_message_type() != MessageType::ERROR) {
      {
        Message client_msg;
        receive_message(m_fdbuf, client_msg);
        MessageType type = client_msg.get_message_type();
        if (type == MessageType::CREATE) { m_server->create_table(client_msg.get_table()); }
        if (type == MessageType::PUSH) { values.push(client_msg.get_value()); }
        if (type == MessageType::POP) { values.pop(); }
        if (type == MessageType::TOP) { 
          std::string top_value = values.get_top();
          Message top_msg(MessageType::DATA, {top_value});
          sent_message = true;
          send_message(m_fdbuf, m_client_fd, top_msg);
        }
        if (type == MessageType::SET) {
          Table table = client_msg.get_table();
          std::string key = client_msg.get_key();
          //does set pop the top value or just get the top value???
          std::string value = values.get_top();
          //values.pop();
          table.lock();
          table.set(key, value);
          table.unlock();
        } //need to guard?
        if (type == MessageType::GET) {
          Table table = client_msg.get_table();
          std::string key = client_msg.get_key();
          std::string value = table.get(key);
          values.push(value);
        }
        if (type == MessageType::ADD) { 
          int right = stoi(values.get_top());
          values.pop();
          right = stoi(values.get_top()) + right;
          values.pop();
          values.push(std::to_string(right));
        }
        if (type == MessageType::SUB) {
          int right = stoi(values.get_top());
          values.pop();
          right = stoi(values.get_top()) - right;
          values.pop();
          values.push(std::to_string(right));
        }
        if (type == MessageType::MUL) {
          int right = stoi(values.get_top());
          values.pop();
          right = stoi(values.get_top()) * right;
          values.pop();
          values.push(std::to_string(right));
        }
        if (type == MessageType::DIV) {
          int right = stoi(values.get_top());
          values.pop();
          right = stoi(values.get_top()) / right;
          values.pop();
          values.push(std::to_string(right));
        }
        if (type == MessageType::BEGIN) {}
        if (type == MessageType::COMMIT) {}
        if (type == MessageType::BYE) {}
        else { throw InvalidMessage("Message was not a request message"); }
        
        if (!sent_message) { send_message(m_fdbuf, m_client_fd, server_response); }
      }
    }
  } catch (CommException ex) { //unrecoverable
    return;
  } catch (InvalidMessage ex) { //unrecoverable
    return;
  }

  
}

