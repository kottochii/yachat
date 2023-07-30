/**
 * server.cpp
 * Contains main
 */
#include "host.hpp"
#include <iostream>

#define PORT    8382

int main()
{
    std::cout   << "If you encounter errors, report to GitHub unless: " << std::endl
                << "1. Your hardware/software does not support IPv6 and the error is rising from that" << std::endl
                << "2. Bind error 98 - that means port " << PORT << " is busy." << std::endl
                << "\tChange constant PORT to something free in server/server.cpp" << std::endl << std::endl;
    // define parameters for socket address
    struct sockaddr_in6 saddr;
    saddr.sin6_addr = in6addr_any;
    saddr.sin6_family = AF_INET6;
    saddr.sin6_port = htons(PORT);
    // construct chat_server and listen
    yachat::chat_server srv(saddr);
    // start accepting
    // this does not keep the thread active!!!
    srv.start_accepting();
    // keep main active until chat server is done
    while(srv.isActive())
    {
        // do nothing, wait for the program to finish
        sleep(1);
    }

}