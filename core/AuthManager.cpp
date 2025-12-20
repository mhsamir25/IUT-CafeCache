#include "AuthManager.h"
#include "FileManager.h"

#include <iostream>

using namespace std;

User* AuthManager::currentUser = nullptr;

bool AuthManager::login(string userID, string password){
    //To Come After FileManager is ready
}

void AuthManager::logout(){
    if(currentUser != nullptr){
        delete currentUser;
        currentUser = nullptr;
    }
}

User* AuthManager::getCurrentUser(){
    return currentUser;
}

bool AuthManager::isLoggedIn(){
    return currentUser != nullptr;  
}

bool AuthManager::isAdmin(){
    return currentUser != nullptr && currentUser->getRole() == "ADMIN";
}