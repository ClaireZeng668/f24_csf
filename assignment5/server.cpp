#include <iostream>
#include <cassert>
#include <memory>
#include "client_connection.h"
#include "csapp.h"
#include "exceptions.h"
#include "guard.h"
#include "table.h"
#include "server.h"

Server::Server()
  : server_tables()
{

}

Server::~Server()
{
  close(server_fd);
}

void Server::listen( const std::string &port )
{
  int fd = open_listenfd(port.c_str());
  if (server_fd < 0) {
    throw CommException("Failed to create server socket");
  }
  server_fd = fd;
}

void Server::server_loop()
{
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
}


void *Server::client_worker( void *arg )
{
  pthread_detach(pthread_self());
  std::unique_ptr<ClientConnection> client( static_cast<ClientConnection *>( arg ) );
  client->chat_with_client();
  return nullptr;
}

void Server::log_error( const std::string &what )
{
  std::cerr << "Error: " << what << "\n";
}


void Server::create_table( const std::string &name ) {
  Table* new_table = find_table(name);
  if (new_table == NULL) {
    new_table = new Table(name);
    server_tables.push_back(new_table);
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
