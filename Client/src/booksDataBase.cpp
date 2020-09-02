#include <iostream>
#include <utility>
#include "booksDataBase.h"

BooksDataBase::BooksDataBase():
lock_inventory(pthread_mutex_t()),// a locker for adding and removing from the inventory
lock_subscription(pthread_mutex_t()),// a locker for adding and removing from the subscriptionIdGenre
lock_borrowedBooks(pthread_mutex_t()),// a locker for adding and removing from the borrowedBooks
userName(""),// the userName of the user
subscriptionIdGenre(map <string,int>()),// holds the genre and it's subscription id
bookInventory(map <string,std::list<string>>()),// holds the genre and the books that added to this genre
receipts(map <int,string>()),// holds the receipt id's and it's type(DISCONNECT,SUBSCRIBE, UNSUBSCRIBE)
borrowedBooks(map <string,string>()),// holds the books that the user borrowed and their lender's userName
bookToBorrow(vector <string>()),// a list of books that the user wishes to borrow
shouldTerminate(false) {}

void BooksDataBase:: addSubscriptionIdGenre(const string& genre, int id) {// add a new subscription
    pthread_mutex_lock(&lock_subscription);
    if(subscriptionIdGenre.find(genre) == subscriptionIdGenre.end())  // checks if the genre exists
        subscriptionIdGenre.insert({genre,id});
    pthread_mutex_unlock (&lock_subscription);
}

int BooksDataBase::getSubscriptionId(const string& genre) {// return the subscription id of the genre or -1 if the user isn't subscribed to the genre
    pthread_mutex_lock(&lock_subscription);
    if(subscriptionIdGenre.find(genre) == subscriptionIdGenre.end()) {
        pthread_mutex_unlock (&lock_subscription);
        return -1;
    }
    pthread_mutex_unlock (&lock_subscription);
    return subscriptionIdGenre.at(genre);
}

void BooksDataBase:: removeSubscription(const string& genre) {// remove the subscription to the genre
    pthread_mutex_lock(&lock_subscription);
    subscriptionIdGenre.erase(genre);
    pthread_mutex_unlock (&lock_subscription);
}

void BooksDataBase::addBookInventory(const string& genre, const string& bookName) {// add a book to the inventory
    pthread_mutex_lock(&lock_inventory);
    if(bookInventory.find(genre)==bookInventory.end()) { // new genre
        list<string> temp = {bookName};
        bookInventory.insert({genre,temp});
    }
    else
        bookInventory[genre].push_back(bookName); // adding book to exists genre.
    pthread_mutex_unlock (&lock_inventory);
}


void BooksDataBase:: removeBookInventory(const string& genre,const string& bookName) {// remove a book from the inventory
    pthread_mutex_lock(&lock_inventory);
    bookInventory[genre].remove(bookName);//  there is no duplicate book names .
    pthread_mutex_unlock (&lock_inventory);
}

bool BooksDataBase::hasBook(const string& genre, const string& bookName) {// check if the book exists in the inventory
    pthread_mutex_lock(&lock_inventory);
    if (bookInventory.find(genre) == bookInventory.end()) {
        pthread_mutex_unlock (&lock_inventory);
        return false;
    }
    for (auto & book : bookInventory.find(genre)->second) {
        if (book==bookName) {
            pthread_mutex_unlock (&lock_inventory);
            return true;
        }
    }
    pthread_mutex_unlock (&lock_inventory);
    return false;
}

list<string> *BooksDataBase::getAllBooks(const string& genre) {// return all the books under that genre in the inventory
    if (bookInventory.find(genre)==bookInventory.end())
        return nullptr;
    return &bookInventory.find(genre)->second;
}

void BooksDataBase::setUserName(string _userName) {// set the username
    userName=std::move(_userName);
}

string BooksDataBase::getUserName() {// return the username
    return userName;
}

void BooksDataBase::addReceipt(int receipt, string command) {// add a new receipt
    receipts[receipt]=std::move(command);
}

string BooksDataBase::getReceiptType(int receiptID) {// get the receipt's type
    return receipts.find(receiptID)->second;
}

bool BooksDataBase::hasBorrowedBooks(const string &bookName) {// check if the user borrowed that book
    if (borrowedBooks.find(bookName) ==borrowedBooks.end() )
        return false;
    return true;
}

void BooksDataBase::addBorrowedBook(const string& bookName, const string& UserName) {// add the borrowed book and it's lender's username
    pthread_mutex_lock(&lock_borrowedBooks);
    borrowedBooks.insert({bookName,UserName});
    pthread_mutex_unlock (&lock_borrowedBooks);
}

string BooksDataBase::removeBorrowedBook(const string& bookName) {// remove the borrowed book and return the lender's username
    pthread_mutex_lock(&lock_borrowedBooks);
    string borrowUserName = borrowedBooks[bookName];
    borrowedBooks.erase(bookName);
    pthread_mutex_unlock (&lock_borrowedBooks);
    return borrowUserName;
}

void BooksDataBase::addBookToBorrow(const string& bookName) {// add the book to the list of books the user wishes to borrow

    if (!hasBookToBorrow(bookName))
        bookToBorrow.push_back(bookName);


}

void BooksDataBase::removeBookToBorrow(const string& bookName) {// remove the book from the list of books the user wishes to borrow
    for (auto book=bookToBorrow.begin();book!=bookToBorrow.end();book++){
        if (*book==bookName) {
            bookToBorrow.erase(book);
            return;
        }
    }

}

bool BooksDataBase::hasBookToBorrow(const string& bookName) {// check if the user wishes to borrow the book

    for (const string& book:bookToBorrow){
        if (book==bookName) {

            return true;
        }
    }

    return false;
}

bool BooksDataBase::getWishToBorrow() {// check if the user wishes to borrow any book
    return !bookToBorrow.empty();
}

bool BooksDataBase::getShouldTerminate() {
    return shouldTerminate;
}

void BooksDataBase::setShouldTerminate(bool _shouldTerminate) {
    shouldTerminate=_shouldTerminate;
}