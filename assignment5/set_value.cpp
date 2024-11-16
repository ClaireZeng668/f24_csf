#include "csapp.h"
#include <iostream>
#include <sstream>
#include <string>

void usage() {
    std::cerr << "Usage: ./set_value <hostname> <port> <username> <table> <key> <value>\n";
}

int main(int argc, char **argv) {
    if (argc != 7) {
        usage();
        return 1;
    }

    std::string hostname = argv[1];
    std::string port = argv[2];
    std::string username = argv[3];
    std::string table = argv[4];
    std::string key = argv[5];
    std::string value = argv[6];

    //cpen connection to server
    int client_fd = open_clientfd(hostname.c_str(), port.c_str());
    if (client_fd < 0) {
        std::cerr << "Error: Could not connect to server at " << hostname << ":" << port << "\n";
        return 1;
    }

    //prepare req string
    std::ostringstream request_stream;
    request_stream << "SET " << username << " " << table << " " << key << " " << value << "\r\n";
    std::string request = request_stream.str();

    //init rio
    rio_t rio;
    rio_readinitb(&rio, client_fd);

    //send request to server
    if (rio_writen(client_fd, request.c_str(), request.size()) != (ssize_t)request.size()) {
        std::cerr << "Error: Failed to send request to server\n";
        close(client_fd);
        return 1;
    }

    //read response from server
    char response[1024];
    ssize_t n = rio_readlineb(&rio, response, sizeof(response));
    if (n < 0) {
        std::cerr << "Error: Failed to read response from server\n";
        close(client_fd);
        return 1;
    }

    //output
    std::cout << "Server response: " << response;

    close(client_fd);

    return 0;
}

//??? still wip
