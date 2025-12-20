#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>
#include "User.h"
#include "RechargeRequest.h"
#include "Token.h"

using namespace std;

class FileManager {
private:
    static const string USERS_FILE;
    static const string RECHARGE_FILE;
    static const string TOKENS_FILE;
    
public:
    // User operations
    static vector<User> loadUsers();
    static bool saveUsers(const vector<User>& users);
    static bool addUser(const User& user);
    static bool updateUser(const User& user);
    static User* findUser(const string& userId, vector<User>& users);
    
    // Recharge operations
    static vector<RechargeRequest> loadRechargeRequests();
    static bool saveRechargeRequests(const vector<RechargeRequest>& requests);
    static bool addRechargeRequest(const RechargeRequest& request);
    
    // Token operations
    static vector<Token> loadTokens();
    static bool saveTokens(const vector<Token>& tokens);
    static bool addToken(const Token& token);
    static vector<Token> getUserTokens(const string& userId);
};

#endif