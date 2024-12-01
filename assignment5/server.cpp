#include <iostream>
#include <cassert>
#include <memory>
#include <algorithm>
#include "client_connection.h"
#include "csapp.h"
#include "exceptions.h"
#include "guard.h"
#include "table.h"
#include "server.h"

Server::Server()
  : server_tables()
  //, locked_tables()
  // TODO: initialize member variables
{
  // TODO: implement

}

Server::~Server()
{
  // TODO: implement
  close(server_fd);
}

void Server::listen( const std::string &port )
{
  // TODO: implement
  int fd = open_listenfd(port.c_str());
  if (server_fd < 0) {
    throw CommException("Failed to create server socket");
  }
  server_fd = fd;
}

void Server::server_loop()
{
  // TODO: implement
  bool connected = true;
  while (connected) {
    int client_fd = Accept(server_fd, NULL, NULL);
    if (client_fd > 0) {
      ClientConnection *client = new ClientConnection( this, client_fd );
      pthread_t thr_id;
      if ( pthread_create( &thr_id, nullptr, client_worker, client ) != 0 ) {
        log_error( "Could not create client thread" );
        connected = false;
      }
    } else {
      log_error("Failed to accept client connection");
      connected = false;
    }
  }
  // Note that your code to start a worker thread for a newly-connected
  // client might look something like this:
/*
  ClientConnection *client = new ClientConnection( this, client_fd );
  pthread_t thr_id;
  if ( pthread_create( &thr_id, nullptr, client_worker, client ) != 0 )
    log_error( "Could not create client thread" );
*/
}


void *Server::client_worker( void *arg )
{
  // TODO: implement
  //try {
    pthread_detach(pthread_self());
    std::unique_ptr<ClientConnection> client( static_cast<ClientConnection *>( arg ) );
    client->chat_with_client();
  // } catch (const std::exception &e) {
  //   return nullptr;
  // }
  return nullptr;
  // Assuming that your ClientConnection class has a member function
  // called chat_with_client(), your implementation might look something
  // like this:
/*
  std::unique_ptr<ClientConnection> client( static_cast<ClientConnection *>( arg ) );
  client->chat_with_client();
  return nullptr;
*/
}

void Server::log_error( const std::string &what )
{
  std::cerr << "Error: " << what << "\n";
}

// TODO: implement member functions
void Server::create_table( const std::string &name ) {
  Table* new_table = find_table(name);
  if (new_table == NULL) {
    new_table = new Table(name);
    new_table->lock();
    server_tables.push_back(new_table);
    new_table->unlock();
  } else {
    throw OperationException("Table already exists");
  }
}

Table* Server::find_table( const std::string &name ) {
  for (auto it = server_tables.begin(); it != server_tables.end(); it++) {
    if ((*it)->get_name() == name) {
      return *it;
    }
  }
  return NULL;
}

void Server::add_table( Table* to_add ) {
  server_tables.push_back(to_add);
}

// bool Server::is_locked( Table* table_to_lock ) {
//   for (auto it = locked_tables.begin(); it != locked_tables.end(); it++) {
//     Table *table = *it;
//     if (table == table_to_lock) {
//       return true;
//     }
//   }
//   return false;
// }

// void Server::unlock_table( Table* table_to_unlock ) {
//   locked_tables.erase(find(locked_tables.begin(), locked_tables.end(), table_to_unlock));
// }