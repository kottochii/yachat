/**
 * request_handler.cpp
 * Part of a bigger YAChat
 * Handles whatever requests comes into host from its sockfd
 * 
 * Only holds handle_request, for other symbols in yachat::host, see host.cpp
 * TODO:
 * put responses in a struct and return them as struct
*/

#include "host.hpp"
#include <fstream>
#include <regex>
#include <iostream>

// defining standard responses
#define BAD_REQUEST " 400 Bad Request"s + newline_separator + "Content-Type: text/plain;charset=utf-8"s + newline_separator + "Connection: keep-alive"s + newline_separator + "Date: "s + newline_separator + "Server: yachat/0.1 (C++ version)"s + newline_separator + "Cache-Control: max-age=100"s + newline_separator + "Content-Length: 76"s + newline_separator + ""s + newline_separator + "Bad request! You should only use the main page located at / of this website!"
#define NOT_FOUND   " 404 Not Found"s + newline_separator + "Content-Type: text/plain;charset=utf-8"s + newline_separator + "Connection: keep-alive"s + newline_separator + "Date: "s + newline_separator + "Server: yachat/0.1 (C++ version)"s + newline_separator + "Cache-Control: max-age=100"s + newline_separator + "Content-Length: 79"s + newline_separator + ""s + newline_separator + "Page not found! You should only use the main page located at / of this website!"
#define INT_SERV_ER " 500 Internal Server Error"s + newline_separator + "Content-Type: text/plain;charset=utf-8"s + newline_separator + "Connection: keep-alive"s + newline_separator + "Date: "s + newline_separator + "Server: yachat/0.1 (C++ version)"s + newline_separator + "Cache-Control: max-age=100"s + newline_separator + "Content-Length: 97"s + newline_separator + ""s + newline_separator + "Internal Server Error! :("s + newline_separator + "Unfortunately, the server cannot proccess the request. Try again later."

using namespace yachat;
using namespace std;

/**
 * Extremely simple handler
 * Can only handle GET /
 * and POST /send, /get and /cmd
*/
string chat_server::handle_request(string_view request)
{
    // finding which separator 
    string newline_separator = "\n"; 
    // check for \r\n and \n\r
    if(request.find_first_of('\r') != string::npos) {
        size_t first_nr = 0;
        if((first_nr = request.find("\n")) != string::npos)
        {
            // if this is either \r\n or \n\r
            // if \r\n
            if(request[first_nr - 1] == '\r') newline_separator = "\r\n";
            // otherwise \n\r
            // also client might use mix, or not legitimate
            // still recognise as \n\r this just in case
            else newline_separator = "\n\r";
        }
        // neither, then just \r
        // probably the client is not legitimate but still use this
        else newline_separator = "\r";
    }
    // regex to read the first line of the request
    auto regex_s = "([A-Z\\s]+)\\s+([^?\\s]+)((?:[?&][^&\\s]+)*)\\s+(HTTP/.*)"s;
    regex first_line_regex(regex_s);
    // separate the first line into string first_line
    auto first_line_pos = request.find_first_of(newline_separator);
    // separate first line
    string first_line(request.substr(0,first_line_pos));
    smatch first_line_match;
    string method, url, query, http_protocol;
    // match regex
    if(regex_match(first_line, first_line_match, first_line_regex)) 
    {
        // if in, try setting things into the respective variables
        method = first_line_match[1].str();
        url = first_line_match[2].str();
        query = first_line_match[3].str();
        http_protocol = first_line_match[4].str();
    }
    // if no match, just give BAD_REQUEST with HTTP/1.1

    else return "HTTP/1.1"s + BAD_REQUEST;
    if(method == "GET")
    {
        if(url == "/")
        {

            const char* home = getenv("HOME");
            string index_html_filename = (string)home + "/.yachat-kottochii/html/index.html";
            fstream index_html(index_html_filename);
            if(!index_html.is_open()){
                cout << "Main page not openable at " << index_html_filename << endl;
                return http_protocol + INT_SERV_ER;
            }
            // load main page only
            string index_html_str = "";
            getline(index_html,index_html_str,'\0');
            string response = http_protocol + " 200 OK"s + newline_separator + "Content-Type: text/html;charset=utf-8"s + newline_separator + "Connection: keep-alive"s + newline_separator + "Date: "s + newline_separator + "Server: yachat/0.1 (C++ version)" + newline_separator + "Cache-Control: max-age=100" + newline_separator + "Content-Length: "s + to_string(index_html_str.length()) + newline_separator + newline_separator;
            response += index_html_str;
            return response;
        }
        else return http_protocol + NOT_FOUND;
    }
    else if(method == "POST")
    {
        // skip headers, get request text
        auto content_start_pos = request.find(newline_separator + newline_separator);
        // fail if no text
        if(content_start_pos == string_view::npos) return http_protocol + BAD_REQUEST;
        // get content into string_view
        // need string instead?
        string_view content = request.substr(content_start_pos + newline_separator.length() * 2);
        // sending message
        if(url == "/send")
        {
            // check if starts with "SEND:"
            if(content.starts_with("SEND:"))
                content.remove_prefix(5);
            // if no, then 400
            else return http_protocol + BAD_REQUEST;

            // try adding message
            bool success = _chat.add_message(content);

            // respond 200 if success
            string text_response = "Sent";
            if(success) return http_protocol + " 200 OK" + newline_separator + "Content-Type: text/plain;charset=utf-8"s + newline_separator + "Connection: keep-alive"s + newline_separator + "Date: "s + newline_separator + "Server: yachat/0.1 (C++ version)" + newline_separator + "Cache-Control: max-age=100" + newline_separator + "Content-Length: "s + to_string(text_response.length()) + newline_separator + newline_separator + text_response;
            // otherwise 400
            else return http_protocol + BAD_REQUEST;

        }
        // retrieving messages
        else if (url == "/get")
        {
            // only if request starts with LAST:
            if(content.starts_with("LAST:"))
                content.remove_prefix(5);
            // otherwise 400
            else return http_protocol + " " + BAD_REQUEST;
            // by default, retrieve all messages starting from 0
            uint16_t first_message_id = 0;
            if(content.length() != 0)
            { 
                // found anything except digits (last message id)
                if(content.find_first_not_of("1234567890") != string::npos)
                    return http_protocol + " " + BAD_REQUEST;
                // convert the digits into string
                string cnt_str(content);
                // then to int
                first_message_id = stoi(cnt_str) + 1;
            }

            // get all messages in a string
            auto response = _chat.printAllMessages(first_message_id, newline_separator.c_str());

            // return proper 200
            return http_protocol + " 200 OK" + newline_separator + "Content-Type: text/plain;charset=utf-8"s + newline_separator + "Connection: keep-alive"s + newline_separator + "Date: "s + newline_separator + "Server: yachat/0.1 (C++ version)" + newline_separator + "Cache-Control: max-age=100" + newline_separator + "Content-Length: "s + to_string(response.length()) + newline_separator + newline_separator + response;
        }
        // sending a command
        else if (url == "/cmd")
        {
            // check if the request starts with USER:
            if(content.starts_with("USER:"))
                content.remove_prefix(5);
            // if not, fail
            else return http_protocol + " " + BAD_REQUEST;

            // look for \n only because content separator in the request body will always be \n
            auto newline_pos = content.find("\n");
            // if no \n, return 400 because there is no command
            if(newline_pos == string_view::npos)
                return http_protocol + " " + BAD_REQUEST;
            // retrieve the command
            auto username = content.substr(0,newline_pos);
            std::string command (content.substr(newline_pos + 1));
            // by default always 200 with empty response
            auto response = "CMD not found (:)"s;
            /**
             * TODO:
             * make this more suitable for modification 
             * like run check for a list of known commands
            */
            if((command == "/quit" || command.starts_with("/quit "))) {
                if(!quit_requested) {
                    // show message and who requested the quit
                    string quit_msg = "(#000000)The server has been scheduled to quit by "s + (string)username + " in 10 seconds."s;
                    _chat.add_message(quit_msg);
                    // run async
                    handled_connections.push_back(async(launch::async,[this]{
                        // make sure that nothing else requests quit
                        quit_requested = true;
                        // wait for 10 seconds
                        sleep(10);
                        // start quit procedures
                        requestQuit();
                    }));
                }
                // change response
                response = "OK";
            }
            return http_protocol + " 200 OK" + newline_separator + "Content-Type: text/plain;charset=utf-8"s + newline_separator + "Connection: keep-alive"s + newline_separator + "Date: "s + newline_separator + "Server: yachat/0.1 (C++ version)" + newline_separator + "Cache-Control: max-age=100" + newline_separator + "Content-Length: "s + to_string(response.length()) + newline_separator + newline_separator + response;
        }
        // not recognised url
        else return http_protocol + NOT_FOUND;
    }
    // method not recognised, BAD_REQUEST
    // 405 not applicable because the server may not know the method
    // and we are not writing nginx here
    else return http_protocol + BAD_REQUEST;

}