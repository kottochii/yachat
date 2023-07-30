/**
 * host.cpp
 * Part of YAChat
 * Host class
 * TODO:
 * make responds struct in handle_request(...), see also TODO in the head of request_handler.cpp
*/

#include "host.hpp"
#include <iostream>
#include <string>
#include <arpa/inet.h>

using namespace std;
using namespace yachat;

chat_server::chat_server(struct sockaddr_in6& in) : sockfd(socket(AF_INET6,SOCK_STREAM, 0)),
                                                    port(ntohs(in.sin6_port)),
                                                    quit_requested(false),
                                                    done(false),
                                                    _chat()
{
    if(sockfd < 0) throw runtime_error("Socket failed: error "s + to_string(errno));
    auto bnd = bind(sockfd,(const sockaddr*) &in,sizeof(in));
    if(bnd != 0) throw runtime_error("Bind failed: error "s + to_string(errno));
    auto lstng = listen(sockfd,25);
    if(lstng != 0) throw runtime_error("Listen failed: error "s + to_string(errno));

    cout << "Opened server at http://localhost:" << port << "/" << endl
         << "To shut down, press \"request quit\" on the webpage." << endl;


}

chat_server::~chat_server()
{
    cout << "Closed server successfully.";
    if(sockfd > 0) close(sockfd);
}

bool chat_server::isActive()
{
    return !done;
}

void chat_server::start_accepting()
{
    accepter = thread([this]{
        sockaddr_in6 client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int clientfd = 0;
        while((!quit_requested)&&(clientfd = accept(sockfd,(sockaddr* )&client_addr,&client_addr_len)) > 0)
        {
            if(quit_requested) break;
            handled_connections.push_back( 
                async(launch::async,
                    [client_addr,client_addr_len,clientfd, this]
                    {
                        
                        char client_ip[INET6_ADDRSTRLEN] = "";
                        inet_ntop( AF_INET6, &(client_addr.sin6_addr), client_ip, INET6_ADDRSTRLEN );

                        char raw_request[1024] = "";
                        read(clientfd, &raw_request, sizeof(raw_request));

                        string_view request_sv(raw_request);
                        auto response = handle_request(request_sv);

                        /*
                        auto t = time(NULL);
                        char t_str[100] = "";
                        auto t_stct = gmtime(&t);
                        strftime(t_str,sizeof(t_str),"%s",t_stct);
                        auto response = "HTTP/1.1 404 Not Found\nContent-Type: text/plain;charset=utf-8\nConnection: keep-alive\nDate: "s +
                                t_str + "\nServer: yachat/0.3 (Linux)\nCache-Control: max-age=100\n\nNo pages exist in this website :(("s;
                        */
                        write(clientfd,response.c_str(),response.length());
                        close(clientfd);
                    }
                )
            );
        }
    });
}

void chat_server::requestQuit()
{
    quit_requested = true;
    // additional operations to shut the server
    // don't need to clean deque because it will wait until the proccesses will be executed
    // detach the accepter ASAP
    // it is safe because if client is in async inside accepter, it does not kill it
    // if not, client may just experience timeout
    accepter.detach();
    done = true;
}