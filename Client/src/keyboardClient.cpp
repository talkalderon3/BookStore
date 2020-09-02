#include <utility>
#include <vector>
#include <stompFrame.h>
#include <keyboardClient.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace std;
KeyboardClient::KeyboardClient(BooksDataBase &booksDataBase, ConnectionHandler &handler):booksDataBase(booksDataBase), handler(handler){}
void KeyboardClient::run() {
    while (!booksDataBase.getShouldTerminate()) {
        string input;
        getline(cin, input);
        vector<string> separateWords; // this vector will store the words.
        separateWords = boost::split(separateWords, input, boost::is_any_of("\t "));
        string command = separateWords[0];
        string genre;
        if (separateWords.size()>1) {
            genre = separateWords[1];
        }
        if (command == "login")
            cout<<"you're already logged in"<<endl;
        else if (command == "join")// Structure: join {genre}
            join(genre);
        else if (command=="exit")// Structure: exit {genre}
            exit(genre);
        else if (command=="add")// Structure: add {genre} {book name}
            add(genre, getBookName(separateWords));
        else if (command=="borrow")// Structure: borrow {genre} {book name}
            borrow(genre, getBookName(separateWords));
        else if (command=="return")// Structure: return {genre} {book name}
            _return(genre, getBookName(separateWords));
        else if (command=="status")// Structure: status {genre}
            status(genre);
        else if (command=="logout")// Structure: logout
            logout();
    }
}

void KeyboardClient::join(const string& genre) {// subscribe to genre
    if(booksDataBase.getSubscriptionId(genre) == -1){// if the user isn't already subscribed to the genre
        // add to data base
        booksDataBase.addSubscriptionIdGenre(genre, subscriptionId);
        booksDataBase.addReceipt(receiptID, "Joined club " + genre);
        // send SUBSCRIBE frame
        handler.sendFrameAscii(createNewSubscribeFrame(genre, subscriptionId, receiptID).toString(), '\0');
        subscriptionId++; // for unique id
        receiptID++; // for unique id
    }
    else
        cout << "You are already subscribed this genre"<<endl;
}

void KeyboardClient::exit(const string& genre) { // unsubscribe from genre
    if (booksDataBase.getSubscriptionId(genre) != -1) {// if the user is subscribed to the genre
        booksDataBase.addReceipt(receiptID, "Exited club " + genre);
        // send UNSUBSCRIBE framelogin 127.0.0.1:7777 noam pass
        handler.sendFrameAscii(createNewUnSubscribeFrame(booksDataBase.getSubscriptionId(genre), receiptID).toString(), '\0');
        booksDataBase.removeSubscription(genre);
        receiptID++;
    }
}

void KeyboardClient::add(const string& genre, const string& bookName) {// add book to inventory and send a SEND frame
    string body = booksDataBase.getUserName() + " has added the book " + bookName;
    booksDataBase.addBookInventory(genre, bookName);
    handler.sendFrameAscii(createNewSendFrame(genre, body).toString(), '\0');
}

void KeyboardClient::borrow(const string& genre, const string& bookName) {// send a SEND frame
    string body = booksDataBase.getUserName() + " wish to borrow " + bookName;
    handler.sendFrameAscii(createNewSendFrame(genre, body).toString(), '\0');
}

void KeyboardClient::_return(const string& genre, const string& bookName) {// remove book inventory and send a SEND frame
    if (booksDataBase.hasBorrowedBooks(bookName)) {// only if we need to return the book
        string borrowUserName = booksDataBase.removeBorrowedBook(bookName);
        booksDataBase.removeBookInventory(genre, bookName);
        string body = "Returning " + bookName + " to " + borrowUserName;
        handler.sendFrameAscii(createNewSendFrame(genre, body).toString(), '\0');
    }
}

void KeyboardClient::status(const string& genre) {// send a SEND frame with book status body
    handler.sendFrameAscii(createNewSendFrame(genre, "book status").toString(), '\0');
}

void KeyboardClient::logout() {// send a DISCONNECT frame
    booksDataBase.setShouldTerminate(true);
    booksDataBase.addReceipt(receiptID, "DISCONNECT");
    handler.sendFrameAscii(createNewDisconnectFrame(receiptID).toString(), '\0');
    receiptID++;
}

StompFrame KeyboardClient::createNewSubscribeFrame(const string& genre,int _subscriptionId,int _receiptID) {
    map<string,string> headers;
    headers.insert({"destination",genre});
    headers.insert({"id",to_string(_subscriptionId)});
    headers.insert({"receipt",to_string(_receiptID)});
    return StompFrame("SUBSCRIBE",headers,"");
}

StompFrame KeyboardClient::createNewUnSubscribeFrame(int _subscriptionId,int _receiptID) {
    map<string,string> headers;
    headers.insert({"id",to_string(_subscriptionId)});
    headers.insert({"receipt",to_string(_receiptID)});
    return StompFrame("UNSUBSCRIBE",headers,"");
}
StompFrame KeyboardClient::createNewSendFrame (const string& genre,string bodyString) {
    map<string,string> headers;
    headers.insert({"destination",genre});
    return StompFrame("SEND",headers,std::move(bodyString));
}

StompFrame KeyboardClient::createNewDisconnectFrame(int _receiptID) {
    map<string,string> headers;
    headers.insert({"receipt",to_string(_receiptID)});
    return StompFrame("DISCONNECT",headers,"");
}

string KeyboardClient::getBookName(vector<string> separateWords) {
    string bookName;
    for(auto it = separateWords.begin() + 2 ; it!=separateWords.end(); ++it){ //the name of the book run from the third place, always.
        bookName += *it+" ";
    }
    return bookName.substr(0, bookName.size()-1);
}
