#include <iostream>
#include <string>
#include <stdexcept>

int main(int argc, char **argv) {
  if ( argc != 6 && (argc != 7 || std::string(argv[1]) != "-t") ) {
    std::cerr << "Usage: ./incr_value [-t] <hostname> <port> <username> <table> <key>\n";
    std::cerr << "Options:\n";
    std::cerr << "  -t      execute the increment as a transaction\n";
    return 1;
  }

  int count = 1;

  bool use_transaction = false;
  if ( argc == 7 ) {
    use_transaction = true;
    count = 2;
  }

  std::string hostname = argv[count++];
  std::string port = argv[count++];
  std::string username = argv[count++];
  std::string table = argv[count++];
  std::string key = argv[count++];

  // TODO: implement

  // Establish connection
  int clientfd = open_clientfd(hostname.c_str(), port.c_str());
  if (clientfd < 0) {
      std::cerr << "Error: Unable to connect to server\n";
      return 1;
  }

  try {
      // Login
      Message login_msg("LOGIN", {username});
      send_message(clientfd, login_msg);
      Message response = receive_message(clientfd);
      if (response.get_command() != "OK") {
          throw std::runtime_error("Login failed: " + response.get_argument(0));
      }

      //begin transaction (if -t flag is used)
      if (use_transaction) {
        try {
          Message begin_msg("BEGIN");
          send_message(clientfd, begin_msg);
          response = receive_message(clientfd);
          if (response.get_command() != "OK") {
              throw std::runtime_error("Failed to start transaction: " + response.get_argument(0));
          } catch (const std::exception &e) { 
            Message rollback_msg("ROLLBACK");
            send_message(clientfd, rollback_msg);
            response = receive_message(clientfd);
            if (response.get_command() != "OK") {
                throw std::runtime_error("Failed to rollback transaction: " + response.get_argument(0));
            }
            throw;    
          }
      }

      //retrieve the value
      Message get_msg("GET", {table, key});
      send_message(clientfd, get_msg);
      response = receive_message(clientfd);
      if (response.get_command() != "DATA") {
          throw std::runtime_error("Failed to retrieve value: " + response.get_argument(0));
      }
      std::string current_value = response.get_argument(0);

      //increment the value
      Message push_msg("PUSH", {current_value});
      send_message(clientfd, push_msg);
      response = receive_message(clientfd);
      if (response.get_command() != "OK") {
          throw std::runtime_error("Failed to push current value: " + response.get_argument(0));
      }

      Message push_one_msg("PUSH", {"1"});
      send_message(clientfd, push_one_msg);
      response = receive_message(clientfd);
      if (response.get_command() != "OK") {
          throw std::runtime_error("Failed to push increment value: " + response.get_argument(0));
      }

      Message add_msg("ADD");
      send_message(clientfd, add_msg);
      response = receive_message(clientfd);
      if (response.get_command() != "OK") {
          throw std::runtime_error("Failed to add values: " + response.get_argument(0));
      }

      // update value
      Message set_msg("SET", {table, key});
      send_message(clientfd, set_msg);
      response = receive_message(clientfd);
      if (response.get_command() != "OK") {
          throw std::runtime_error("Failed to set new value: " + response.get_argument(0));
      }

      // commit
      if (use_transaction) {
          Message commit_msg("COMMIT");
          send_message(clientfd, commit_msg);
          response = receive_message(clientfd);
          if (response.get_command() != "OK") {
              throw std::runtime_error("Failed to commit transaction: " + response.get_argument(0));
          }
      }

      // Logout
      Message bye_msg("BYE");
      send_message(clientfd, bye_msg);

  } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << '\n';
      close(clientfd);
      return 1;
  }

  close(clientfd);
  return 0;
}
}
