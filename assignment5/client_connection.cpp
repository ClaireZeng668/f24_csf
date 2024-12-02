#include <iostream>
#include <cassert>
#include <string>
#include "csapp.h"
#include "guard.h"
#include "table.h"
#include "value_stack.h"
#include "message.h"
#include "message_serialization.h"
#include "server.h"
#include "exceptions.h"
#include "client_connection.h"

ClientConnection::ClientConnection( Server *server, int client_fd )
  : m_server( server )
  , m_client_fd( client_fd )
  , client_locked_tables()
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
    unlock_all();
    throw CommException("Failed to read server response.");
    return false;
  }

  //try {
    std::string response_str(response_buf);
    MessageSerialization::decode(response_str, response); //string -> message
  // } catch (const std::exception &e) {
  //   response.set_message_type(MessageType::ERROR);
  //   unlock_all();
  //   throw InvalidMessage(e.what());
  //   return false;
  // }
  if (response.get_message_type() != MessageType::LOGIN) {
    if (!response.is_valid()) {
      response.set_message_type(MessageType::ERROR);
      unlock_all();
      throw InvalidMessage("Message couldn't be processed because of missing or invalid data");
    }
  }
  return true;
}

bool ClientConnection::send_message(rio_t &rio, int fd, const Message &message) {
  std::string serialized_message;
  MessageSerialization::encode(message, serialized_message);
  if (rio_writen(fd, serialized_message.data(), serialized_message.size()) != serialized_message.size()) {
    unlock_all();
    throw CommException("Failed to send message.");
  }
  return true;
}

bool ClientConnection::is_number(std::string value) {
  for (int i = 0; i < value.length(); i++) {
    if (!isdigit(value.at(i))) {
      return false;
    }
  }
  return true;
}

void ClientConnection::math_request(ValueStack &values, std::string operation) {
  std::string right_value = values.get_top();
  values.pop();
  std::string left_value = values.get_top();
  values.pop();
  if (!is_number(right_value) || !is_number(left_value)) {
    throw OperationException("Attempted arithmetic operation on non-numeric values");
  }
  int right = stoi(right_value);
  int left = stoi(left_value);
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

bool ClientConnection::execute_transaction(ValueStack &values, bool &sent_message) {
  Message server_response(MessageType::OK);
  bool did_request = false;
  Message client_msg;
  //invlid message and comm exception exit execute and chat back to server?
  //try {
  send_message(m_fdbuf, m_client_fd, server_response);
  receive_message(m_fdbuf, client_msg);
  MessageType type = client_msg.get_message_type();
  while (type != MessageType::COMMIT) {
    if (type == MessageType::BYE) {
      unlock_all();
      send_message(m_fdbuf, m_client_fd, server_response);
      return true;
    }
    did_request = regular_requests(type, client_msg, values, /*sent_message, */true);
    if (!did_request) {
      unlock_all();
      throw InvalidMessage("Message request was not a valid request");
    }
    receive_message(m_fdbuf, client_msg);
    type = client_msg.get_message_type();
  }
  
  for (auto it = client_locked_tables.begin(); it != client_locked_tables.end(); it++) {
    Table *table = *it;
    table->commit_changes();
    if (table->created_in_transaction()) {
      m_server->add_table(table);
    }
    table->unlock();
    send_message(m_fdbuf, m_client_fd, server_response);
  }
  return false;
}

Table* ClientConnection::find_table(std::string &name) {
  for (auto it = client_locked_tables.begin(); it != client_locked_tables.end(); it++) {
    if ((*it)->get_name() == name) {
      return *it;
    }
  }
  return NULL;
}

Table* ClientConnection::create_transaction_table(std::string table_name) {
  Table* new_table = find_table(table_name);
  if (new_table == NULL) {
    new_table = new Table(table_name);
    client_locked_tables.push_back(new_table);
  } else {
    throw OperationException("Table already exists");
  }
  return new_table;
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


void ClientConnection::get_request(ValueStack &values, Message client_msg, bool try_lock) {
  std::string table_name = client_msg.get_table();
  std::string key = client_msg.get_key();
  Table* table = m_server->find_table(table_name);
  if (table == NULL) {
    throw OperationException("Table does not exist");
  }
  if (try_lock) {
    table->trylock();
    client_locked_tables.push_back(table);
    values.push(table->get(key));
  } else {
    table->lock();
    values.push(table->get(key));
    table->unlock();
  }
}

bool ClientConnection::regular_requests( MessageType type, Message client_msg, ValueStack &values, /*bool &sent_message, */bool try_lock) {
  bool completed_request = false;
  bool sent_message = false;
  Message ok_response(MessageType::OK);
  if (type == MessageType::CREATE) {
    if (!client_msg.is_valid()) {
      unlock_all();
      throw InvalidMessage("Invalid table name");
    }
    std::string table_name = client_msg.get_table();
    if (try_lock) {
      Table* new_table = create_transaction_table(table_name);
      new_table->trylock();
      new_table->set_created();
    } else {
      m_server->create_table(table_name);
      completed_request = true;
    }
  }
  else if (type == MessageType::PUSH) { 
    if (!client_msg.is_valid()) {
      unlock_all();
      throw InvalidMessage("Value contains whitespace characters");
    }
    values.push(client_msg.get_value());
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
    if (!client_msg.is_valid()) {
      unlock_all();
      throw InvalidMessage("Invalid message format");
    }
    set_request(values, client_msg, try_lock);
    completed_request = true;
  }
  else if (type == MessageType::GET) {
    if (!client_msg.is_valid()) {
      unlock_all();
      throw InvalidMessage("Invalid message format");
    }
    get_request(values, client_msg, try_lock);
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
  }
  if (!sent_message && completed_request) { 
    send_message(m_fdbuf, m_client_fd, ok_response); 
  }
  return completed_request;
}

void ClientConnection::unlock_all() {
  for (auto it = client_locked_tables.begin(); it != client_locked_tables.end(); it++) {
    Table *table = *it;
    if (table->created_in_transaction()) {
      table->~Table();
    } else {
      table->rollback_changes();
      table->unlock();
    }
  }
  client_locked_tables.clear();
  m_server->end_transaction();
}

void ClientConnection::chat_with_client() {
  ValueStack values;
  bool sent_message = false;
  bool did_request = false;
  // TODO: implement
  Message login_response;
  receive_message(m_fdbuf, login_response);
  try {
    //receive_message(m_fdbuf, login_response);
    if (login_response.get_message_type() != MessageType::LOGIN) {
      throw InvalidMessage("First request must be LOGIN");
    }
    if (!login_response.is_valid()) {
      throw InvalidMessage("Invalid username");
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
            bool log_out = execute_transaction(values, sent_message);
            m_server->end_transaction();
            if (log_out) {return;}
          } else if (type == MessageType::BYE) {  //need to check stuff?
            unlock_all();
            send_message(m_fdbuf, m_client_fd, done);
            return;
          } else { 
            unlock_all();
            throw InvalidMessage("Message request was not a valid request");
          }
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