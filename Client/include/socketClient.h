#ifndef BOOST_ECHO_CLIENT_SOCKETCLIENT_H
#define BOOST_ECHO_CLIENT_SOCKETCLIENT_H

#include "connectionHandler.h"
#include "stompFrame.h"


class SocketClient {
public:
    SocketClient (BooksDataBase&, ConnectionHandler&);
    void run();
    void receipt(StompFrame);
    void message(StompFrame);
    void GenreBookStatusCommand(const string&);
    void borrow(vector<std::string>&,const string&);
    void hasRequestedBook(vector<std::string>&,const string&);
    void takeBook(const string&,vector<std::string>&);
    void _return(const string& genre,vector<std::string> &split);

private:
    BooksDataBase &booksDataBase;
    ConnectionHandler & handler;
    bool keepReading=true;
};


#endif //BOOST_ECHO_CLIENT_SOCKETCLIENT_H
