//incr_value.cpp
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
    if (argc != 6 && argc != 7) {
        std::cerr << "Usage: ./incr_value [-t] <hostname> <port> <username> <table> <key>\n";
        return 1;
    }

    bool use_transaction = (argc == 7);
    std::string hostname = argv[use_transaction ? 2 : 1];
    std::string port = argv[use_transaction ? 3 : 2];
    std::string username = argv[use_transaction ? 4 : 3];
    std::string table = argv[use_transaction ? 5 : 4];
    std::string key = argv[use_transaction ? 6 : 5];

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

        if (use_transaction) {
            Message begin_msg(MessageType::BEGIN);
            send_message(fd, begin_msg);
            Message begin_response = receive_message(fd, rio);
            if (begin_response.get_message_type() != MessageType::OK) {
                handle_error(begin_response.get_quoted_text());
            }
        }

        // GET
        Message get_msg(MessageType::GET, {table, key});
        send_message(fd, get_msg);
        Message get_response = receive_message(fd, rio);
        if (get_response.get_message_type() != MessageType::DATA) {
            handle_error(get_response.get_quoted_text());
        }
        int current_value = std::stoi(get_response.get_value());

        // Increment
        Message push_msg(MessageType::PUSH, {std::to_string(current_value)});
        send_message(fd, push_msg);
        Message push_response = receive_message(fd, rio);
        if (push_response.get_message_type() != MessageType::OK) {
            handle_error(push_response.get_quoted_text());
        }

        Message add_msg(MessageType::ADD, {"1"});
        send_message(fd, add_msg);
        Message add_response = receive_message(fd, rio);
        if (add_response.get_message_type() != MessageType::OK) {
            handle_error(add_response.get_quoted_text());
        }

        // SET
        Message set_msg(MessageType::SET, {table, key, std::to_string(current_value + 1)});
        send_message(fd, set_msg);
        Message set_response = receive_message(fd, rio);
        if (set_response.get_message_type() != MessageType::OK) {
            handle_error(set_response.get_quoted_text());
        }

        if (use_transaction) {
            Message commit_msg(MessageType::COMMIT);
            send_message(fd, commit_msg);
            Message commit_response = receive_message(fd, rio);
            if (commit_response.get_message_type() != MessageType::OK) {
                handle_error(commit_response.get_quoted_text());
            }
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