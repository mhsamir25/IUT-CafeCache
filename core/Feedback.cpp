#include "Feedback.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include "TerminalSetup.h"
#include "TokenGenerator.h"

using namespace std;

Feedback::Feedback() 
    : feedbackId(""), userId(""), tokenId(""), rating(0), remark(""), timestamp(time(nullptr)) {}

Feedback::Feedback(string uid, string tId, int rate, string rem)
    : feedbackId(TokenGenerator::generateRequestId()), userId(uid), tokenId(tId), 
      rating(rate), remark(rem), timestamp(time(nullptr)) {}

string Feedback::getFeedbackId() const { return feedbackId; }
string Feedback::getUserId() const { return userId; }
string Feedback::getTokenId() const { return tokenId; }
int Feedback::getRating() const { return rating; }
string Feedback::getRemark() const { return remark; }
time_t Feedback::getTimestamp() const { return timestamp; }

string Feedback::toFileString() const {
    return feedbackId + "|" + userId + "|" + tokenId + "|" + to_string(rating) + "|" + remark + "|" + to_string(timestamp);
}

Feedback Feedback::fromFileString(const string& line) {
    stringstream ss(line);
    string fbId, uid, tId, ratingStr, rem, timeStr;
    
    getline(ss, fbId, '|');
    getline(ss, uid, '|');
    getline(ss, tId, '|');
    getline(ss, ratingStr, '|');
    getline(ss, rem, '|');
    getline(ss, timeStr, '|');
    
    Feedback fb(uid, tId, stoi(ratingStr), rem);
    fb.feedbackId = fbId;
    
    return fb;
}

void Feedback::display() const {
    printLabelValue("User ID: ", userId);
    printLabelValue("Token ID: ", tokenId);
    printLabelValue("Rating: ", to_string(rating) + "/5");
    printLabelValue("Remark: ", remark);
    time_t t = timestamp;
    printLabelValue("Time: ", string(ctime(&t)));
    printSeparator();
}
