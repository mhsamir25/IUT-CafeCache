#include "RechargeManager.h"
#include "FileManager.h"
#include "TokenGenerator.h"
#include "RechargeRequest.h"
#include <iostream>
#include <vector>

using namespace std;

bool RechargeManager::submitRechargeRequest(string userId, double amount) {
    if (amount <= 0) {
        cout << "\nError: Amount must be greater than 0.\n" << endl;
        return false;
    }
    
    string requestId = TokenGenerator::generateRequestId();
    RechargeRequest request(requestId, userId, amount);
    
    if (FileManager::addRechargeRequest(request)) {
        cout << "\n✓ Recharge request submitted successfully!" << endl;
        cout << "Request ID: " << requestId << endl;
        cout << "Amount: BDT " << amount << endl;
        cout << "Status: PENDING\n" << endl;
        return true;
    }
    
    return false;
}

void RechargeManager::viewUserRequests(string userId) {
    vector<RechargeRequest> allRequests = FileManager::loadRechargeRequests();
    RechargeList list;
    
    for (const auto& req : allRequests) {
        list.addRequest(req);
    }
    
    list.displayUserRequests(userId);
}

void RechargeManager::viewPendingRequests() {
    vector<RechargeRequest> allRequests = FileManager::loadRechargeRequests();
    RechargeList list;
    
    for (const auto& req : allRequests) {
        list.addRequest(req);
    }
    
    list.displayPendingRequests();
}

bool RechargeManager::approveRequest(string requestId) {
    vector<RechargeRequest> requests = FileManager::loadRechargeRequests();
    bool found = false;
    string userId;
    double amount;
    
    for (auto& req : requests) {
        if (req.getRequestId() == requestId && req.getStatus() == "PENDING") {
            req.setStatus("APPROVED");
            userId = req.getUserId();
            amount = req.getAmount();
            found = true;
            break;
        }
    }
    
    if (found) {
        FileManager::saveRechargeRequests(requests);
        
        // Update user wallet
        vector<User> users = FileManager::loadUsers();
        User* user = FileManager::findUser(userId, users);
        
        if (user != nullptr) {
            user->addToWallet(amount);
            FileManager::saveUsers(users);
        }
        
        cout << "\n✓ Recharge request approved successfully!" << endl;
        cout << "User: " << userId << " has been credited BDT " << amount << "\n" << endl;
        return true;
    }
    
    cout << "\nError: Request not found or already processed.\n" << endl;
    return false;
}

bool RechargeManager::rejectRequest(string requestId) {
    vector<RechargeRequest> requests = FileManager::loadRechargeRequests();
    bool found = false;
    
    for (auto& req : requests) {
        if (req.getRequestId() == requestId && req.getStatus() == "PENDING") {
            req.setStatus("REJECTED");
            found = true;
            break;
        }
    }
    
    if (found) {
        FileManager::saveRechargeRequests(requests);
        cout << "\n✓ Recharge request rejected.\n" << endl;
        return true;
    }
    
    cout << "\nError: Request not found or already processed.\n" << endl;
    return false;
}