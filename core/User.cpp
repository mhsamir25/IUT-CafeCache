#include "User.h"
#include <sstream>
#include <iostream>

using namespace std;

User::User() : userID(""), password(""), name(""), role(""), walletBalance(0.0) {}

User::User(string id, string pass, string userName, string userRole, double balance):
    userID(id), password(pass), name(userName), role(userRole), walletBalance(balance) {}

string User::getUserID() const{
    return userID;
}

string User::getPassword() const{
    return password;
}

string User::getName() const{
    return name;
}

string User::getRole() const{
    return role;
}

double User::getWalletBalance() const{
    return walletBalance;
}

void User::setWalletBalance(double amount){
    walletBalance = amount;
}

void User::addtoWallet(double amount){
    if(amount > 0){
        walletBalance += amount;
    }
}

bool User::deductFromWallet(double amount){
    if(amount>0 && walletBalance >=amount){
        walletBalance -= amount;
        return true;
    }
    return false;
}

void User:: displayInfo() const{
    cout<< "\n=============USER INFORMATION=============="<<endl;
    cout<< "User ID: " <<userID<<endl;
    cout<< "Name: " <<name<<endl;
    cout<<"Role: " <<role<<endl;
    cout<<"Wallet Balance: BDT " <<walletBalance<<endl;
    cout<<"==========================================\n"<<endl;
}

string User::toFileString() const{
    return userID + "|" + password + "|" + name + "|" + role + "|" + to_string(walletBalance);  
}

User User::fromFileString(const string& line){
    stringstream ss(line);
    string id, pass, userName, userRole, balanceStr;
    getline(ss, id, '|');
    getline(ss, pass, '|');
    getline(ss, userName, '|');
    getline(ss, userRole, '|');
    getline(ss, balanceStr, '|');
    double balance = balanceStr.empty() ? 0.0 : stod(balanceStr);
    return User(id, pass, userName, userRole, balance); 
}