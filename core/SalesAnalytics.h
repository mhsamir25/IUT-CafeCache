#ifndef SalesAnalytics_H
#define SalesAnalytics_H

#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>

using namespace std;

struct PackageStats {
    string packageName;
    double avgRating;
    int totalOrders;
    vector<int> ratings;
    vector<time_t> timestamps;
};

struct DailyData {
    string dateStr;
    double avgRating;
    int orders;
};

class SalesAnalytics {
private:
    // Map: packageName -> [(rating, timestamp)]
    map<string, vector<pair<int, time_t>>> packageData;
    
    // All packages in order
    vector<string> allPackages = {
        "Monday Breakfast", "Monday Lunch", "Monday Dinner",
        "Tuesday Breakfast", "Tuesday Lunch", "Tuesday Dinner",
        "Wednesday Breakfast", "Wednesday Lunch", "Wednesday Dinner",
        "Thursday Breakfast", "Thursday Lunch", "Thursday Dinner",
        "Friday Breakfast", "Friday Lunch", "Friday Dinner",
        "Saturday Breakfast", "Saturday Lunch", "Saturday Dinner",
        "Sunday Breakfast", "Sunday Lunch", "Sunday Dinner"
    };
    
    bool isInRange(time_t timestamp, int days);
    string extractPackageFromToken(string tokenId, string comment);
    void plotBarGraph(vector<PackageStats> stats, string title);
    void plotBarGraphDaily(vector<DailyData> dailyStats, string title);
    void plotBarGraphWeekly(vector<DailyData> weeklyStats, string title);
    void plotTopSellingPackages(vector<PackageStats> topStats);
    vector<DailyData> getDailyBreakdown(string package, int days);
    vector<DailyData> getWeeklyBreakdown(string package, int days);
    
public:
    SalesAnalytics();
    
    // Load feedback data
    void loadFeedbackData();
    
    // Display methods
    void displayTopSellingPackages();
    void displayPackageAnalysis();
    
    // Data methods
    vector<PackageStats> getTopPackages(int topN, int timeDays);
    PackageStats getPackageStats(string package, int timeDays);
    vector<PackageStats> getAllPackagesStats(int timeDays);
};

// Helper function
string fixed_to_string(double val, int precision);

#endif
