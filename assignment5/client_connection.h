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

  bool transaction_in_progress = false;

  // copy constructor and assignment operator are prohibited
  ClientConnection( const ClientConnection & );
  ClientConnection &operator=( const ClientConnection & );

public:
  
  /*
  * ClientConnection object constructor
  * 
  * Parameters:
  *     server - server object client is connected to
  *     client_fd - int value specifying the client fd
  */
  ClientConnection( Server *server, int client_fd );

  /*
  * ClientConnection object destructor
  */
  ~ClientConnection();

  /*
  * Primary function to deal with client requests
  *
  * Throws:
  *     Exceptions as described in following functions
  */
  void chat_with_client();

  /*
  * Handles receiving client requests
  *
  * Parameters:
  *     rio - rio_t value for the client fd
  *     request - Message object created from the client request
  *
  * Throws:
  *     CommException if failed to read client request
  *     InvalidMessage if client request decodes to an invalid message
  */
  void receive_message(rio_t &rio, Message &request);

  /*
  * Handles sending server responses
  *
  * Parameters:
  *     rio - rio_t value for the client fd
  *     fd - the client fd
  *     message - Message object containing the server response
  *
  * Throws:
  *     CommException if failed to send server response
  */
  void send_message(rio_t &rio, int fd, const Message &message);
  
  /*
  * Handles set requests
  *
  * Parameters:
  *     values - ValueStack for the client/thread
  *     client_msg - the Message object containing the set request
  *     try_lock - boolean of whether the set request was part of a transaction
  *         true if part of transaction
  * Throws:
  *     OperationException if table does not exist or ValueStack is empty
  *     FailedTransaction if request is part of a transaction and table is already locked
  */
  void set_request(ValueStack &values, Message client_msg, bool try_lock);

  /*
  * Handles get requests
  *
  * Parameters:
  *     values - ValueStack for the client/thread
  *     client_msg - the Message object containing the get request
  *     try_lock - boolean of whether the get request was part of a transaction
  *         true if part of transaction
  * Throws:
  *     OperationException if table does not exist
  *     FailedTransaction if request is part of a transaction and table is already locked
  */
  void get_request(ValueStack &values, Message client_msg, bool try_lock);

  /*
  * Handles arithmetic requests
  *
  * Parameters:
  *     operation - string of which arithmetic operation to do
  *
  * Throws:
  *     OperationException if ValueStack does not contain two values or values are not integers
  */
  void math_request(ValueStack &values, std::string operation);

  /*
  * Handles transactions
  *
  * Parameters:
  *     values - ValueStack for the client/thread
  *
  * Returns:
  *     boolean true if BYE received before COMMIT
  *
  * Throws:
  *     CommException if failed to read client request or failed to send server response
  *     InvalidMessage if client request decodes to an invalid message
  *     OperationException if transaction attempts to create a table that already exists
  *     FailedTransaction if request is part of a transaction and table is already locked  */
  bool execute_transaction(ValueStack &values);

  /*
  * Handles requests that are not LOGIN, BEGIN, or BYE
  *
  * Parameters:
  *     type - the MessageType of the client request
  *     client_msg - the Message object of the client request
  *     values - ValueStack for the client/thread
  *     try_lock - boolean of whether the request was part of a transaction
  *         true if part of transaction
  *
  * Returns:
  *     boolean true if request was received and executed
  *
  * Throws:
  *     OperationException if attempts to create a table that already exists
  *                        if attempts ValueStack operation on an empty ValueStack
  *                        if ValueStack does not contain two values or values are not integers
  *     InvalidMessage if client request decodes to an invalid message
  *     CommException if failed to send server response
  *     FailedTransaction if request is part of a transaction and table is already locked
  */
  bool regular_requests( MessageType type, Message client_msg, ValueStack &values, bool try_lock);

  /*
  * Unlock all tables and rollback all uncommited changes made by client/thread
  *
  */
  void unlock_all();

  /*
  * Check if the string encodes an integer value
  * Used in arithmetic requests
  *
  * Parameters:
  *     value - string from ValueStack
  *
  * Returns:
  *     boolean true if an artimetic operation can be performed
  */
  bool is_number(std::string value);

  /*
  * Handles create requests in a transaction
  *
  * Parameters:
  *     table_name - name of the table client is trying to create
  *
  * Returns:
  *     Table object of the newly created table
  *
  * Throws:
  *     OperationException if attempts to create a table that already exists
  */
  Table* create_transaction_table(std::string table_name);

  /*
  * Checks if thread has the lock on the table
  *
  * Parameters:
  *     name - name of the table to check
  *
  * Returns:
  *     boolean true if thread has lock on table named
  */
  bool find_table(std::string &name);

  /*
  * Checks if thread/client is currently in a transaction
  *
  * Returns:
  *     boolean true if thread/client is in a transaction
  */
  bool has_transaction() { return transaction_in_progress; }

  void start_transaction() { transaction_in_progress = true; }
  void end_transaction() { transaction_in_progress = false; }

};

#endif // CLIENT_CONNECTION_H