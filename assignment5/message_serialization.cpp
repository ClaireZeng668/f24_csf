#include <utility>
#include <sstream>
#include <cassert>
#include <map>
#include "exceptions.h"
#include "message.h"
#include "message_serialization.h"

void MessageSerialization::encode( const Message &msg, std::string &encoded_msg )
{
  // TODO: implement
  MessageType type = msg.get_message_type();
  int num_args = msg.get_num_args();
  std::string args;
  for (int i = 0; i < num_args; i++) {
    
  }

}

void MessageSerialization::decode( const std::string &encoded_msg_, Message &msg )
{
  // TODO: implement
}
