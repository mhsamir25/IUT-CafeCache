#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>
#include <ctime>

using namespace std;

struct OrderItem {
    string itemName;
    int quantity;
    double price;
    double totalPrice;
};

class Token {
private:
    string tokenId;
    string userId;
    vector<OrderItem> items;
    double totalAmount;
    time_t timestamp;
    string status; 
    
public:
    Token();
    Token(string tId, string uid);
    
    void addItem(string itemName, int quantity, double price);
    string getTokenId() const;
    string getUserId() const;
    double getTotalAmount() const;
    time_t getTimestamp() const;
    string getStatus() const;
    
    void setStatus(string newStatus);
    void displayToken() const;
    string toFileString() const;
    static Token fromFileString(const string& line);
};

#endif
