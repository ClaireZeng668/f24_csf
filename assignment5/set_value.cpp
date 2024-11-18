//set_value.cpp
#include "csapp.h"
#include "message.h"
#include <iostream>
#include "message_serialization.h"

void handle_error(const std::string &message) {
    std::cerr << "Error: " << message << "\n";
    exit(1);
}

void send_message(int fd, Message &msg) {
    std::string serialized_msg;
    MessageSerialization::encode(msg, serialized_msg);
    if (rio_writen(fd, serialized_msg.data(), serialized_msg.size()) != serialized_msg.size()) {
        handle_error("Failed to send message to server");
    }
}

Message receive_message(int fd, rio_t &rio) {
    char response_buf[1024];
    int bytes_read = rio_readlineb(&rio, response_buf, sizeof(response_buf));
    if (bytes_read <= 0) {
        handle_error("Failed to read message from server");
    }
    std::string response = response_buf;
    Message server_response;
    MessageSerialization::decode(response, server_response);
    return server_response;
}

int main(int argc, char **argv) {
    if (argc != 7) {
        std::cerr << "Usage: ./set_value <hostname> <port> <username> <table> <key> <value>\n";
        return 1;
    }

    std::string hostname = argv[1], port = argv[2], username = argv[3];
    std::string table = argv[4], key = argv[5], value = argv[6];

    int fd = open_clientfd(hostname.c_str(), port.c_str());
    if (fd < 0) handle_error("Couldn't connect to server");

    rio_t rio;
    rio_readinitb(&rio, fd);

    try {
        // LOGIN
        Message login_msg(MessageType::LOGIN, {username});
        send_message(fd, login_msg);
        Message login_response = receive_message(fd, rio);
        if (login_response.get_message_type() != MessageType::OK) {
            handle_error(login_response.get_quoted_text());
        }

        // SET
        Message set_msg(MessageType::SET, {table, key, value});
        send_message(fd, set_msg);
        Message set_response = receive_message(fd, rio);
        if (set_response.get_message_type() != MessageType::OK) {
            handle_error(set_response.get_quoted_text());
        }

        // BYE
        Message bye_msg(MessageType::BYE);
        send_message(fd, bye_msg);
    } catch (const std::exception &e) {
        handle_error(e.what());
    }

    close(fd);
    return 0;
}
