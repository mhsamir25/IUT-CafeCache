#include "Token.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

Token::Token() : tokenId(""), userId(""), totalAmount(0.0), timestamp(time(nullptr)), status("ACTIVE") {}

Token::Token(string tId, string uid) 
    : tokenId(tId), userId(uid), totalAmount(0.0), timestamp(time(nullptr)), status("ACTIVE") {}

void Token::addItem(string itemName, int quantity, double price) {
    OrderItem item;
    item.itemName = itemName;
    item.quantity = quantity;
    item.price = price;
    item.totalPrice = quantity * price;
    
    items.push_back(item);
    totalAmount += item.totalPrice;
}

string Token::getTokenId() const { return tokenId; }
string Token::getUserId() const { return userId; }
double Token::getTotalAmount() const { return totalAmount; }
time_t Token::getTimestamp() const { return timestamp; }
string Token::getStatus() const { return status; }

void Token::setStatus(string newStatus) { status = newStatus; }

void Token::displayToken() const {
    cout << "\n╔════════════════════════════════════════╗" << endl;
    cout << "║         IUT CAFECACHE TOKEN           ║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
    cout << "\nToken ID: " << tokenId << endl;
    cout << "User ID: " << userId << endl;
    time_t t = timestamp;
    cout << "Time: " << ctime(&t);
    cout << "\n----------------------------------------" << endl;
    cout << left << setw(20) << "ITEM" << setw(8) << "QTY" 
         << setw(10) << "PRICE" << "TOTAL" << endl;
    cout << "----------------------------------------" << endl;
    
    for (const auto& item : items) {
        cout << left << setw(20) << item.itemName 
             << setw(8) << item.quantity
             << "BDT " << setw(6) << fixed << setprecision(2) << item.price
             << "BDT " << item.totalPrice << endl;
    }
    
    cout << "----------------------------------------" << endl;
    cout << "TOTAL AMOUNT: BDT " << fixed << setprecision(2) << totalAmount << endl;
    cout << "STATUS: " << status << endl;
    cout << "========================================\n" << endl;
}

string Token::toFileString() const {
    stringstream ss;
    ss << tokenId << "|" << userId << "|" << totalAmount << "|" 
       << timestamp << "|" << status << "|";
    
    for (size_t i = 0; i < items.size(); i++) {
        ss << items[i].itemName << "," << items[i].quantity << "," << items[i].price;
        if (i < items.size() - 1) ss << ";";
    }
    
    return ss.str();
}

Token Token::fromFileString(const string& line) {
    stringstream ss(line);
    string tId, uid, amtStr, timeStr, stat, itemsStr;
    
    getline(ss, tId, '|');
    getline(ss, uid, '|');
    getline(ss, amtStr, '|');
    getline(ss, timeStr, '|');
    getline(ss, stat, '|');
    getline(ss, itemsStr, '|');
    
    Token token(tId, uid);
    token.setStatus(stat);
    
    // Parse items
    if (!itemsStr.empty()) {
        stringstream itemStream(itemsStr);
        string itemData;
        while (getline(itemStream, itemData, ';')) {
            stringstream itemDetails(itemData);
            string name, qtyStr, priceStr;
            getline(itemDetails, name, ',');
            getline(itemDetails, qtyStr, ',');
            getline(itemDetails, priceStr, ',');
            
            token.addItem(name, stoi(qtyStr), stod(priceStr));
        }
    }
    
    return token;
}