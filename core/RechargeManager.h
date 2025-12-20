#ifndef RECHARGEMANAGER_H
#define RECHARGEMANAGER_H

#include <string>

using namespace std;

class RechargeManager {
public:
    static bool submitRechargeRequest(string userId, double amount);
    static void viewUserRequests(string userId);
    static void viewPendingRequests();
    static bool approveRequest(string requestId);
    static bool rejectRequest(string requestId);
};

#endif