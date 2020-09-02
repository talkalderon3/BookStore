
#ifndef BOOST_ECHO_CLIENT_DATABASE_H
#define BOOST_ECHO_CLIENT_DATABASE_H

#include <unordered_map>
#include <list>
#include <vector>
#include <map>

using namespace std;

class BooksDataBase {
private:
    pthread_mutex_t lock_inventory;
    pthread_mutex_t lock_subscription;
    pthread_mutex_t lock_borrowedBooks;
    string userName;
    map <string,int> subscriptionIdGenre;// to match a subscriptionId to genre.
    map <string,std::list<string>> bookInventory;// string genre, list - book names
    map <int,string>  receipts; // this map is for that what to do when we get receipt from the server.
    map <string,string> borrowedBooks; // <bookName,username> //new -
    vector <string> bookToBorrow;//  books that user wish to borrow
    bool shouldTerminate;
public:
    BooksDataBase();
    void addSubscriptionIdGenre(const string&, int);
    int getSubscriptionId(const string& genre);
    void removeSubscription(const string& genre);
    bool hasBorrowedBooks(const string &bookName);
    void addBookInventory(const string& genre,const string& bookName);
    string getUserName();
    void setUserName(string);
    void addReceipt(int receipt, string command);
    bool hasBook(const string& genre, const string& bookName);
    list<string> *getAllBooks(const string& genre);
    void addBorrowedBook(const string& bookName,const string& UserName);//new
    string removeBorrowedBook(const string& bookName); //remove and return the borrow username. //new
    void removeBookInventory(const string& genre,const string& bookName); // new
    string getReceiptType (int);
    bool getWishToBorrow();
    void addBookToBorrow(const string&);
    void removeBookToBorrow(const string&);
    bool hasBookToBorrow(const string&);
    bool getShouldTerminate();
    void setShouldTerminate(bool);

};


#endif //BOOST_ECHO_CLIENT_DATABASE_H
