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
    response.set_message_type(MessageType::ERROR);
    throw CommException("Failed to read server response.");
    return false;
  }

  try {
    std::string response_str(response_buf);
    MessageSerialization::decode(response_str, response); //string -> message
  } catch (const std::exception &e) {
    response.set_message_type(MessageType::ERROR);
    throw InvalidMessage("Failed to decode server response: " + std::string(e.what()));
    return false;
  }
  if (!response.is_valid()) {
    response.set_message_type(MessageType::ERROR);
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

void ClientConnection::set_request(ValueStack &values, Message client_msg, bool try_lock) {
  std::string table_name = client_msg.get_table();
  std::string key = client_msg.get_key();
  std::string value = values.get_top();
  values.pop();
  Table *table = m_server->find_table(table_name);
  if (table == NULL) {
    throw OperationException("Table does not exist");
  }
  if (try_lock) {
    table->trylock();
    table->set(key, value);
  } else { 
    table->lock();
    table->set(key, value);
    table->commit_changes();
    table->unlock();
  }
}

void ClientConnection::math_request(ValueStack &values, std::string operation) { //TODO: check if int?
  int right = stoi(values.get_top());
  values.pop();
  int left = stoi(values.get_top());
  values.pop();
  int result;
  if (operation == "add") {
    result = right + left;
  } else if (operation == "sub") {
    result = left - right;
  } else if (operation == "mul") {
    result = left * right;
  } else {
    result = left / right;
  }
  values.push(std::to_string(result));
}

void ClientConnection::execute_transaction(ValueStack &values, bool &sent_message) {
  Message server_response(MessageType::OK);
  bool did_request = false;
  Message client_msg;
  //invlid message and comm exception exit execute and chat back to server?
  //try {
  send_message(m_fdbuf, m_client_fd, server_response);
  while (client_msg.get_message_type() != MessageType::COMMIT && client_msg.get_message_type() != MessageType::BYE) {
    receive_message(m_fdbuf, client_msg);
    MessageType type = client_msg.get_message_type();
    did_request = regular_requests(type, client_msg, values, /*sent_message, */true);
    if (!did_request) {
      throw InvalidMessage("Message request was not a valid request"); //TODO: have to commit before bye?
    }
  }
  for (auto it = client_locked_tables.begin(); it != client_locked_tables.end(); it++) {
    Table *table = *it;
    table->commit_changes();
    send_message(m_fdbuf, m_client_fd, server_response);
  }
  // } catch (FailedTransaction ex) {
  //   for (auto it = client_locked_tables.begin(); it != client_locked_tables.end(); it++) {
  //     Table *table = *it;
  //     table->rollback_changes();
  //   }
  // } catch (OperationException ex) {
  //   for (auto it = client_locked_tables.begin(); it != client_locked_tables.end(); it++) {
  //     Table *table = *it;
  //     table->rollback_changes();
  //   }
  // }
}

bool ClientConnection::regular_requests( MessageType type, Message client_msg, ValueStack &values, /*bool &sent_message, */bool try_lock) {
  bool completed_request = false;
  bool sent_message = false;
  Message ok_response(MessageType::OK);
  if (type == MessageType::CREATE) {
    if (!client_msg.is_valid()) {
      throw InvalidMessage("Invalid table name");
    }
    std::string table_name = client_msg.get_table();
    m_server->create_table(table_name); 
    completed_request = true;
    if (try_lock) {
      Table* table = m_server->find_table(table_name);
      table->trylock();
      client_locked_tables.push_back(table);
    }
  }
  else if (type == MessageType::PUSH) { 
    values.push(client_msg.get_value()); //TODO: has to be int?
    completed_request = true;
  }
  else if (type == MessageType::POP) { 
    values.pop(); 
    completed_request = true;
  }
  else if (type == MessageType::TOP) { 
    std::string top_value = values.get_top();
    Message top_msg(MessageType::DATA, {top_value});
    sent_message = true;
    send_message(m_fdbuf, m_client_fd, top_msg);
    completed_request = true;
  }
  else if (type == MessageType::SET) {
    set_request(values, client_msg, try_lock);
    completed_request = true;
  }
  else if (type == MessageType::GET) {
    std::string table_name = client_msg.get_table();
    Table* table = m_server->find_table(table_name);
    if (table == NULL) {
      throw OperationException("Table does not exist");
    }
    if (try_lock) {
      table->trylock();
      client_locked_tables.push_back(table);
    }
    std::string key = client_msg.get_key();
    std::string value = table->get(key);
    values.push(value);
    completed_request = true;
  }
  else if (type == MessageType::ADD) { 
    math_request(values, "add");
    completed_request = true;
  }
  else if (type == MessageType::SUB) {
    math_request(values, "sub");
    completed_request = true;
  }
  else if (type == MessageType::MUL) {
    math_request(values, "mul");
    completed_request = true;
  }
  else if (type == MessageType::DIV) {
    math_request(values, "div");
    completed_request = true;
  } else if (type == MessageType::COMMIT) {
    return true;
  }
  
  if (!sent_message && completed_request) { 
    send_message(m_fdbuf, m_client_fd, ok_response); 
  }
  return completed_request;
}

void ClientConnection::unlock_all() {
  for (auto it = client_locked_tables.begin(); it != client_locked_tables.end(); it++) {
    Table *table = *it;
    table->rollback_changes();
  }
  client_locked_tables.clear();
}

void ClientConnection::chat_with_client() {
  ValueStack values;
  bool sent_message = false;
  bool did_request = false;
  // TODO: implement
  Message login_response;
  receive_message(m_fdbuf, login_response);
  try {
    if (login_response.get_message_type() != MessageType::LOGIN) {
      throw InvalidMessage("First request must be LOGIN");
    }
    if (!login_response.is_valid()) {
      throw InvalidMessage("invalid username");
    }
    Message client_msg;
    Message done(MessageType::OK);
    send_message(m_fdbuf, m_client_fd, done);
    while (1) {//server_response.get_message_type() != MessageType::ERROR) { throwing invalidmessage or commexception will exit chat with client?
      try {
        receive_message(m_fdbuf, client_msg);
        MessageType type = client_msg.get_message_type();
        did_request = regular_requests(type, client_msg, values, /*sent_message, */false);
        if (!did_request) {
          if (type == MessageType::BEGIN) {
            if (m_server->has_transaction()) {
              throw FailedTransaction("Transaction already in progress");
            }
            m_server->start_transaction();
            execute_transaction(values, sent_message);
            m_server->end_transaction();
          } else if (type == MessageType::BYE) {  //need to check stuff?
            unlock_all();
            send_message(m_fdbuf, m_client_fd, done);
            return;
          } else { throw InvalidMessage("Message request was not a valid request"); }
        }
      } catch (OperationException ex) { //recoverable
        unlock_all();
        Message response(MessageType::FAILED, {ex.what()});
        send_message(m_fdbuf, m_client_fd, response);
      } catch (FailedTransaction ex) { //recoverable
        unlock_all();
        Message response(MessageType::FAILED, {ex.what()});
        send_message(m_fdbuf, m_client_fd, response);
      } 
    }
  } catch (const std::exception &e) {
    Message response(MessageType::ERROR, {e.what()});
    send_message(m_fdbuf, m_client_fd, response);
    return;
  }
}

