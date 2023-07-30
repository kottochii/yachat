/**
 * chat.hpp
 * Part of YAChat
 * 
 * Defines class chat
 * TODO:
 * delete old messages
 * deletion of messages
 */
#pragma once 

#include <deque>
#include <string>

namespace yachat
{
    class chat
    {
        public:
        typedef std::string message;
        /**
         * Empty constructor and destructor
        */
        chat() noexcept;
        ~chat() noexcept;
        /**
         * Adds message in format (#HTMLRepresentationOfColour)User: Message
         * Returns true if successfully added message
         * Will only fail if message is not formatted correctly
         */
        bool add_message(std::string str);
        /**
         * Ditto for string_view as parameter
        */
        bool add_message(std::string_view str);
        /**
         * Returns all messages starting from first_message
         * Separates lines with separator
        */
        std::string printAllMessages(uint32_t first_message, const char* separator);
        private:
        /**
         * Stores all messages as deque
        */
        std::deque<message> messages_queue;
    };
}