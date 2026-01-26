#include "FeedbackManager.h"
#include "FileManager.h"
#include <iostream>
#include "TerminalSetup.h"

using namespace std;

bool FeedbackManager::submitFeedback(string userId, string tokenId, int rating, string remark) {
    if (rating < 1 || rating > 5) {
        printError("Error: Rating must be between 1 and 5.");
        return false;
    }
    
    if (remark.empty()) {
        printError("Error: Remark cannot be empty.");
        return false;
    }
    
    Feedback feedback(userId, tokenId, rating, remark);
    return FileManager::addFeedback(feedback);
}

pair<double, int> FeedbackManager::getTokenRating(string tokenId) {
    vector<Feedback> allFeedbacks = FileManager::loadFeedback();
    double totalRating = 0.0;
    int count = 0;
    
    for (const auto& fb : allFeedbacks) {
        if (fb.getTokenId() == tokenId) {
            totalRating += fb.getRating();
            count++;
        }
    }
    
    if (count == 0) {
        return {0.0, 0};
    }
    
    double average = totalRating / count;
    return {average, count};
}

vector<Feedback> FeedbackManager::getFeedbackForToken(string tokenId) {
    vector<Feedback> allFeedbacks = FileManager::loadFeedback();
    vector<Feedback> tokenFeedbacks;
    
    for (const auto& fb : allFeedbacks) {
        if (fb.getTokenId() == tokenId) {
            tokenFeedbacks.push_back(fb);
        }
    }
    
    return tokenFeedbacks;
}

bool FeedbackManager::hasFeedbackForToken(string userId, string tokenId) {
    vector<Feedback> allFeedbacks = FileManager::loadFeedback();
    
    for (const auto& fb : allFeedbacks) {
        if (fb.getUserId() == userId && fb.getTokenId() == tokenId) {
            return true;
        }
    }
    
    return false;
}

vector<Feedback> FeedbackManager::getUserFeedback(string userId) {
    vector<Feedback> allFeedbacks = FileManager::loadFeedback();
    vector<Feedback> userFeedbacks;
    
    for (const auto& fb : allFeedbacks) {
        if (fb.getUserId() == userId) {
            userFeedbacks.push_back(fb);
        }
    }
    
    return userFeedbacks;
}

pair<double, int> FeedbackManager::getDayMealRating(string dayOfWeek, string mealType) {
    // Get ratings for a specific day's meal by matching tokens with that day/meal
    vector<Token> allTokens = FileManager::loadTokens();
    vector<Feedback> allFeedbacks = FileManager::loadFeedback();
    
    double totalRating = 0.0;
    int count = 0;
    
    // Find all tokens for this day/meal
    for (const auto& token : allTokens) {
        auto [day, meal] = token.extractDayAndMeal();
        
        if (day == dayOfWeek && meal == mealType) {
            // This token matches the day/meal; find all feedback for it
            for (const auto& fb : allFeedbacks) {
                if (fb.getTokenId() == token.getTokenId()) {
                    totalRating += fb.getRating();
                    count++;
                }
            }
        }
    }
    
    if (count == 0) {
        return {0.0, 0};
    }
    
    double average = totalRating / count;
    return {average, count};
}

vector<Feedback> FeedbackManager::getFeedbackForDayMeal(string dayOfWeek, string mealType) {
    vector<Token> allTokens = FileManager::loadTokens();
    vector<Feedback> allFeedbacks = FileManager::loadFeedback();
    vector<Feedback> dayMealFeedbacks;
    
    for (const auto& token : allTokens) {
        auto [day, meal] = token.extractDayAndMeal();
        
        if (day == dayOfWeek && meal == mealType) {
            for (const auto& fb : allFeedbacks) {
                if (fb.getTokenId() == token.getTokenId()) {
                    dayMealFeedbacks.push_back(fb);
                }
            }
        }
    }
    
    return dayMealFeedbacks;
}
