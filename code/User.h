#ifndef USER_H
#define USER_H

#include <string>
#include <ctime>

using namespace std;    

class User{
    protected:
        string userID;
        string password;
        string name;
        string role;
        double walletBalance;

    public:
        User();
        User(string id, string pass, string userName, string userRole, double balance = 0.0);

        string getUserID() const;
        string getPassword() const;
        string getName() const;
        string getRole() const;
        double getWalletBalance() const;

        void setWalletBalance(double amount);
        void addtoWallet(double amount);
        bool deductFromWallet(double amount);

        virtual void displayInfo() const;
        string toFileString() const;
        static User fromFileString(const string& fileString);
};

#endif