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

void ClientConnection::receive_message(rio_t &rio, Message &request) {
  char response_buf[1024 + 1] = {0};
  request.clear_args();
  int bytes_read = rio_readlineb(&rio, response_buf, 1024);
  if (bytes_read <= 0) {
    request.set_message_type(MessageType::ERROR);
    unlock_all();
    throw CommException("Failed to read client request.");
  }

  std::string response_str(response_buf);
  MessageSerialization::decode(response_str, request); //string -> message
  if (request.get_message_type() != MessageType::LOGIN) {
    if (!request.is_valid()) {
      request.set_message_type(MessageType::ERROR);
      unlock_all();
      throw InvalidMessage("Message couldn't be processed because of missing or invalid data");
    }
  }
}

void ClientConnection::send_message(rio_t &rio, int fd, const Message &message) {
  std::string serialized_message;
  MessageSerialization::encode(message, serialized_message);
  if (rio_writen(fd, serialized_message.data(), serialized_message.size()) != serialized_message.size()) {
    unlock_all();
    throw CommException("Failed to send message.");
  }
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

bool ClientConnection::execute_transaction(ValueStack &values) {
  Message server_response(MessageType::OK);
  bool did_request = false;
  Message client_msg;
  send_message(m_fdbuf, m_client_fd, server_response);
  receive_message(m_fdbuf, client_msg);
  MessageType type = client_msg.get_message_type();
  while (type != MessageType::COMMIT) {
    if (type == MessageType::BYE) {
      unlock_all();
      send_message(m_fdbuf, m_client_fd, server_response);
      return true;
    }
    did_request = regular_requests(type, client_msg, values, true);
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
  }
  client_locked_tables.clear();
  send_message(m_fdbuf, m_client_fd, server_response);
  end_transaction();
  return false;
}

bool ClientConnection::find_table(std::string &name) {
  for (auto it = client_locked_tables.begin(); it != client_locked_tables.end(); it++) {
    if ((*it)->get_name() == name) {
      return true;
    }
  }
  return false;
}

Table* ClientConnection::create_transaction_table(std::string table_name) {
  if (!find_table(table_name)) {
    Table* new_table = new Table(table_name);
    client_locked_tables.push_back(new_table);
    return new_table;
  } else {
    throw OperationException("Table already exists");
  }
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
    if (!find_table(table_name)) {
      table->trylock();
      client_locked_tables.push_back(table);
    }
    table->set(key, value);
  } else { 
    {
      pthread_mutex_t mutex = table->get_mutex();
      Guard g(mutex);
      table->set(key, value);
      table->commit_changes();
    }
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
    if (!find_table(table_name)) {
      table->trylock();
      client_locked_tables.push_back(table);
    }
    values.push(table->get(key));
  } else {
    {
      pthread_mutex_t mutex = table->get_mutex();
      Guard g(mutex);
      values.push(table->get(key));
    }
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
  end_transaction();
}

void ClientConnection::chat_with_client() {
  ValueStack values;
  bool sent_message = false;
  bool did_request = false;
  Message login_response;
  receive_message(m_fdbuf, login_response);
  try {
    if (login_response.get_message_type() != MessageType::LOGIN) {
      throw InvalidMessage("First request must be LOGIN");
    }
    if (!login_response.is_valid()) {
      throw InvalidMessage("Invalid username");
    }
    Message client_msg;
    Message done(MessageType::OK);
    send_message(m_fdbuf, m_client_fd, done);
    while (1) {
      try {
        receive_message(m_fdbuf, client_msg);
        MessageType type = client_msg.get_message_type();
        did_request = regular_requests(type, client_msg, values, false);
        if (!did_request) {
          if (type == MessageType::BEGIN) {
            if (has_transaction()) {
              throw FailedTransaction("Transaction already in progress");
            }
            start_transaction();
            bool log_out = execute_transaction(values);
            end_transaction();
            if (log_out) {return;}
          } else if (type == MessageType::BYE) {
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