/**
 * Part of yachat
 * 
 * Host library
 * Hosts the chat server at given port
 * Responsible for handling HTTP request
*/
#pragma once

#include <unistd.h>
#include <cstdint>
#include <sys/socket.h>
#include <netinet/in.h>
#include <future>
#include <deque>
#include "chat.hpp"


namespace yachat 
{
    class chat_server
    {
        private:
        uint16_t port;
        int sockfd = 0;
        bool quit_requested = false;
        bool done = false;
        std::thread accepter;
        std::deque<std::future<void>> handled_connections;
        chat _chat;


        public:
        /**
         * Constructs basic server over ipv6
         * Should also be available at ipv4
         * Requires reference to sockaddr_in6
         * Starts accepting the requests
        */
        chat_server(struct sockaddr_in6& in);
        /**
         * Stops accepting
         * Closes the connection
        */
        ~chat_server();
        /**
         * returns reversed done
        */
        bool isActive();
        /**
         * sets quit_requested to false
        */
        void requestQuit();
        void start_accepting();

        /**
         * Handles request
         * Returns the raw response
        */
        std::string handle_request(std::string_view in);
    };
}