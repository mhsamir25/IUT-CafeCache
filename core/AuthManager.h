#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H

#include<string>
#include "User.h"

using namespace std;

class AuthManager{
    private:
        static User* currentUser;
    public:
        static bool login(string userID, string password);
        static void logout();
        static User* getCurrentUser();
        static bool isLoggedIn();
        static bool isAdmin();
};

#endif