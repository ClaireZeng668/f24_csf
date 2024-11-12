#include <set>
#include <map>
#include <regex>
#include <cassert>
#include "message.h"

Message::Message()
  : m_message_type(MessageType::NONE)
{
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

bool Message::is_valid() const
{
  switch (m_message_type) {
    case //LOGIN CREATE FAILED ERROR
      if (this->get_num_args() > 1) {return false;}
      if (!isalpha(this->get_arg(0)[0])) {return false;}
    case //SET GET
      if (this->get_num_arr()!=2) {return false;}
      if (!isalpha(this->get_arg(0)[0]) || !isalpha(this->get_arg(1)[0])) {return false;}
    case //DATA PUSH
      if (!isspace(this->get_arg(0)[0])) {return false;}
    case //everything else?
      if (this->get_num_args()!=0) {return false;}
  }
  
  
  return true;
}
