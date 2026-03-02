#ifndef RECHARGEMANAGER_H
#define RECHARGEMANAGER_H

#include <string>
#include <vector>

using namespace std;

class RechargeManager {
public:
    static bool submitRechargeRequest(string userId, double amount);
    static void viewUserRequests(string userId);
    static void viewPendingRequests();
    // New helper: return pending requests so callers (admin UI) can show a numbered list
    static vector<class RechargeRequest> getPendingRequests();
    static bool approveRequest(string requestId);
    static bool rejectRequest(string requestId);
    // Bulk operations
    static vector<int> parseRequestIndices(const string& input, int maxSize);
    static int bulkApproveRequests(const vector<int>& indices);
    static int bulkRejectRequests(const vector<int>& indices);
};

#endif