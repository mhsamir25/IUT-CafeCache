#include "RechargeRequest.h"
#include <sstream>
#include <iostream>
#include <iomanip>

using namespace std;

RechargeRequest::RechargeRequest() 
    : requestId(""), userId(""), amount(0.0), status("PENDING"), timestamp(time(nullptr)) {}

RechargeRequest::RechargeRequest(string reqId, string uid, double amt)
    : requestId(reqId), userId(uid), amount(amt), status("PENDING"), timestamp(time(nullptr)) {}

string RechargeRequest::getRequestId() const { return requestId; }
string RechargeRequest::getUserId() const { return userId; }
double RechargeRequest::getAmount() const { return amount; }
string RechargeRequest::getStatus() const { return status; }
time_t RechargeRequest::getTimestamp() const { return timestamp; }

void RechargeRequest::setStatus(string newStatus) { status = newStatus; }

string RechargeRequest::toFileString() const {
    return requestId + "|" + userId + "|" + to_string(amount) + "|" + status + "|" + to_string(timestamp);
}

RechargeRequest RechargeRequest::fromFileString(const string& line) {
    stringstream ss(line);
    string reqId, uid, amtStr, stat, timeStr;
    
    getline(ss, reqId, '|');
    getline(ss, uid, '|');
    getline(ss, amtStr, '|');
    getline(ss, stat, '|');
    getline(ss, timeStr, '|');
    
    RechargeRequest req(reqId, uid, stod(amtStr));
    req.setStatus(stat);
    
    return req;
}

void RechargeRequest::display() const {
    time_t t = timestamp;
    cout << "Request ID: " << requestId << endl;
    cout << "User ID: " << userId << endl;
    cout << "Amount: BDT " << fixed << setprecision(2) << amount << endl;
    cout << "Status: " << status << endl;
    cout << "Time: " << ctime(&t);
    cout << "----------------------------------------" << endl;
}

// RechargeList implementation
RechargeNode::RechargeNode(RechargeRequest req) : request(req), next(nullptr) {}

RechargeList::RechargeList() : head(nullptr) {}

RechargeList::~RechargeList() {
    RechargeNode* current = head;
    while (current != nullptr) {
        RechargeNode* temp = current;
        current = current->next;
        delete temp;
    }
}

void RechargeList::addRequest(RechargeRequest request) {
    RechargeNode* newNode = new RechargeNode(request);
    if (head == nullptr) {
        head = newNode;
    } else {
        RechargeNode* current = head;
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = newNode;
    }
}

RechargeRequest* RechargeList::findRequest(string requestId) {
    RechargeNode* current = head;
    while (current != nullptr) {
        if (current->request.getRequestId() == requestId) {
            return &(current->request);
        }
        current = current->next;
    }
    return nullptr;
}

void RechargeList::displayAllRequests() const {
    if (head == nullptr) {
        cout << "\nNo recharge requests found.\n" << endl;
        return;
    }
    
    cout << "\n========== ALL RECHARGE REQUESTS ==========\n" << endl;
    RechargeNode* current = head;
    while (current != nullptr) {
        current->request.display();
        current = current->next;
    }
}

void RechargeList::displayUserRequests(string userId) const {
    bool found = false;
    cout << "\n========== YOUR RECHARGE REQUESTS ==========\n" << endl;
    
    RechargeNode* current = head;
    while (current != nullptr) {
        if (current->request.getUserId() == userId) {
            current->request.display();
            found = true;
        }
        current = current->next;
    }
    
    if (!found) {
        cout << "No recharge requests found.\n" << endl;
    }
}

void RechargeList::displayPendingRequests() const {
    bool found = false;
    cout << "\n========== PENDING RECHARGE REQUESTS ==========\n" << endl;
    
    RechargeNode* current = head;
    while (current != nullptr) {
        if (current->request.getStatus() == "PENDING") {
            current->request.display();
            found = true;
        }
        current = current->next;
    }
    
    if (!found) {
        cout << "No pending recharge requests.\n" << endl;
    }
}

bool RechargeList::updateRequestStatus(string requestId, string status) {
    RechargeRequest* req = findRequest(requestId);
    if (req != nullptr) {
        req->setStatus(status);
        return true;
    }
    return false;
}