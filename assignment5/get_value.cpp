#include "csapp.h"
#include <iostream>

int main(int argc, char **argv)
{
  if ( argc != 6 ) {
    std::cerr << "Usage: ./get_value <hostname> <port> <username> <table> <key>\n";
    return 1;
  }

  std::string hostname = argv[1];
  std::string port = argv[2];
  std::string username = argv[3];
  std::string table = argv[4];
  std::string key = argv[5];

  // TODO: implement
  int fd = open_clientfd(argv[1], argv[2]);
  if (fd < 0) {
    std::cerr << "Error: Couldn't connect to server";
    return 1;
  }
  // TODO: read??? idk whas going on
}
