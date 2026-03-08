#ifndef FEEDBACK_H
#define FEEDBACK_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Utils.h"

using namespace std;

class Feedback {
public:
    static void submitFeedback(const string& studentId, const string& category, 
                               const string& day, const string& feedbackText) {
        if (feedbackText.length() > 250) {
            cout << "\nError: Feedback exceeds 250 characters limit!" << endl;
            return;
        }

        if (feedbackText.empty()) {
            cout << "\nError: Feedback cannot be empty!" << endl;
            return;
        }

        ofstream file("feedbacks.txt", ios::app);
        if (!file) {
            cout << "\nError: Cannot submit feedback!" << endl;
            return;
        }

        file << studentId << "_review|" << category << "|" << day << "|"
             << Utils::getCurrentDateTime() << "|" << feedbackText << endl;
        file.close();

        Utils::clearScreen();
        cout << "\nFeedback submitted successfully!" << endl;
        cout << "Thank you for your feedback!" << endl;
    }

    static void viewAllFeedbacks() {
        ifstream file("feedbacks.txt");
        if (!file) {
            cout << "\nError: No feedbacks found!" << endl;
            return;
        }

        cout << "\n" << string(100, '=') << endl;
        cout << "ALL FEEDBACKS" << endl;
        cout << string(100, '=') << endl;
        
        string line;
        int count = 1;
        
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            while (getline(ss, token, '|')) {
                tokens.push_back(Utils::trim(token));
            }

            if (tokens.size() >= 5) {
                cout << "\n#" << count++ << " " << tokens[0] << endl;
                cout << "Category: " << tokens[1] << " | Day: " << tokens[2] << endl;
                cout << "Time: " << tokens[3] << endl;
                cout << "Feedback: " << tokens[4] << endl;
                cout << string(100, '-') << endl;
            }
        }
        
        file.close();
        cout << string(100, '=') << endl;
    }

    static void viewFeedbacksByCategory(const string& category) {
        ifstream file("feedbacks.txt");
        if (!file) {
            cout << "\nError: No feedbacks found!" << endl;
            return;
        }

        cout << "\n" << string(100, '=') << endl;
        string displayCategory = category;
        displayCategory[0] = toupper(displayCategory[0]);
        cout << "FEEDBACKS - " << displayCategory << endl;
        cout << string(100, '=') << endl;
        
        string line;
        bool found = false;
        
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            while (getline(ss, token, '|')) {
                tokens.push_back(Utils::trim(token));
            }

            if (tokens.size() >= 5 && tokens[1] == category) {
                found = true;
                cout << "\n" << tokens[0] << endl;
                cout << "Day: " << tokens[2] << " | Time: " << tokens[3] << endl;
                cout << "Feedback: " << tokens[4] << endl;
                cout << string(100, '-') << endl;
            }
        }
        
        file.close();
        
        if (!found) {
            cout << "\nNo feedbacks found for this category." << endl;
        }
        cout << string(100, '=') << endl;
    }
};

#endif