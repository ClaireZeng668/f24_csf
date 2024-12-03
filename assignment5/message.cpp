#include <cctype>
#include <set>
#include <map>
#include <regex>
#include <cassert>
#include <stdexcept>
#include "message.h"

Message::Message()
  : m_message_type(MessageType::NONE)
{
  std::vector<std::string> m_args;
}

Message::Message( MessageType message_type, std::initializer_list<std::string> args )
  : m_message_type( message_type )
  , m_args( args )
{
}

Message::Message( const Message &other )
  : m_message_type( other.m_message_type )
  , m_args( other.m_args )
{
}

Message::~Message()
{
}

Message &Message::operator=( const Message &rhs )
{
  this->m_message_type = rhs.get_message_type();
  std::vector<std::string> rhs_args;
  int num_args = rhs.get_num_args();
  for (int i = 0; i < num_args; i++) {
    rhs_args.at(i) = rhs.get_arg(i);
  }
  this->m_args = rhs_args;
  return *this;
}

MessageType Message::get_message_type() const
{
  return m_message_type;
}

void Message::set_message_type(MessageType message_type)
{
  m_message_type = message_type;
}

std::string Message::get_username() const
{
  return m_args.at(0);
}

std::string Message::get_table() const
{
  return m_args.at(0);
}

std::string Message::get_key() const
{
  return m_args.at(1);
}

std::string Message::get_value() const
{
  return m_args.at(0);
}

std::string Message::get_quoted_text() const
{
  return m_args.at(0);
}

void Message::push_arg( const std::string &arg )
{
  m_args.push_back( arg );
}

bool Message::is_valid_identifier(std::string ident) const {
  int ident_length = ident.length();
  if (ident_length < 2) {return false;}
  for (int i = 1; i < ident_length; i++) {
    char current = ident.at(i);
    if (!isalnum(current) && current != '_') {
      return false;
    }
  }
  return true;
}

std::string Message::get_arg(unsigned i) const {
  try {
    return m_args.at( i );
  } catch (std::out_of_range ex) {
    return NULL;
  }
}

bool Message::is_valid() const
{
  if (m_message_type == MessageType::LOGIN || m_message_type == MessageType::CREATE) {
    if (m_args.size() != 1) {
      return false;
    }
    if (!isalpha(m_args.at(0).at(0)) || !is_valid_identifier(m_args.at(0))) {
      return false;
    }
  } else if (m_message_type == MessageType::FAILED || m_message_type == MessageType::ERROR) {
    if (m_args.size() != 1) {
      return false;
    }
  } else if (m_message_type == MessageType::SET || m_message_type == MessageType::GET) {
    if (m_args.size() != 2) {
      return false;
    }
    if (!isalpha(m_args.at(0).at(0)) || !isalpha(m_args.at(1).at(0))) {
      return false;
    }
    if (!is_valid_identifier(m_args.at(0)) || !is_valid_identifier(m_args.at(1))) {
      return false;
    }
  } else if (m_message_type == MessageType::PUSH || m_message_type == MessageType::DATA) {
    if (m_args.size() != 1) {
      return false;
    }
    std::string value = m_args.at(0);
    for (int i = 0; i < value.length(); i++) {
      if (isspace(value.at(0))) {
        return false;
      }
    }
  } else {
    if (m_args.size() != 0) {
      return false;
    }
  }
  return true;
}