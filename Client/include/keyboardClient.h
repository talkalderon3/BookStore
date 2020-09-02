#include <iostream>
#include <boost/algorithm/string.hpp>
#include <mutex>
#include "connectionHandler.h"
#include "booksDataBase.h"
#include "socketClient.h"
#include "stompFrame.h"

#ifndef BOOST_ECHO_CLIENT_KEYBOARDCLIENT_H
#define BOOST_ECHO_CLIENT_KEYBOARDCLIENT_H

using namespace std;

class KeyboardClient {
public:
    KeyboardClient (BooksDataBase&, ConnectionHandler&);
    void run();
    void join(const string&);
    void exit(const string&);
    void add(const string&,const string&);
    void borrow(const string&,const string&);
    void _return(const string&,const string&);
    void status(const string&);
    void logout();
    static string getBookName(vector<string> );
    static StompFrame createNewSubscribeFrame(const string& , int ,int );
    static StompFrame createNewUnSubscribeFrame(int ,int );
    static StompFrame createNewSendFrame(const string& ,string );
    static StompFrame createNewDisconnectFrame(int );
private:
    BooksDataBase &booksDataBase;
    ConnectionHandler &handler;
    int receiptID=0;
    int subscriptionId=0;
};


#endif //BOOST_ECHO_CLIENT_KEYBOARDCLIENT_H