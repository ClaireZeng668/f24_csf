#include <iostream>
#include "server.h"

int main(int argc, char **argv)
{
  if ( argc != 2 ) {
    std::cerr << "Usage: ./server <port>\n";
    return 1;
  }

  Server server;

  try {
    server.listen( argv[1] );
    server.server_loop();
  } catch ( std::runtime_error &ex ) {
    server.log_error( "Fatal error starting server" );
    return 1;
  }

  return 0;
}
