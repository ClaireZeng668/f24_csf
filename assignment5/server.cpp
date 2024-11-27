#include <iostream>
#include <cassert>
#include <memory>
#include "client_connection.h"
#include "csapp.h"
#include "exceptions.h"
#include "guard.h"
#include "server.h"

Server::Server()
  : server_tables()
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
    log_error("Failed to create server socket");
  }
  server_fd = fd;
}

void Server::server_loop()
{
  // TODO: implement
  while (1) {
    int client_fd = Accept(server_fd, NULL, NULL);
    if (client_fd > 0) {
      ClientConnection *client = new ClientConnection( this, client_fd );
      pthread_t thr_id;
      if ( pthread_create( &thr_id, nullptr, client_worker, client ) != 0 ) {
        log_error( "Could not create client thread" );
      }
    } else {
      log_error("Failed to accept client connection");
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
  try {
    pthread_detach(pthread_self());
    std::unique_ptr<ClientConnection> client( static_cast<ClientConnection *>( arg ) );
    client->chat_with_client();
  } catch (const std::exception &e) {
    //TODO: why is it not letting me call log error???
    //log_error(e.what());
    return nullptr;
  }
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
  Table* new_table = new Table(name);
  server_tables.push_back(new_table);
}

Table* Server::find_table( const std::string &name ) {
  for (auto it = server_tables.begin(); it != server_tables.end(); it++) {
    Table *current = *it;
    //std::string current = it->get_name();
    if (current->get_name() == name) {
      return *it;
    }
  }
  return NULL;
}