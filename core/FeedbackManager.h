#ifndef FEEDBACKMANAGER_H
#define FEEDBACKMANAGER_H

#include <string>
#include <vector>
#include "Feedback.h"

using namespace std;

class FeedbackManager {
public:
    // Submit feedback for a specific order (token)
    static bool submitFeedback(string userId, string tokenId, int rating, string remark);
    
    // Get average rating and count for a specific token
    static pair<double, int> getTokenRating(string tokenId);
    
    // Get all feedback for a specific token
    static vector<Feedback> getFeedbackForToken(string tokenId);
    
    // Check if user has already given feedback for a token
    static bool hasFeedbackForToken(string userId, string tokenId);
    
    // Get all feedback submitted by a user (for their order history page)
    static vector<Feedback> getUserFeedback(string userId);
    
    // NEW: Get average rating and count for a specific day's meal (e.g., Monday Breakfast)
    static pair<double, int> getDayMealRating(string dayOfWeek, string mealType);
    
    // NEW: Get all feedback for a specific day's meal
    static vector<Feedback> getFeedbackForDayMeal(string dayOfWeek, string mealType);
};

#endif
