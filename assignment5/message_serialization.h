#ifndef MESSAGE_SERIALIZATION_H
#define MESSAGE_SERIALIZATION_H

#include "message.h"

namespace MessageSerialization {

  /*
  * Converts Message object to string 
  *
  * Parameters:
  *     msg - input Message object
  *     encoded_msg - the string conversion of the Message object
  */
  void encode(const Message &msg, std::string &encoded_msg);

  /*
  * Converts string to Message object
  *
  * Parameters:
  *     msg - Message object conversion of the string
  *     encoded_msg - the string message
  */
  void decode(const std::string &encoded_msg, Message &msg);
};

#endif // MESSAGE_SERIALIZATION_H
