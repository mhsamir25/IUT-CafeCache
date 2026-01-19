#include "RechargeManager.h"
#include "FileManager.h"
#include "TokenGenerator.h"
#include "RechargeRequest.h"
#include <iostream>
#include <vector>
#include "TerminalSetup.h"

using namespace std;

bool RechargeManager::submitRechargeRequest(string userId, double amount) {
    if (amount <= 0) {
            printError("Error: Amount must be greater than 0.");
        return false;
    }
    
    string requestId = TokenGenerator::generateRequestId();
    RechargeRequest request(requestId, userId, amount);
    
    if (FileManager::addRechargeRequest(request)) {
        printSuccess("\n✓ Recharge request submitted successfully!");
        printLabelValue("Request ID: ", requestId);
        printLabelValue("Amount: BDT ", to_string(amount));
        printLabelValue("Status: ", string("PENDING"));
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
            user->addtoWallet(amount);
            FileManager::saveUsers(users);
        }
        
            printSuccess("\u2713 Recharge request approved successfully!");
            printLabelValue("User: ", userId);
            printLabelValue("Credited: BDT ", to_string(amount));
        return true;
    }
    
        printError("Error: Request not found or already processed.");
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
            printSuccess("\u2713 Recharge request rejected.");
        return true;
    }
    
        printError("Error: Request not found or already processed.");
    return false;
}