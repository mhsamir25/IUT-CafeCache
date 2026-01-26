#ifndef RECHARGEREQUEST_H
#define RECHARGEREQUEST_H

#include <string>
#include <ctime>

using namespace std;

class RechargeRequest {
private:
    string requestId;
    string userId;
    double amount;
    string status; 
    time_t timestamp;
    
public:
    RechargeRequest();
    RechargeRequest(string reqId, string uid, double amt);
    
    string getRequestId() const;
    string getUserId() const;
    double getAmount() const;
    string getStatus() const;
    time_t getTimestamp() const;
    
    void setStatus(string newStatus);
    
    string toFileString() const;
    static RechargeRequest fromFileString(const string& line);
    void display() const;
};

// Node for linked list
class RechargeNode {
public:
    RechargeRequest request;
    RechargeNode* next;
    
    RechargeNode(RechargeRequest req);
};

// Linked list for recharge requests
class RechargeList {
private:
    RechargeNode* head;
    
public:
    RechargeList();
    ~RechargeList();
    
    void addRequest(RechargeRequest request);
    RechargeRequest* findRequest(string requestId);
    void displayAllRequests() const;
    void displayUserRequests(string userId) const;
    void displayPendingRequests() const;
    bool updateRequestStatus(string requestId, string status);
};

#endif