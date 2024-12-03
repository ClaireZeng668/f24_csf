#ifndef SERVER_H
#define SERVER_H

#include <map>
#include <string>
#include <pthread.h>
#include "table.h"
#include "client_connection.h"

class Server {
private:
  // TODO: add member variables
  int server_fd;
  std::vector<Table*> server_tables;
  std::vector<Table*> locked_tables;
  //bool transaction_in_progress = false;
  // copy constructor and assignment operator are prohibited
  Server( const Server & );
  Server &operator=( const Server & );

public:

  /*
  * Server object constructor
  */
  Server();

  /*
  * Server object destructor
  */
  ~Server();

  /*
  * Creates server socket
  *
  * Parameters:
  *     port - the port number for the socket
  *
  * Throws:
  *     CommException if failed to create server socket
  */
  void listen( const std::string &port );

  /*
  * Waits for client connections and creates threads to handle client requests
  */
  void server_loop();

  /*
  * Start routine for threads
  *
  * Parameters:
  *     arg - ClientConnection object to handle client
  */
  static void *client_worker( void *arg );

  /*
  * Prints error message
  */
  void log_error( const std::string &what );

  /*
  * Creates server table
  *
  * Parameters:
  *     name - the specified name for the table
  *
  * Throws:
  *     OperationException if table already exists
  */
  void create_table( const std::string &name );

  /*
  * Checks if table exists in the server
  *
  * Parameters:
  *     name - the specified name for the table
  *
  * Returns:
  *     Table pointer to the specified table
  *     NULL is table does not exist in the server
  */
  Table *find_table( const std::string &name );

  /*
  * Adds a table to the server
  *
  * Parameters:
  *     to_add - the specified table to add to the server
  */
  void add_table( Table* to_add );

};


#endif // SERVER_H