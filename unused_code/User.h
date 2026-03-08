#ifndef USER_H
#define USER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include "Utils.h"

using namespace std;

class User {
public:
    string studentId;
    string username;
    string password;
    double balance;
    string role;

    User() : balance(0.0), role("student") {}

    User(string sid, string uname, string pass, double bal, string r) 
        : studentId(sid), username(uname), password(pass), balance(bal), role(r) {}

    static bool registerUser(const string& studentId, const string& username, 
                            const string& password, const string& role) {
        ifstream file("users.txt");
        string line;
        
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            while (getline(ss, token, '|')) {
                tokens.push_back(Utils::trim(token));
            }
            
            if (tokens.size() >= 4 && tokens[0] == studentId) {
                file.close();
                cout << "\nError: Student ID already exists!" << endl;
                return false;
            }

            if (tokens.size() >= 4 && tokens[1] == username) {
                file.close();
                cout << "\nError: Username already exists!" << endl;
                return false;
            }
        }
        file.close();

        ofstream outFile("users.txt", ios::app);
        if (!outFile) {
            cout << "\nError: Cannot open users file!" << endl;
            return false;
        }

        outFile << studentId << "|" << username << "|" << password << "|0.0|" << role << endl;
        outFile.close();

        cout << "\nRegistration successful!" << endl;
        return true;
    }

    static User* login(const string& username, const string& password) {
        ifstream file("users.txt");
        if (!file) {
            cout << "\nError: Cannot open users file!" << endl;
            return nullptr;
        }

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            while (getline(ss, token, '|')) {
                tokens.push_back(Utils::trim(token));
            }

            if (tokens.size() >= 5 && tokens[1] == username && tokens[2] == password) {
                User* user = new User();
                user->studentId = tokens[0];
                user->username = tokens[1];
                user->password = tokens[2];
                user->balance = stod(tokens[3]);
                user->role = tokens[4];
                
                file.close();
                return user;
            }
        }
        
        file.close();
        cout << "\nError: Invalid username or password!" << endl;
        return nullptr;
    }

    void updateBalance() {
        vector<string> allUsers;
        ifstream file("users.txt");
        string line;
        bool updated = false;

        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            while (getline(ss, token, '|')) {
                tokens.push_back(Utils::trim(token));
            }

            if (tokens.size() >= 5 && tokens[0] == studentId) {
                stringstream newLine;
                newLine << studentId << "|" << username << "|" << password << "|" 
                       << fixed << setprecision(2) << balance << "|" << role;
                allUsers.push_back(newLine.str());
                updated = true;
            } else {
                allUsers.push_back(line);
            }
        }
        file.close();

        ofstream outFile("users.txt");
        for (const auto& userLine : allUsers) {
            outFile << userLine << endl;
        }
        outFile.close();
    }

    static User* getUserByIdentifier(const string& identifier) {
        ifstream file("users.txt");
        if (!file) return nullptr;

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            while (getline(ss, token, '|')) {
                tokens.push_back(Utils::trim(token));
            }

            if (tokens.size() >= 5 && (tokens[0] == identifier || tokens[1] == identifier)) {
                User* user = new User();
                user->studentId = tokens[0];
                user->username = tokens[1];
                user->password = tokens[2];
                user->balance = stod(tokens[3]);
                user->role = tokens[4];
                file.close();
                return user;
            }
        }
        
        file.close();
        return nullptr;
    }

    void viewWallet() {
        cout << "\n" << string(60, '=') << endl;
        cout << "WALLET INFORMATION" << endl;
        cout << string(60, '=') << endl;
        cout << "Student ID: " << studentId << endl;
        cout << "Username: " << username << endl;
        cout << "Current Balance: Tk " << fixed << setprecision(2) << balance << endl;
        cout << string(60, '=') << endl;
    }
};

#endif