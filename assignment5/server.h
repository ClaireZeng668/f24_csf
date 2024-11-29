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
  bool transaction_in_progress = false;
  // copy constructor and assignment operator are prohibited
  Server( const Server & );
  Server &operator=( const Server & );

public:
  Server();
  ~Server();

  void listen( const std::string &port );
  void server_loop();

  static void *client_worker( void *arg );

  void log_error( const std::string &what );
  void create_table( const std::string &name );
  Table *find_table( const std::string &name );
  void add_table( Table* to_add );
  bool has_transaction() { return transaction_in_progress; }
  void start_transaction() { transaction_in_progress = true; }
  void end_transaction() { transaction_in_progress = false; }
  // TODO: add member functions

  // Some suggested member functions:
/*
  void create_table( const std::string &name );
  Table *find_table( const std::string &name );
  void log_error( const std::string &what );
*/
};


#endif // SERVER_H
