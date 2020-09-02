
#include <stompFrame.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <booksDataBase.h>
#include "../include/socketClient.h"

using namespace std;
SocketClient::SocketClient(BooksDataBase& booksDataBase, ConnectionHandler& handler): booksDataBase(booksDataBase), handler(handler){}
void SocketClient::run() {// send and receive messages from server
    // stay in loop until receiving a receipt for DISCONNECT command
    while(keepReading){
        //try getting a message from the server
        string fromServer;
        if (!handler.getFrameAscii(fromServer, '\0')) {
            std::cout << "Disconnected. Exiting..." << std::endl;
            keepReading=false;
        }
        // if the message received successfully
        else {
            StompFrame toFrame=StompFrame::toFrame(fromServer);
            if (toFrame.getCommand() == "RECEIPT")
                receipt(toFrame);
            else if (toFrame.getCommand() == "MESSAGE")
                message(toFrame);
        }
    }
}

void SocketClient::message(StompFrame frame) {// if the message has a MESSAGE command
    // check the body of the message in order to handle the message
    string genre = frame.getHeaders().find("destination")->second;
    string body=frame.getBody();
    cout<<genre<<":"<<body<<endl;
    //if the body is "book status"
    if (body=="book status"){//Genre Book Status Command
        GenreBookStatusCommand(genre);
        return;
    }
    std::vector<std::string> split;
    boost::split(split, body, boost::is_any_of("\t "));

    // if the first word of the body is "Taking"
    //format:"Taking {book name} from {book owner username}"
    if (!split.empty()&&split[0]=="Taking")//Borrow Book Command
        takeBook(genre,split);

    // if the second word of the body is "has"
    //format:“{username} has {book name}”
    else if (split.size()>1&&split[1]=="has")//Borrow Book Command
        hasRequestedBook(split, genre);

    // if the fourth word of the body is "borrow"
    //format:“{user} wish to borrow {book name}“
    else if(split.size()>3&&split[3]=="borrow") //Borrow Book Command
        borrow(split, genre);

    // if the first word of the body is "Returning"
    //format:"Returning {book name} to {book lender}"
    else if (!split.empty()&&split[0]=="Returning")//Return Book Command
        _return(genre, split);
}

void SocketClient::receipt(StompFrame frame) {// if the message has a RECEIPT command
    int receiptId=stoi(frame.getHeaders().find("receipt-id")->second);
    string receiptType=booksDataBase.getReceiptType(receiptId);
    // if the receipt is a respond for a DISCONNECT command
    if (receiptType=="DISCONNECT") {
        handler.close();//Once the client receives the RECEIPT frame, it should close the socket and await further user commands
        std::cout << "Disconnected. Exiting..." << std::endl;
        keepReading=false;// exit the run() function
    }
    else// subscribe or unsubscribe receipt
        cout<<receiptType<<endl;// display the message to the screen
}

void SocketClient::GenreBookStatusCommand(const string& genre) {//Genre Book Status Command
    // send a SEND frame with user's book status
    map<string,string> headers;
    headers.insert({"destination",genre});
    string bookStatus= booksDataBase.getUserName() + ":";
    if (booksDataBase.getAllBooks(genre) != nullptr ) {
        for (const string& book:*booksDataBase.getAllBooks(genre)){
            bookStatus+=book+",";
        }
        bookStatus=bookStatus.substr(0,bookStatus.size()-1);
    }
    StompFrame send("SEND",headers,bookStatus);
    handler.sendFrameAscii(send.toString(),'\0');
}

void SocketClient::borrow(vector<std::string>& split, const string& genre) {//format:“{user} wish to borrow {book name}“
    // get bookName from body
    string bookName;
    for (unsigned int i=4;i<split.size();i++) {
        bookName += split[i]+" ";
    }
    bookName=bookName.substr(0,bookName.size()-1);
    // handle message
    if (split[0] == booksDataBase.getUserName()) {//if current user is the user who wish to borrow
        booksDataBase.addBookToBorrow(bookName);// add the book to the list of books the current user wishes to borrow
    }
    else if (booksDataBase.hasBook(genre, bookName)) {//if current user has the book in inventory
        // send a SEND frame with a body saying the current user has the requested book
        map<string,string> headers;
        headers.insert({"destination",genre});
        StompFrame send("SEND", headers, booksDataBase.getUserName() + " has " + bookName);
        handler.sendFrameAscii(send.toString(),'\0');
    }
}
void SocketClient::hasRequestedBook(vector<std::string> &split, const string& genre) {//format:“{username} has {book name}”
    // get bookName from body
    string bookName;
    for (unsigned int i=2;i<split.size();i++) {
        bookName += split[i]+" ";
    }
    bookName=bookName.substr(0,bookName.size()-1);
    // handle message
    if (booksDataBase.getWishToBorrow() & booksDataBase.hasBookToBorrow(bookName)){// if current user has a borrow request for the book in the message
        booksDataBase.addBookInventory(genre, bookName);// add the book to the inventory
        booksDataBase.addBorrowedBook(bookName, split[0]);// add book and lender to the borrowed book list
        booksDataBase.removeBookToBorrow(bookName);// remove the book from the list of books the current user wishes to borrow
        // send a SEND frame with body saying someone is taking the book
        map<string,string> headers;
        headers.insert({"destination",genre});
        StompFrame send("SEND",headers,"Taking "+bookName+" from "+split[0]);
        handler.sendFrameAscii(send.toString(),'\0');
    }
}

void SocketClient::takeBook(const string& genre,vector<std::string> &split) {//format:"Taking {book name} from {book owner username}"
    // get bookName from body
    string bookName;
    for (unsigned int i=1;i<split.size()-2;i++) {
        bookName += split[i]+" ";
    }
    bookName=bookName.substr(0,bookName.size()-1);
    // handle message
    if (split[split.size()-1] == booksDataBase.getUserName()){//if current user is the user that borrowed the book
        booksDataBase.removeBookInventory(genre,bookName);
    }
}

void SocketClient::_return(const string& genre,vector<std::string> &split) {//format:"Returning {book name} to {book lender}"
    // get bookName from body
    string bookName;
    for (unsigned int i=1;i<split.size()-2;i++) {
        bookName += split[i]+" ";
    }
    bookName=bookName.substr(0,bookName.size()-1);
    // handle message
    if (split[split.size()-1]==booksDataBase.getUserName()){// if current user is the user in the message
        booksDataBase.addBookInventory(genre,bookName);
    }
}