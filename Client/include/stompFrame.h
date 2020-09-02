#ifndef BOOST_ECHO_CLIENT_STOMPFRAME_H
#define BOOST_ECHO_CLIENT_STOMPFRAME_H

#include <map>
#include <string>
#include "booksDataBase.h"
#include "connectionHandler.h"

class StompFrame {
public:
    StompFrame(std::string, std::map<std::string, std::string>, std::string);
    std::string getCommand ();
    std::map<std::string, std::string> getHeaders();
    std::string getBody ();
    std::string toString();
    static StompFrame toFrame(std::string);
    pthread_mutex_t mutex=pthread_mutex_t();

private:
    std::string command;
    std::map<std::string, std::string> headers;
    std::string body;
};


#endif //BOOST_ECHO_CLIENT_STOMPFRAME_H
