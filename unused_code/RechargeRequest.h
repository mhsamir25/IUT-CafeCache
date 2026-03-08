#ifndef RECHARGEREQUEST_H
#define RECHARGEREQUEST_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <string>
#include "Utils.h"
#include "User.h"

using namespace std;

class RechargeRequest {
public:
    int requestId;
    string studentId;
    string username;
    double amount;
    string timestamp;
    string status;

    static void createRequest(const string& studentId, const string& username, double amount) {
        if (amount <= 0) {
            cout << "\nError: Invalid amount!" << endl;
            return;
        }

        ofstream file("recharge_requests.txt", ios::app);
        if (!file) {
            cout << "\nError: Cannot create recharge request!" << endl;
            return;
        }

        int requestId = Utils::generateRandomId();
        file << requestId << "|" << studentId << "|" << username << "|" 
             << fixed << setprecision(2) << amount << "|" 
             << Utils::getCurrentDateTime() << "|pending" << endl;
        file.close();

        Utils::clearScreen();
        cout << "\nRecharge request submitted successfully!" << endl;
        cout << "Request ID: " << requestId << endl;
        cout << "Amount: Tk " << fixed << setprecision(2) << amount << endl;
        cout << "Status: Pending Admin Approval" << endl;
    }

    struct RequestInfo {
        int requestId;
        string studentId;
        string username;
        double amount;
        string timestamp;
    };

    static vector<RequestInfo> viewPendingRequests() {
        vector<RequestInfo> pendingRequests;
        ifstream file("recharge_requests.txt");
        if (!file) {
            cout << "\nError: No recharge requests found!" << endl;
            return pendingRequests;
        }

        cout << "\n" << string(80, '=') << endl;
        cout << "PENDING RECHARGE REQUESTS" << endl;
        cout << string(80, '=') << endl;
        
        string line;
        int index = 1;
        
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            while (getline(ss, token, '|')) {
                tokens.push_back(Utils::trim(token));
            }

            if (tokens.size() >= 6 && tokens[5] == "pending") {
                RequestInfo req;
                req.requestId = stoi(tokens[0]);
                req.studentId = tokens[1];
                req.username = tokens[2];
                req.amount = stod(tokens[3]);
                req.timestamp = tokens[4];
                pendingRequests.push_back(req);

                cout << "\n[" << index << "] Request ID: " << tokens[0] << endl;
                cout << "    Student ID: " << tokens[1] << endl;
                cout << "    Username: " << tokens[2] << endl;
                cout << "    Amount: Tk " << tokens[3] << endl;
                cout << "    Timestamp: " << tokens[4] << endl;
                cout << string(80, '-') << endl;
                index++;
            }
        }
        
        file.close();
        
        if (pendingRequests.empty()) {
            cout << "\nNo pending requests." << endl;
        }
        cout << string(80, '=') << endl;
        
        return pendingRequests;
    }

    static void approveRequests(const vector<int>& requestIds) {
        if (requestIds.empty()) {
            cout << "\nError: No requests selected!" << endl;
            return;
        }

        stringstream ss;
        ss << "Approve " << requestIds.size() << " request(s)?";
        if (!Utils::confirmAction(ss.str())) {
            cout << "\nAction cancelled." << endl;
            return;
        }

        int successCount = 0;
        double totalApproved = 0.0;

        for (size_t idx = 0; idx < requestIds.size(); idx++) {
            int reqId = requestIds[idx];
            vector<string> allRequests;
            ifstream file("recharge_requests.txt");
            string line;
            bool found = false;
            string targetStudentId;
            double rechargeAmount = 0.0;

            while (getline(file, line)) {
                stringstream ss2(line);
                string token;
                vector<string> tokens;
                
                while (getline(ss2, token, '|')) {
                    tokens.push_back(Utils::trim(token));
                }

                if (tokens.size() >= 6 && stoi(tokens[0]) == reqId && tokens[5] == "pending") {
                    found = true;
                    targetStudentId = tokens[1];
                    rechargeAmount = stod(tokens[3]);
                    
                    stringstream newLine;
                    newLine << tokens[0] << "|" << tokens[1] << "|" << tokens[2] << "|"
                           << tokens[3] << "|" << tokens[4] << "|approved";
                    allRequests.push_back(newLine.str());
                } else {
                    allRequests.push_back(line);
                }
            }
            file.close();

            if (found) {
                ofstream outFile("recharge_requests.txt");
                for (const auto& req : allRequests) {
                    outFile << req << endl;
                }
                outFile.close();

                User* user = User::getUserByIdentifier(targetStudentId);
                if (user) {
                    user->balance += rechargeAmount;
                    user->updateBalance();
                    successCount++;
                    totalApproved += rechargeAmount;
                    
                    cout << "\nApproved: Student ID " << targetStudentId 
                         << " - Tk " << fixed << setprecision(2) << rechargeAmount << endl;
                    
                    delete user;
                }
            }
        }

        Utils::clearScreen();
        cout << "\n" << string(80, '=') << endl;
        cout << "APPROVAL SUMMARY" << endl;
        cout << string(80, '=') << endl;
        cout << "Total Requests Approved: " << successCount << endl;
        cout << "Total Amount Approved: Tk " << fixed << setprecision(2) << totalApproved << endl;
        cout << string(80, '=') << endl;
    }

    static void rejectRequests(const vector<int>& requestIds) {
        if (requestIds.empty()) {
            cout << "\nError: No requests selected!" << endl;
            return;
        }

        int successCount = 0;

        for (size_t idx = 0; idx < requestIds.size(); idx++) {
            int reqId = requestIds[idx];
            vector<string> allRequests;
            ifstream file("recharge_requests.txt");
            string line;
            bool found = false;

            while (getline(file, line)) {
                stringstream ss2(line);
                string token;
                vector<string> tokens;
                
                while (getline(ss2, token, '|')) {
                    tokens.push_back(Utils::trim(token));
                }

                if (tokens.size() >= 6 && stoi(tokens[0]) == reqId && tokens[5] == "pending") {
                    found = true;
                    stringstream newLine;
                    newLine << tokens[0] << "|" << tokens[1] << "|" << tokens[2] << "|"
                           << tokens[3] << "|" << tokens[4] << "|rejected";
                    allRequests.push_back(newLine.str());
                } else {
                    allRequests.push_back(line);
                }
            }
            file.close();

            if (found) {
                ofstream outFile("recharge_requests.txt");
                for (const auto& req : allRequests) {
                    outFile << req << endl;
                }
                outFile.close();
                successCount++;
                cout << "\nRejected: Request ID " << reqId << endl;
            }
        }

        Utils::clearScreen();
        cout << "\n" << string(80, '=') << endl;
        cout << "REJECTION SUMMARY" << endl;
        cout << string(80, '=') << endl;
        cout << "Total Requests Rejected: " << successCount << endl;
        cout << string(80, '=') << endl;
    }
};

#endif