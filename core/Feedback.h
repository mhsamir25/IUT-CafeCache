#ifndef FEEDBACK_H
#define FEEDBACK_H

#include <string>
#include <vector>
#include <ctime>

using namespace std;

class Feedback {
private:
    string feedbackId;
    string userId;
    string tokenId;    // Reference to the order (token) being reviewed
    int rating;        // 1-5
    string remark;     // Comment/feedback text
    time_t timestamp;

public:
    Feedback();
    Feedback(string uid, string tId, int rate, string rem);
    
    string getFeedbackId() const;
    string getUserId() const;
    string getTokenId() const;
    int getRating() const;
    string getRemark() const;
    time_t getTimestamp() const;
    
    string toFileString() const;
    static Feedback fromFileString(const string& line);
    
    void display() const;
};

// Helper structure for storing feedback statistics per token
struct TokenFeedback {
    string tokenId;
    double averageRating;
    int totalRatings;
    vector<Feedback> allFeedbacks;
};

#endif
