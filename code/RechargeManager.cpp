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

vector<RechargeRequest> RechargeManager::getPendingRequests() {
    vector<RechargeRequest> allRequests = FileManager::loadRechargeRequests();
    vector<RechargeRequest> pending;
    for (const auto &req : allRequests) {
        if (req.getStatus() == "PENDING") pending.push_back(req);
    }
    return pending;
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

vector<int> RechargeManager::parseRequestIndices(const string& input, int maxSize) {
    vector<int> indices;
    string temp = input;
    size_t start = 0;
    
    // Split by comma
    while (start < temp.length()){
        size_t comma = temp.find(',', start);
        if (comma == string::npos) comma = temp.length();
        
        string part = temp.substr(start, comma - start);
        
        // Trim whitespace
        size_t firstNonSpace = part.find_first_not_of(" \t");
        size_t lastNonSpace = part.find_last_not_of(" \t");
        if (firstNonSpace == string::npos) {
            start = comma + 1;
            continue;
        }
        part = part.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);
        
        // Check if it's a range (contains '-')
        size_t dashPos = part.find('-');
        if (dashPos != string::npos && dashPos > 0 && dashPos < part.length() - 1) {
            // It's a range like "1-10"
            int rangeStart = stoi(part.substr(0, dashPos));
            int rangeEnd = stoi(part.substr(dashPos + 1));
            
            if (rangeStart >= 1 && rangeEnd >= rangeStart && rangeEnd <= maxSize) {
                for (int i = rangeStart; i <= rangeEnd; ++i) {
                    indices.push_back(i);
                }
            }
        } else {
            // Single number
            int num = stoi(part);
            if (num >= 1 && num <= maxSize) {
                indices.push_back(num);
            }
        }
        
        start = comma + 1;
    }
    
    return indices;
}

int RechargeManager::bulkApproveRequests(const vector<int>& indices) {
    vector<RechargeRequest> allPending = RechargeManager::getPendingRequests();
    int approvedCount = 0;
    
    for (int idx : indices) {
        if (idx >= 1 && idx <= (int)allPending.size()) {
            if (RechargeManager::approveRequest(allPending[idx - 1].getRequestId())) {
                approvedCount++;
            }
        }
    }
    
    return approvedCount;
}

int RechargeManager::bulkRejectRequests(const vector<int>& indices) {
    vector<RechargeRequest> allPending = RechargeManager::getPendingRequests();
    int rejectedCount = 0;
    
    for (int idx : indices) {
        if (idx >= 1 && idx <= (int)allPending.size()) {
            if (RechargeManager::rejectRequest(allPending[idx - 1].getRequestId())) {
                rejectedCount++;
            }
        }
    }
    
    return rejectedCount;
}