#ifndef SALESANALYTICS_H
#define SALESANALYTICS_H

#include <string>
#include <vector>
#include <map>

using namespace std;

// Forward declarations for utility functions from main.cpp
void clearScreen();
void pauseScreen();

// Structure to hold package sales data
struct PackageSales {
    string packageName;
    int salesCount;
};

class SalesAnalytics {
private:
    // Helper: Extract date from token ID (e.g., "TKN20260214-1749" -> "20260214")
    static string extractDateFromToken(const string& tokenId);
    
    // Helper: Parse package info from token line (6th field)
    // Format: "Monday Breakfast Package,1,40" where 1 is the quantity
    static pair<string, int> parsePackageInfo(const string& packageField);
    
    // Helper: Check if a date is within last 30 days from a reference date
    static bool isWithinLast30Days(const string& tokenDate, const string& referenceDate);
    
    // Helper: Convert date string (YYYYMMDD) to comparable integer
    static int dateStringToInt(const string& dateStr);
    
public:
    // Get top 5 packages sold in the last 30 days
    // Returns a vector of PackageSales sorted by sales count (descending)
    static vector<PackageSales> getTop5Packages();
    
    // Get sales data for a specific package over the last 7 days
    // Returns a map of date (YYYY-MM-DD) -> sales count
    static map<string, int> getPackageSalesLast7Days(const string& packageName);
    
    // Display the graph for a specific package over 7 days
    static void displayPackageWeeklyGraph(const string& packageName);
    
    // Get sales data for a specific package over the last 30 days (grouped by week)
    // Returns a map of week_range (e.g., "22-28/1") -> total sales count
    static map<string, pair<int, int>> getPackageSalesLast30DaysWeekly(const string& packageName);
    
    // Display the graph for a specific package over 30 days (weekly intervals)
    static void displayPackageMonthlyGraph(const string& packageName);
    
    // Display package selection submenu (day + meal type)
    static void displayPackageSelectionMenu();
    
    // Handle package selection and graph display
    static void handlePackageAnalysisOperations();
    
    // Display the graph for top 5 packages
    static void displayTop5PackagesGraph();
    
    // Display Sales Analytics submenu
    static void displaySalesAnalyticsMenu();
    
    // Handle Sales Analytics menu operations
    static void handleSalesAnalyticsOperations();
};

#endif

