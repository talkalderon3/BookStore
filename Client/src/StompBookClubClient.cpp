#include <connectionHandler.h>
#include <thread>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <stompFrame.h>
#include <socketClient.h>
#include <booksDataBase.h>
#include <mutex>
#include <keyboardClient.h>

int main (int argc, char *argv[]) {
    BooksDataBase booksDataBase=BooksDataBase();// initialize the data base
    // get input in a loop until the user types a login command
    string input;
    getline(cin, input);
    vector<string> separateWords;
    boost::split(separateWords, input, boost::is_any_of("\t "));// separate the words in the input and store in the vector
    while(!(separateWords[0] =="login")) {// keep getting input until user enters login command
        cout << "you should send login command first" << endl;
        getline(cin, input); // get input from user
        boost::split(separateWords, input, boost::is_any_of("\t "));
    }

    //handle a login command:
    vector<string> hostPort;
    boost::split(hostPort,separateWords[1],boost::is_any_of(":"));//gets host and port from host:port
    short port = boost::lexical_cast<short>(hostPort[1]);
    ConnectionHandler handler(hostPort[0],port);// initialize the connection handler
    if(!handler.connect()) {
        std::cerr << "Could not connect to server" << std::endl;
        return 1;
    }

    // create connect frame
    map<string,string> headers;
    headers.insert({"accept-version","1.2"});
    headers.insert({"host",hostPort[1]}); // insert host header
    headers.insert({"login",separateWords[2]});
    headers.insert({"passcode",separateWords[3]});
    StompFrame connectFrame = StompFrame("CONNECT",headers,"");

    //send a connect frame to the server
    handler.sendFrameAscii(connectFrame.toString(),'\0');

    //handle a respond from the serve
    string fromServer;
    handler.getFrameAscii(fromServer, '\0');
    StompFrame toFrame=StompFrame::toFrame(fromServer);
    if (toFrame.getCommand()=="CONNECTED") {
        std::cout << "Login successful" << std::endl;
        booksDataBase.setUserName(separateWords[2]);// set username
    }
    else if (toFrame.getCommand()=="ERROR") {// if server responded with an error frame - exit the program
        cout<<(toFrame.getHeaders())["message"]<<endl;//print error message
        handler.close();
        return 1;
    }

    //create a keyboardClient thread and a socketClient thread
    KeyboardClient keyboardClient(booksDataBase, handler);
    thread keyBoardThread(&KeyboardClient::run, keyboardClient); //active thread.
    SocketClient socketClient(booksDataBase, handler);
    thread socketThread(&SocketClient::run,socketClient); // active thread.

    // exit program after threads stop running
    keyBoardThread.join();
    socketThread.join();
    return 0;
}


