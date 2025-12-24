#include "FileManager.h"
#include <fstream>
#include <iostream>
#include "TerminalSetup.h"

using namespace std;

const string FileManager::USERS_FILE = "users.txt";
const string FileManager::RECHARGE_FILE = "recharge_requests.txt";
const string FileManager::TOKENS_FILE = "tokens.txt";

// User operations
vector<User> FileManager::loadUsers() {
    vector<User> users;
    ifstream file(USERS_FILE);
    
    if (!file.is_open()) {
        // Create default admin if file doesn't exist
        User admin("ADMIN001", "admin123", "System Admin", "ADMIN", 0.0);
        users.push_back(admin);
        saveUsers(users);
        return users;
    }
    
    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            users.push_back(User::fromFileString(line));
        }
    }
    
    file.close();
    
    // Ensure at least one admin exists
    if (users.empty()) {
        User admin("ADMIN001", "admin123", "System Admin", "ADMIN", 0.0);
        users.push_back(admin);
        saveUsers(users);
    }
    
    return users;
}

bool FileManager::saveUsers(const vector<User>& users) {
    ofstream file(USERS_FILE);
    
    if (!file.is_open()) {
        cerr << RED << "Error: Could not open users file for writing." << RESET << endl;
        return false;
    }
    
    for (const auto& user : users) {
        file << user.toFileString() << endl;
    }
    
    file.close();
    return true;
}

bool FileManager::addUser(const User& user) {
    ofstream file(USERS_FILE, ios::app);
    
    if (!file.is_open()) {
        cerr << RED << "Error: Could not open users file." << RESET << endl;
        return false;
    }
    
    file << user.toFileString() << endl;
    file.close();
    return true;
}

bool FileManager::updateUser(const User& user) {
    vector<User> users = loadUsers();
    bool found = false;
    
    for (auto& u : users) {
        if (u.getUserID() == user.getUserID()) {
            u = user;
            found = true;
            break;
        }
    }
    
    if (found) {
        return saveUsers(users);
    }
    
    return false;
}

User* FileManager::findUser(const string& userId, vector<User>& users) {
    for (auto& user : users) {
        if (user.getUserID() == userId) {
            return &user;
        }
    }
    return nullptr;
}

// Recharge operations
vector<RechargeRequest> FileManager::loadRechargeRequests() {
    vector<RechargeRequest> requests;
    ifstream file(RECHARGE_FILE);
    
    if (!file.is_open()) {
        return requests;
    }
    
    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            requests.push_back(RechargeRequest::fromFileString(line));
        }
    }
    
    file.close();
    return requests;
}

bool FileManager::saveRechargeRequests(const vector<RechargeRequest>& requests) {
    ofstream file(RECHARGE_FILE);
    
    if (!file.is_open()) {
        cerr << RED << "Error: Could not open recharge requests file for writing." << RESET << endl;
        return false;
    }
    
    for (const auto& req : requests) {
        file << req.toFileString() << endl;
    }
    
    file.close();
    return true;
}

bool FileManager::addRechargeRequest(const RechargeRequest& request) {
    ofstream file(RECHARGE_FILE, ios::app);
    
    if (!file.is_open()) {
        cerr << RED << "Error: Could not open recharge requests file." << RESET << endl;
        return false;
    }
    
    file << request.toFileString() << endl;
    file.close();
    return true;
}

// Token operations
vector<Token> FileManager::loadTokens() {
    vector<Token> tokens;
    ifstream file(TOKENS_FILE);
    
    if (!file.is_open()) {
        return tokens;
    }
    
    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            tokens.push_back(Token::fromFileString(line));
        }
    }
    
    file.close();
    return tokens;
}

bool FileManager::saveTokens(const vector<Token>& tokens) {
    ofstream file(TOKENS_FILE);
    
    if (!file.is_open()) {
        cerr << RED << "Error: Could not open tokens file for writing." << RESET << endl;
        return false;
    }
    
    for (const auto& token : tokens) {
        file << token.toFileString() << endl;
    }
    
    file.close();
    return true;
}

bool FileManager::addToken(const Token& token) {
    ofstream file(TOKENS_FILE, ios::app);
    
    if (!file.is_open()) {
        cerr << RED << "Error: Could not open tokens file." << RESET << endl;
        return false;
    }
    
    file << token.toFileString() << endl;
    file.close();
    return true;
}

    vector<Token> FileManager::getUserTokens(const std::string& userId) {
    vector<Token> matched;

    // Assumes FileManager::loadTokens() exists and returns all tokens from tokens.txt
    vector<Token> allTokens = FileManager::loadTokens();

    for (const auto &t : allTokens) {
        if (t.getUserId() == userId) {
            matched.push_back(t);
        }
    }

    return matched;
}