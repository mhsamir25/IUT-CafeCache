#include "Token.h"
#include <iostream>
#include <iomanip>
#include <sstream>

#include "TerminalSetup.h"

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
vector<OrderItem> Token::getItems() const { return items; }

void Token::setStatus(string newStatus) { status = newStatus; }

void Token::displayToken() const {
    std::ostringstream header;
    header << "\n";
    printHeader("╔════════════════════════════════════════╗");
    printHeader("║          IUT CAFECACHE TOKEN           ║");
    printHeader("╚════════════════════════════════════════╝");
    printLabelValue("Token ID: ", tokenId);
    printLabelValue("User ID: ", userId);
    time_t t = timestamp;
    printLabelValue("Time: ", string(ctime(&t)));
    printSeparator();

    // table header
    {
        std::ostringstream ss;
        ss << std::left << std::setw(20) << "ITEM" << std::setw(8) << "QTY" << std::setw(14) << "PRICE" << "TOTAL";
        printInfo(ss.str());
    }
    printInfo(std::string("----------------------------------------"));

    for (const auto& item : items) {
        std::ostringstream ss;
        ss << std::left << std::setw(20) << item.itemName
           << std::setw(8) << item.quantity;
        {
            std::ostringstream pss; pss << "BDT " << std::fixed << std::setprecision(2) << item.price;
            ss << std::setw(14) << pss.str();
        }
        ss << "BDT " << std::fixed << std::setprecision(2) << item.totalPrice;
        printInfo(ss.str());
    }

    printInfo(std::string("----------------------------------------"));
    {
        std::ostringstream tot; tot << "TOTAL AMOUNT: BDT " << std::fixed << std::setprecision(2) << totalAmount;
        printSuccess(tot.str());
    }
    string statusColor = RESET;
    if (status == "ACTIVE") statusColor = CYAN;
    else if (status == "COMPLETED") statusColor = GREEN;
    else if (status == "CANCELLED") statusColor = RED;
    printLabelValueColored("STATUS: ", status, statusColor);
    printHeader("========================================\n");
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

pair<string, string> Token::extractDayAndMeal() const {
    if (items.empty()) {
        return {"", ""};
    }
    
    string itemName = items[0].itemName;
    
    // Split by space
    stringstream ss(itemName);
    string day = "", meal = "";
    
    ss >> day >> meal;  // Get first two words
    
    return {day, meal};
}