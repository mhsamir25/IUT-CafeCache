#include "RechargeRequest.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include "TerminalSetup.h"

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
    // Use colored label/value printing
    printLabelValue("Request ID: ", requestId);
    printLabelValue("User ID: ", userId);
    {
        std::ostringstream ss; ss << fixed << setprecision(2) << amount;
        printLabelValue("Amount: BDT ", ss.str());
    }
    string statusColor = RESET;
    if (status == "PENDING") statusColor = YELLOW;
    else if (status == "APPROVED") statusColor = GREEN;
    else if (status == "REJECTED") statusColor = RED;
    cout << "Status: " << statusColor << status << RESET << endl;
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
        cout << GRAY << "\nNo recharge requests found.\n" << RESET << endl;
        return;
    }
    
    cout << BOLD << CYAN << "\n========== ALL RECHARGE REQUESTS ==========" << RESET << "\n" << endl;
    RechargeNode* current = head;
    while (current != nullptr) {
        current->request.display();
        current = current->next;
    }
}

void RechargeList::displayUserRequests(string userId) const {
    bool found = false;
    cout << BOLD << CYAN << "\n========== YOUR RECHARGE REQUESTS ==========" << RESET << "\n" << endl;
    
    RechargeNode* current = head;
    while (current != nullptr) {
        if (current->request.getUserId() == userId) {
            current->request.display();
            found = true;
        }
        current = current->next;
    }
    
    if (!found) {
        cout << GREEN << "No recharge requests found.\n" << RESET << endl;
    }
}

void RechargeList::displayPendingRequests() const {
    bool found = false;
    cout << BOLD << CYAN << "\n========== PENDING RECHARGE REQUESTS ==========" << RESET << "\n" << endl;
    
    RechargeNode* current = head;
    while (current != nullptr) {
        if (current->request.getStatus() == "PENDING") {
            current->request.display();
            found = true;
        }
        current = current->next;
    }
    
    if (!found) {
        cout << GREEN << "No pending recharge requests.\n" << RESET << endl;
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