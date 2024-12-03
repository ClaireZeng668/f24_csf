#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <string>

enum class MessageType {
  // Used only for uninitialized Message objects
  NONE,

  // Requests
  LOGIN,
  CREATE,
  PUSH,
  POP,
  TOP,
  SET,
  GET,
  ADD,
  SUB,
  MUL,
  DIV,
  BEGIN,
  COMMIT,
  BYE,

  // Responses
  OK,
  FAILED,
  ERROR,
  DATA,
};

class Message {
private:
  MessageType m_message_type;
  std::vector<std::string> m_args;

public:
  // Maximum encoded message length (including terminator newline character)
  static const unsigned MAX_ENCODED_LEN = 1024;
  
  /*
  * Empty Message object constructor
  */
  Message();

  /*
  * Message object constructor from MessageType and arguments
  */
  Message( MessageType message_type, std::initializer_list<std::string> args = std::initializer_list<std::string>() );

  /*
  * Copy constructor from another MessageObject
  */
  Message( const Message &other );

  /*
  * Message object destructor
  */
  ~Message();

  /*
  * Message object assignment operator
  */
  Message &operator=( const Message &rhs );

  /*
  * Gets the Message type
  *
  * Returns:
  *     MessageType for the Message
  */
  MessageType get_message_type() const;

  /*
  * Sets the Message type
  */
  void set_message_type( MessageType message_type );

  /*
  * Gets the username argument of the message
  *
  * Returns:
  *     username as a string
  */
  std::string get_username() const;

  /*
  * Gets the table argument of the message
  *
  * Returns:
  *     table as a string
  */
  std::string get_table() const;

  /*
  * Gets the key argument of the message
  *
  * Returns:
  *     key as a string
  */
  std::string get_key() const;

  /*
  * Gets the value argument of the message
  *
  * Returns:
  *     value as a string
  */
  std::string get_value() const;

  /*
  * Gets the text specified by the message
  * Only part of ERROR and FAILED messages
  *
  * Returns:
  *     text as a string
  */
  std::string get_quoted_text() const;

  /*
  * Pushes a value onto the ValueStack
  *
  * Parameters:
  *     arg - string value to push onto the ValueStack
  *
  * Returns:
  *     text as a string
  */
  void push_arg( const std::string &arg );

  /*
  * Checks if Message is a valid Messsage object (as specified in the assignment)
  *
  * Returns:
  *     boolean true if Message is valid
  */
  bool is_valid() const;

  /*
  * Checks if username, table, or key is a valid identifier
  * As specified in the assignment: starts with a letter and consists of letters, underscores, or digits
  *
  * Parameters:
  *     ident - string identifier to check
  *
  * Returns:
  *     boolean true if identifier is valid
  */
  bool is_valid_identifier(std::string ident) const;

  /*
  * Gets the number of arguments of the Message
  *
  * Returns:
  *     unsigned number of arguments
  */
  unsigned get_num_args() const { return m_args.size(); }
  
  /*
  * Gets the argument specified by parameter
  *
  * Parameters:
  *     i - unsigned value of index of argument
  *
  * Returns:
  *     string argument
  *     NULL if out of range
  */
  std::string get_arg( unsigned i ) const;

  /*
  * Empties Message arguments vector
  */
  void clear_args() {m_args.clear();}
};

#endif // MESSAGE_H