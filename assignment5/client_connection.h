#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#include <set>
#include "message.h"
#include "csapp.h"
#include "value_stack.h"

class Server; // forward declaration
class Table; // forward declaration

class ClientConnection {
private:
  Server *m_server;
  int m_client_fd;
  rio_t m_fdbuf;
  std::vector<Table*> client_locked_tables;

  // copy constructor and assignment operator are prohibited
  ClientConnection( const ClientConnection & );
  ClientConnection &operator=( const ClientConnection & );

public:
  ClientConnection( Server *server, int client_fd );
  ~ClientConnection();

  void chat_with_client();
  bool receive_message(rio_t &rio, Message &response);
  bool send_message(rio_t &rio, int fd, const Message &message);

  // TODO: additional member functions
  void set_request(ValueStack &values, Message client_msg, bool try_lock);
  void get_request(ValueStack &values, Message client_msg, bool try_lock);
  void math_request(ValueStack &values, std::string operation);
  bool execute_transaction(ValueStack &values, bool &sent_message);
  bool regular_requests( MessageType type, Message client_msg, ValueStack &values, /*bool &sent_message, */ bool try_lock);
  void unlock_all();
  bool is_number(std::string value);
  Table* create_transaction_table(std::string table_name);
  bool find_table(std::string &name);

};

#endif // CLIENT_CONNECTION_H