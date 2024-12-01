#include <iostream>
#include <utility>
#include <sstream>
#include <cassert>
#include <string>
#include <map>
#include "exceptions.h"
#include "message.h"
#include "message_serialization.h"

void MessageSerialization::encode( const Message &msg, std::string &encoded_msg )
{
  encoded_msg.clear();
  MessageType type = msg.get_message_type();
  int num_args = msg.get_num_args();
  std::string args;
  if (type == MessageType::LOGIN) {encoded_msg = "LOGIN";}
  if (type == MessageType::CREATE) {encoded_msg = "CREATE";}
  if (type == MessageType::PUSH) {encoded_msg = "PUSH";}
  if (type == MessageType::POP) {encoded_msg = "POP";}
  if (type == MessageType::TOP) {encoded_msg = "TOP";}
  if (type == MessageType::SET) {encoded_msg = "SET";}
  if (type == MessageType::GET) {encoded_msg = "GET";}
  if (type == MessageType::ADD) {encoded_msg = "ADD";}
  if (type == MessageType::SUB) {encoded_msg = "SUB";}
  if (type == MessageType::MUL) {encoded_msg = "MUL";}
  if (type == MessageType::DIV) {encoded_msg = "DIV";}
  if (type == MessageType::BEGIN) {encoded_msg = "BEGIN";}
  if (type == MessageType::COMMIT) {encoded_msg = "COMMIT";}
  if (type == MessageType::BYE) {encoded_msg = "BYE";}
  if (type == MessageType::OK) {encoded_msg = "OK";}
  if (type == MessageType::FAILED) {encoded_msg = "FAILED";}
  if (type == MessageType::ERROR) {encoded_msg = "ERROR";}
  if (type == MessageType::DATA) {encoded_msg = "DATA";}
  for (int i = 0; i < num_args; i++) {
    std::string current = msg.get_arg(i);
    encoded_msg.append(" ");
    if (type == MessageType::FAILED || type == MessageType::ERROR) {
      encoded_msg.append("\"");
    }
    encoded_msg.append(current);
  }
  if (encoded_msg.length() > 1023) {
    throw InvalidMessage("resulting encoded message exceeds maximum length");
  }
  if (type == MessageType::FAILED || type == MessageType::ERROR) {
    encoded_msg.append("\"");
  }
  encoded_msg.append("\n");
}

void MessageSerialization::decode( const std::string &encoded_msg_, Message &msg )
{
  msg.clear_args();
  char quote = '\n';
  if (encoded_msg_.back() != quote) {
    throw InvalidMessage("Message lacks terminating newline character");
  }
  if (encoded_msg_.length() > 1024) {
    throw InvalidMessage("source encoded message exceeds maximum length");
  }
  std::stringstream ss;
  ss << encoded_msg_;
  std::string command;
  ss >> command;
  if (command.compare("LOGIN") == 0) {msg.set_message_type(MessageType::LOGIN);}
  else if (command.compare("CREATE") == 0) {msg.set_message_type(MessageType::CREATE);}
  else if (command.compare("PUSH") == 0) {msg.set_message_type(MessageType::PUSH);}
  else if (command.compare("DATA") == 0) {msg.set_message_type(MessageType::DATA);}
  else if (command.compare("POP") == 0) {msg.set_message_type(MessageType::POP);}
  else if (command.compare("TOP") == 0) {msg.set_message_type(MessageType::TOP);}
  else if (command.compare("SET") == 0) {msg.set_message_type(MessageType::SET);}
  else if (command.compare("GET") == 0) {msg.set_message_type(MessageType::GET);}
  else if (command.compare("ADD") == 0) {msg.set_message_type(MessageType::ADD);}
  else if (command.compare("SUB") == 0) {msg.set_message_type(MessageType::SUB);}
  else if (command.compare("MUL") == 0) {msg.set_message_type(MessageType::MUL);}
  else if (command.compare("DIV") == 0) {msg.set_message_type(MessageType::DIV);}
  else if (command.compare("BEGIN") == 0) {msg.set_message_type(MessageType::BEGIN);}
  else if (command.compare("COMMIT") == 0) {msg.set_message_type(MessageType::COMMIT);}
  else if (command.compare("BYE") == 0) {msg.set_message_type(MessageType::BYE);}
  else if (command.compare("OK") == 0) {msg.set_message_type(MessageType::OK);}
  else if (command.compare("FAILED") == 0) {msg.set_message_type(MessageType::FAILED);}
  else if (command.compare("ERROR") == 0) {msg.set_message_type(MessageType::ERROR);}
  else {throw InvalidMessage("Invalid message command");}
  std::string arg;
  quote = '\"';
  while (ss >> arg) {
    if (arg.front() == quote) {
      arg.append(" ");
      std::string text;
      ss >> text;
      while (text.back() != quote) {
        arg.append(text + " ");
        ss >> text;
      }
      arg.append(text);
      arg.pop_back();
      arg.erase(0,1);
    }
    msg.push_arg(arg);;
  }
  // if (!msg.is_valid()) {
  //   throw InvalidMessage("resuling Message object is not valid");
  // }
}