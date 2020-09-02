#include <utility>
#include <vector>
#include "../include/stompFrame.h"
#include <boost/algorithm/string.hpp>
#include <booksDataBase.h>


StompFrame::StompFrame(std::string command,std::map<std::string,std::string> headers,std::string body) : command(std::move(command)), headers(std::move(headers)), body(std::move(body)){}
std::string StompFrame::getCommand() {
    return command;
}
std::map<std::string, std::string> StompFrame::getHeaders() {
    return headers;
}
std::string StompFrame::getBody() {
    return body;
}
std::string StompFrame::toString() {
    pthread_mutex_lock(&mutex);
    std::string toString=command+"\n";
    for (const auto& header: headers) {
        toString += header.first + ":" + header.second + "\n";
    }
    pthread_mutex_unlock (&mutex);
    toString+="\n"+body+"\0";
    return toString;
}

StompFrame StompFrame::toFrame(std::string frame) {
    // separate the words in the string
    std::vector<std::string> split;
    boost::split(split, frame, boost::is_any_of("\n"));
    // get the command from the vector
    std::string command=split[0];
    // create the headers
    std::map<std::string,std::string> headers;
    unsigned int i=1;
    while (i<split.size()&&!split[i].empty()){
        std::vector<std::string> splitHeaders;
        boost::split(splitHeaders, split[i], boost::is_any_of(":"));
        headers.insert({splitHeaders[0],splitHeaders[1]});
        i++;
    }
    i++;// there is one blank line between headers and body
    // get the body from the vector
    std::string _body;
    while (i<split.size()) {// in case body contains '\n'
        _body += split[i];
        i++;
    }
//    if (!_body.empty()) {
//        std::vector<std::string> splitBody;
//        boost::split(splitBody, _body, boost::is_any_of("\0"));
//        _body=splitBody[0];
//    }
    return StompFrame(command, headers, _body) ;
}
