/**
 * chat.cpp
 * Part of YAChat
 * 
 * Responsible for adding messages
 * TODO:
 * see chat.hpp
*/
#include "chat.hpp"
#include <regex>

using namespace yachat;
using namespace std;

// empty constructor and destructor
chat::chat() noexcept {}
chat::~chat() noexcept {}
bool chat::add_message( string message )
{
    // no newlines allowed, otherwise will confuse with new message
    if(message.find_first_of("\r\n") != string::npos) return false;

    // colour check
    auto colour_regex_s = "^\\(#(([0-9a-fA-F]{2}){3}|([0-9a-fA-F]){3})\\)$"s;
    regex colour_regex(colour_regex_s);
    auto colour_str = message.substr(0,9);
    smatch match;
    // check colour is there
    // the reason why message must be string
    if(!regex_match(colour_str, match, colour_regex)) return false;

    // if message is not empty, send
    if(message.length() > 9) 
        messages_queue.push_back(message);
    // else don't raise a problem
    return true;
}
bool chat::add_message( string_view message )
{
    // convert to string
    string msg(message);
    // run the one above
    return add_message(msg);
}
string chat::printAllMessages(uint32_t first_message, const char* separator = "\n")
{
    string output = "";
    for(int i = first_message; i < messages_queue.size(); i++)
    {
        try
        {
            // try if there is an entity
            // if no just continue
            string at = "["s + to_string(i) + "]"s + messages_queue.at(i);
            output += at + separator;
        }
        catch(out_of_range& e)
        {
            // continue;
        }
    }
    return output;
}