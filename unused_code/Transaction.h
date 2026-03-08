#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include "Utils.h"

using namespace std;

class Transaction {
public:
    string studentId;
    string itemName;
    string category;
    int quantity;
    double amount;
    string timestamp;
    string date;
    string day;

    static void recordTransaction(const string& studentId, const string& itemName, 
                                  const string& category, int quantity, double amount) {
        ofstream file("transactions.txt", ios::app);
        if (!file) {
            cout << "\nError: Cannot record transaction!" << endl;
            return;
        }

        file << studentId << "|" << itemName << "|" << category << "|" 
             << quantity << "|" << fixed << setprecision(2) << amount << "|"
             << Utils::getCurrentDateTime() << "|" << Utils::getCurrentDate() << "|"
             << Utils::getDayOfWeek() << endl;
        file.close();
    }

    static void viewUserTransactions(const string& studentId) {
        ifstream file("transactions.txt");
        if (!file) {
            cout << "\nError: No transactions found!" << endl;
            return;
        }

        cout << "\n" << string(100, '=') << endl;
        cout << "TRANSACTION LOG - Student ID: " << studentId << endl;
        cout << string(100, '=') << endl;
        
        string line;
        bool found = false;
        double totalSpent = 0.0;
        
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            while (getline(ss, token, '|')) {
                tokens.push_back(Utils::trim(token));
            }

            if (tokens.size() >= 8 && tokens[0] == studentId) {
                found = true;
                totalSpent += stod(tokens[4]);
                
                cout << "\nDate: " << tokens[5] << " (" << tokens[7] << ")" << endl;
                cout << "Item: " << tokens[1] << " (" << tokens[2] << ")" << endl;
                cout << "Quantity: " << tokens[3] << " | Amount: Tk " << tokens[4] << endl;
                cout << string(100, '-') << endl;
            }
        }
        
        file.close();
        
        if (!found) {
            cout << "\nNo transactions found." << endl;
        } else {
            cout << "\nTotal Spent: Tk " << fixed << setprecision(2) << totalSpent << endl;
        }
        cout << string(100, '=') << endl;
    }

    static void viewAllTransactions() {
        ifstream file("transactions.txt");
        if (!file) {
            cout << "\nError: No transactions found!" << endl;
            return;
        }

        cout << "\n" << string(100, '=') << endl;
        cout << "ALL TRANSACTIONS" << endl;
        cout << string(100, '=') << endl;
        
        string line;
        
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            while (getline(ss, token, '|')) {
                tokens.push_back(Utils::trim(token));
            }

            if (tokens.size() >= 8) {
                cout << "\nStudent ID: " << tokens[0] << " | Date: " << tokens[5] << endl;
                cout << "Item: " << tokens[1] << " (" << tokens[2] << ")" << endl;
                cout << "Quantity: " << tokens[3] << " | Amount: Tk " << tokens[4] << endl;
                cout << string(100, '-') << endl;
            }
        }
        
        file.close();
        cout << string(100, '=') << endl;
    }
};

#endif