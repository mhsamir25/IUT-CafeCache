
#include "SalesAnalytics.h"
#include "FileManager.h"
#include "TerminalSetup.h"
#include <iostream>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <limits>

using namespace std;

// Extract date from token ID (e.g., "TKN20260214-1749" -> "20260214")
string SalesAnalytics::extractDateFromToken(const string& tokenId) {
    // Token format: TKNYYYYmmdd-XXXX
    // Extract characters from position 3 to 10 (8 characters for date)
    if (tokenId.length() >= 11) {
        return tokenId.substr(3, 8);
    }
    return "";
}

// Helper: Get today's date in YYYYMMDD format
string getTodayDate() {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    
    char dateBuffer[9];
    strftime(dateBuffer, sizeof(dateBuffer), "%Y%m%d", timeinfo);
    
    return string(dateBuffer);
}

// Parse package info from token field
// Format: "Monday Breakfast Package,1,40"
// Returns pair of (packageName, quantity)
pair<string, int> SalesAnalytics::parsePackageInfo(const string& packageField) {
    // Find first comma to separate package name from quantity
    size_t firstComma = packageField.find(',');
    
    if (firstComma == string::npos) {
        return {packageField, 1}; // Default quantity to 1 if parsing fails
    }
    
    string packageName = packageField.substr(0, firstComma);
    
    // Extract quantity (between first and second comma)
    size_t secondComma = packageField.find(',', firstComma + 1);
    if (secondComma == string::npos) {
        secondComma = packageField.length();
    }
    
    string quantityStr = packageField.substr(firstComma + 1, secondComma - firstComma - 1);
    int quantity = stoi(quantityStr);
    
    return {packageName, quantity};
}

// Convert date string (YYYYMMDD) to integer for comparison
int SalesAnalytics::dateStringToInt(const string& dateStr) {
    if (dateStr.length() >= 8) {
        return stoi(dateStr.substr(0, 8));
    }
    return 0;
}

// Helper: Get day of week for a date (YYYYMMDD format)
// Returns 0=Sunday, 1=Monday, ..., 6=Saturday
int getDayOfWeek(const string& dateStr) {
    int year = stoi(dateStr.substr(0, 4));
    int month = stoi(dateStr.substr(4, 2));
    int day = stoi(dateStr.substr(6, 2));
    
    struct tm timeinfo = {0};
    timeinfo.tm_year = year - 1900;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_mday = day;
    
    time_t timestamp = mktime(&timeinfo);
    struct tm* resultTime = localtime(&timestamp);
    
    return resultTime->tm_wday; // 0=Sunday, 6=Saturday
}

// Helper: Subtract days from YYYYMMDD date
string subtractDaysFromDate(const string& dateStr, int days) {
    // Convert to time_t, subtract days, convert back
    int year = stoi(dateStr.substr(0, 4));
    int month = stoi(dateStr.substr(4, 2));
    int day = stoi(dateStr.substr(6, 2));
    
    struct tm timeinfo = {0};
    timeinfo.tm_year = year - 1900;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_mday = day;
    
    time_t timestamp = mktime(&timeinfo);
    timestamp -= (days * 86400); // 86400 seconds per day
    
    struct tm* resultTime = localtime(&timestamp);
    
    char dateBuffer[9];
    strftime(dateBuffer, sizeof(dateBuffer), "%Y%m%d", resultTime);
    
    return string(dateBuffer);
}

// Helper: Get the Saturday of the week that contains the given date
// Returns date in YYYYMMDD format of the Saturday at the start of that week
string getSaturdayOfWeek(const string& dateStr) {
    int dayOfWeek = getDayOfWeek(dateStr);
    
    // dayOfWeek: 0=Sunday, 1=Monday, ..., 6=Saturday
    // Days to subtract to get to Saturday: (dayOfWeek + 1) % 7
    // If today is Saturday (6), subtract 0 days
    // If today is Sunday (0), subtract 1 day
    // If today is Monday (1), subtract 2 days
    // If today is Friday (5), subtract 6 days
    int daysBack = (dayOfWeek + 1) % 7;
    
    return subtractDaysFromDate(dateStr, daysBack);
}

// Check if a date is within last 30 days
// We use the latest token date as reference, or current date
bool SalesAnalytics::isWithinLast30Days(const string& tokenDate, const string& referenceDate) {
    int token = dateStringToInt(tokenDate);
    int reference = dateStringToInt(referenceDate);
    
    // Simple calculation: difference should be <= 30 (assuming daily increment)
    // This is a simplified check - proper date arithmetic would use tm structures
    // For this project, we assume dates are YYYYMMDD format where difference <= 30 is acceptable
    
    // More precise approach: calculate the actual difference
    // tokenDate and referenceDate are in format YYYYMMDD
    int yearDiff = (reference / 10000) - (token / 10000);
    int monthDiff = ((reference / 100) % 100) - ((token / 100) % 100);
    int dayDiff = (reference % 100) - (token % 100);
    
    int totalDaysDiff = yearDiff * 365 + monthDiff * 30 + dayDiff;
    
    return totalDaysDiff >= 0 && totalDaysDiff <= 30;
}

// Get top 5 packages sold in the last 30 days
vector<PackageSales> SalesAnalytics::getTop5Packages() {
    vector<Token> tokens = FileManager::loadTokens();
    map<string, int> packageSalesMap; // Map to store package name -> sales count
    
    // Get today's date as reference (instead of most recent token date)
    string referenceDate = getTodayDate();
    
    // Count package sales for the last 30 days
    for (const auto& token : tokens) {
        string tokenDate = extractDateFromToken(token.getTokenId());
        
        // Check if this token is within last 30 days from today
        if (isWithinLast30Days(tokenDate, referenceDate)) {
            // Get the package info from the token
            vector<OrderItem> items = token.getItems();
            for (const auto& item : items) {
                // item.itemName is in format "Monday Breakfast Package" etc
                // Count the quantity sold
                packageSalesMap[item.itemName] += item.quantity;
            }
        }
    }
    
    // Convert map to vector of PackageSales
    vector<PackageSales> packages;
    for (const auto& pair : packageSalesMap) {
        packages.push_back({pair.first, pair.second});
    }
    
    // Sort by sales count in descending order
    sort(packages.begin(), packages.end(), [](const PackageSales& a, const PackageSales& b) {
        return a.salesCount > b.salesCount;
    });
    
    // Keep only top 5
    if (packages.size() > 5) {
        packages.erase(packages.begin() + 5, packages.end());
    }
    
    return packages;
}

// Helper: Abbreviate package name (e.g., "Wednesday Lunch Package" -> "WedLun")
string abbreviatePackageName(const string& packageName) {
    string day, meal;
    
    // Extract day abbreviation (3 chars)
    if (packageName.find("Monday") != string::npos) day = "Mon";
    else if (packageName.find("Tuesday") != string::npos) day = "Tue";
    else if (packageName.find("Wednesday") != string::npos) day = "Wed";
    else if (packageName.find("Thursday") != string::npos) day = "Thu";
    else if (packageName.find("Friday") != string::npos) day = "Fri";
    else if (packageName.find("Saturday") != string::npos) day = "Sat";
    else if (packageName.find("Sunday") != string::npos) day = "Sun";
    else return packageName; // Fallback if day not found
    
    // Extract meal abbreviation (3 chars)
    if (packageName.find("Breakfast") != string::npos) meal = "Bre";
    else if (packageName.find("Lunch") != string::npos) meal = "Lun";
    else if (packageName.find("Dinner") != string::npos) meal = "Din";
    else return packageName; // Fallback if meal not found
    
    return day + meal;
}

// Display graph for top 5 packages using asterisks
void SalesAnalytics::displayTop5PackagesGraph() {
    clearScreen();
    
    vector<PackageSales> topPackages = getTop5Packages();
    
    if (topPackages.empty()) {
        printInfo("\nNo sales data available for the last 30 days.\n");
        pauseScreen();
        return;
    }
    
    // Print header
    cout << "\n";
    printHeader("╔═══════════════════════════════════════════╗");
    printHeader("║   TOP 5 PACKAGES - LAST 30 DAYS (Sales)   ║");
    printHeader("╚═══════════════════════════════════════════╝");
    cout << "\n";
    
    // Fixed scale: 0-100 across 20 rows (each row = 5 units)
    const int TOTAL_ROWS = 20;
    const int MAX_Y = 100;
    
    // Print graph from top to bottom (20 rows)
    for (int row = TOTAL_ROWS; row >= 0; row--) {
        int yValue = row * 5; // Y value at this row (0, 5, 10, 15... 100)
        
        // Print Y-axis label only at 0, 20, 40, 60, 80, 100+
        if (yValue == 100) {
            cout << "100+ | ";
        } else if (yValue == 80) {
            cout << " 80  | ";
        } else if (yValue == 60) {
            cout << " 60  | ";
        } else if (yValue == 40) {
            cout << " 40  | ";
        } else if (yValue == 20) {
            cout << " 20  | ";
        } else if (yValue == 0) {
            cout << "  0  | ";
        } else {
            cout << "     | "; // Blank row (no label)
        }
        
        // Print bars for this height level
        for (size_t i = 0; i < topPackages.size(); ++i) {
            int barHeight = topPackages[i].salesCount;
            
            // Cap the bar height at 100 (any value > 100 will fill to the top)
            if (barHeight > 100) barHeight = 100;
            
            // At top of bar (at exact height): print dash
            if (barHeight == yValue) {
                cout << "-";
            } 
            // Below top of bar: print asterisk
            else if (barHeight > yValue) {
                cout << "*";
            } 
            // Above bar: print space
            else {
                cout << " ";
            }
            
            cout << "       "; // Space between bars (7 chars)
        }
        
        cout << "\n";
    }
    
    // Print bottom axis line
    cout << "     +" << string(topPackages.size() * 8 - 1, '-') << "\n";
    
    // Print package abbreviations on X-axis
    cout << "     | ";
    for (const auto& pkg : topPackages) {
        string abbrev = abbreviatePackageName(pkg.packageName);
        cout << setw(7) << abbrev;
    }
    cout << "\n\n";
    
    // Print legend with full package names and sales count
    printInfo("Package Details:");
    for (size_t i = 0; i < topPackages.size(); ++i) {
        stringstream ss;
        ss << "  " << (i + 1) << ". " << topPackages[i].packageName 
           << " - Sales: " << topPackages[i].salesCount;
        printInfo(ss.str());
    }
    
    cout << "\n";
    pauseScreen();
}

// Helper: Get date in DD/MM format from YYYYMMDD
string formatDateForDisplay(const string& dateStr) {
    if (dateStr.length() >= 8) {
        string day = dateStr.substr(6, 2);      // DD
        string month = dateStr.substr(4, 2);    // MM
        return day + "/" + month;
    }
    return dateStr;
}

// Get sales data for a specific package over the last 7 days
map<string, int> SalesAnalytics::getPackageSalesLast7Days(const string& packageName) {
    vector<Token> tokens = FileManager::loadTokens();
    map<string, int> dailySales; // Map of date (YYYYMMDD) -> sales count
    
    string todayDate = getTodayDate();
    
    // Initialize all 7 days with 0 sales
    for (int i = 0; i <= 6; i++) {
        string date = subtractDaysFromDate(todayDate, i);
        dailySales[date] = 0;
    }
    
    // Count sales for this package across the 7 days
    for (const auto& token : tokens) {
        string tokenDate = extractDateFromToken(token.getTokenId());
        
        // Check if token is within last 7 days
        if (isWithinLast30Days(tokenDate, todayDate)) {
            // Check if it's in the 7-day range
            bool isInRange = false;
            for (int i = 0; i <= 6; i++) {
                if (tokenDate == subtractDaysFromDate(todayDate, i)) {
                    isInRange = true;
                    break;
                }
            }
            
            if (isInRange) {
                vector<OrderItem> items = token.getItems();
                for (const auto& item : items) {
                    if (item.itemName == packageName) {
                        dailySales[tokenDate] += item.quantity;
                    }
                }
            }
        }
    }
    
    return dailySales;
}

// Display the graph for a specific package over 7 days
void SalesAnalytics::displayPackageWeeklyGraph(const string& packageName) {
    clearScreen();
    
    map<string, int> dailySales = getPackageSalesLast7Days(packageName);
    
    // Print header
    cout << "\n";
    printHeader("╔═══════════════════════════════════════════╗");
    printHeader("║   PACKAGE-WISE SALES - LAST 7 DAYS        ║");
    printHeader("╚═══════════════════════════════════════════╝");
    cout << "\n";
    printInfo("Package: " + packageName);
    cout << "\n";
    
    // Fixed scale: 0-100 across 20 rows (each row = 5 units)
    const int TOTAL_ROWS = 20;
    
    // Get dates in descending order (today to 6 days back)
    vector<string> dateList;
    string todayDate = getTodayDate();
    for (int i = 0; i <= 6; i++) {
        dateList.push_back(subtractDaysFromDate(todayDate, i));
    }
    // Reverse so oldest date is on left, today on right
    reverse(dateList.begin(), dateList.end());
    
    // Print graph from top to bottom (20 rows)
    for (int row = TOTAL_ROWS; row >= 0; row--) {
        int yValue = row * 5; // Y value at this row (0, 5, 10, 15... 100)
        
        // Print Y-axis label only at 0, 20, 40, 60, 80, 100+
        if (yValue == 100) {
            cout << "100+ | ";
        } else if (yValue == 80) {
            cout << " 80  | ";
        } else if (yValue == 60) {
            cout << " 60  | ";
        } else if (yValue == 40) {
            cout << " 40  | ";
        } else if (yValue == 20) {
            cout << " 20  | ";
        } else if (yValue == 0) {
            cout << "  0  | ";
        } else {
            cout << "     | "; // Blank row (no label)
        }
        
        // Print bars for this height level
        for (const auto& date : dateList) {
            int barHeight = dailySales[date];
            
            // Cap the bar height at 100
            if (barHeight > 100) barHeight = 100;
            
            // At top of bar: print dash
            if (barHeight == yValue) {
                cout << "-";
            } 
            // Below top of bar: print asterisk
            else if (barHeight > yValue) {
                cout << "*";
            } 
            // Above bar: print space
            else {
                cout << " ";
            }
            
            cout << "     "; // Space between bars (5 chars - aligns with setw(6))
        }
        
        cout << "\n";
    }
    
    // Print bottom axis line
    cout << "     +" << string(dateList.size() * 6 - 1, '-') << "\n";
    
    // Print dates on X-axis (DD/MM format)
    cout << "     | ";
    for (const auto& date : dateList) {
        string displayDate = formatDateForDisplay(date);
        cout << setw(6) << displayDate;
    }
    cout << "\n\n";
    
    // Print sales summary
    printInfo("Daily Sales Summary:");
    for (const auto& date : dateList) {
        string displayDate = formatDateForDisplay(date);
        stringstream ss;
        ss << "  " << displayDate << ": " << dailySales[date] << " units sold";
        printInfo(ss.str());
    }
    
    cout << "\n";
    pauseScreen();
}

// Helper: Add days to YYYYMMDD date
string addDaysToDate(const string& dateStr, int days) {
    int year = stoi(dateStr.substr(0, 4));
    int month = stoi(dateStr.substr(4, 2));
    int day = stoi(dateStr.substr(6, 2));
    
    struct tm timeinfo = {0};
    timeinfo.tm_year = year - 1900;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_mday = day;
    
    time_t timestamp = mktime(&timeinfo);
    timestamp += (days * 86400); // 86400 seconds per day
    
    struct tm* resultTime = localtime(&timestamp);
    
    char dateBuffer[9];
    strftime(dateBuffer, sizeof(dateBuffer), "%Y%m%d", resultTime);
    
    return string(dateBuffer);
}

// Get sales data for a specific package over the last 30 days (grouped by week)
// Returns a map of week_range (e.g., "22-28/1") -> total sales count
map<string, pair<int, int>> SalesAnalytics::getPackageSalesLast30DaysWeekly(const string& packageName) {
    vector<Token> tokens = FileManager::loadTokens();
    vector<int> weeklySales(5, 0); // Vector to store sales count for 5 weeks
    vector<pair<string, string>> weekRanges(5); // Store start and end dates for 5 weeks
    
    string todayDate = getTodayDate();
    
    // Build week ranges: Start from 30 days ago, then add 7 days for each week
    // This ensures exactly 30 days coverage
    string thirtyDaysAgo = subtractDaysFromDate(todayDate, 29); // 29 days back = 30 day period including today
    
    for (int week = 0; week < 5; week++) {
        // Calculate week start by adding (week * 7) days to thirtyDaysAgo
        string weekStartDate = addDaysToDate(thirtyDaysAgo, week * 7);
        
        // Calculate week end
        string weekEndDate;
        if (week < 4) {
            // First 4 weeks: exactly 7 days each (ending 6 days after start)
            weekEndDate = addDaysToDate(weekStartDate, 6); // Add 6 days
        } else {
            // Last week: goes until today
            weekEndDate = todayDate;
        }
        
        weekRanges[week] = {weekStartDate, weekEndDate};
    }
    
    // Count sales for this package across the 30 days, grouped by weekly chunks
    for (const auto& token : tokens) {
        string tokenDate = extractDateFromToken(token.getTokenId());
        
        // Check if token is within last 30 days
        if (isWithinLast30Days(tokenDate, todayDate)) {
            // Find which week this token belongs to
            int tokenInt = dateStringToInt(tokenDate);
            int startInt = dateStringToInt(thirtyDaysAgo);
            
            int yearDiff = (tokenInt / 10000) - (startInt / 10000);
            int monthDiff = ((tokenInt / 100) % 100) - ((startInt / 100) % 100);
            int dayDiff = (tokenInt % 100) - (startInt % 100);
            
            int totalDaysDiff = yearDiff * 365 + monthDiff * 30 + dayDiff;
            
            if (totalDaysDiff >= 0 && totalDaysDiff <= 29) {
                int weekIndex = totalDaysDiff / 7;
                if (weekIndex > 4) weekIndex = 4; // Cap at week 4
                
                vector<OrderItem> items = token.getItems();
                for (const auto& item : items) {
                    if (item.itemName == packageName) {
                        weeklySales[weekIndex] += item.quantity;
                    }
                }
            }
        }
    }
    
    // Convert to map with week range strings and sales data
    map<string, pair<int, int>> result; // week_range -> (salesCount, weekIndex)
    
    for (int week = 0; week < 5; week++) {
        string startDateStr = weekRanges[week].first;
        string endDateStr = weekRanges[week].second;
        
        // Create week range label (e.g., "22-28/1")
        string startDay = startDateStr.substr(6, 2);      // DD
        string startMonth = startDateStr.substr(4, 2);    // MM
        string endDay = endDateStr.substr(6, 2);          // DD
        string endMonth = endDateStr.substr(4, 2);        // MM
        
        string weekRange;
        if (startMonth == endMonth) {
            // Same month: "22-28/1"
            weekRange = startDay + "-" + endDay + "/" + endMonth;
        } else {
            // Different months: "29/1-4/2"
            weekRange = startDay + "/" + startMonth + "-" + endDay + "/" + endMonth;
        }
        
        // Cap sales at 100 for display consistency
        int cappedSales = weeklySales[week];
        if (cappedSales > 100) cappedSales = 100;
        
        result[weekRange] = {cappedSales, week}; // Store capped sales and week index
    }
    
    return result;
}

// Display the graph for a specific package over 30 days (weekly intervals)
void SalesAnalytics::displayPackageMonthlyGraph(const string& packageName) {
    clearScreen();
    
    map<string, pair<int, int>> weeklySales = getPackageSalesLast30DaysWeekly(packageName);
    
    // Print header
    cout << "\n";
    printHeader("╔═══════════════════════════════════════════╗");
    printHeader("║   PACKAGE-WISE SALES - LAST 30 DAYS       ║");
    printHeader("║        (Weekly Total Breakdown)           ║");
    printHeader("╚═══════════════════════════════════════════╝");
    cout << "\n";
    printInfo("Title: " + packageName);
    cout << "\n";
    
    // Fixed scale: 0-100 across 20 rows (each row = 5 units)
    const int TOTAL_ROWS = 20;
    const int COL_WIDTH = 12; // Width of each column (increased from 8 for better spacing)
    
    // Convert map to vector for ordered display (week 0 to 4 = oldest to newest)
    vector<pair<string, int>> weeklyData; // (week_range, sales)
    for (int week = 0; week < 5; week++) {
        for (const auto& entry : weeklySales) {
            if (entry.second.second == week) { // Match week index
                weeklyData.push_back({entry.first, entry.second.first});
                break;
            }
        }
    }
    
    // Print graph from top to bottom (20 rows)
    for (int row = TOTAL_ROWS; row >= 0; row--) {
        int yValue = row * 5; // Y value at this row (0, 5, 10, 15... 100)
        
        // Print Y-axis label only at 0, 20, 40, 60, 80, 100+
        if (yValue == 100) {
            cout << "100+ | ";
        } else if (yValue == 80) {
            cout << " 80  | ";
        } else if (yValue == 60) {
            cout << " 60  | ";
        } else if (yValue == 40) {
            cout << " 40  | ";
        } else if (yValue == 20) {
            cout << " 20  | ";
        } else if (yValue == 0) {
            cout << "  0  | ";
        } else {
            cout << "     | "; // Blank row (no label)
        }
        
        // Print bars for this height level
        for (const auto& week : weeklyData) {
            int barHeight = week.second;
            
            // Cap the bar height at 100
            if (barHeight > 100) barHeight = 100;
            
            // Center the bar symbol within the column using exactly COL_WIDTH characters
            int left_pad = (COL_WIDTH - 1) / 2;  // 5 spaces for COL_WIDTH=12
            int right_pad = COL_WIDTH - 1 - left_pad;  // 6 spaces for COL_WIDTH=12
            
            // Print left padding
            for (int p = 0; p < left_pad; p++) cout << " ";
            
            // At top of bar: print dash
            if (barHeight == yValue) {
                cout << "-";
            } 
            // Below top of bar: print asterisk
            else if (barHeight > yValue) {
                cout << "*";
            } 
            // Above bar: print space
            else {
                cout << " ";
            }
            
            // Print right padding
            for (int p = 0; p < right_pad; p++) cout << " ";
        }
        
        cout << "\n";
    }
    
    // Print bottom axis line
    cout << "     +" << string(weeklyData.size() * COL_WIDTH - 1, '-') << "\n";
    
    // Print week ranges on X-axis
    cout << "     | ";
    for (const auto& week : weeklyData) {
        cout << setw(COL_WIDTH) << week.first;
    }
    cout << "\n\n";
    
    // Print sales summary
    printInfo("Weekly Sales Summary:");
    for (const auto& week : weeklyData) {
        stringstream ss;
        ss << "  " << week.first << ": " << week.second << " units sold";
        printInfo(ss.str());
    }
    
    cout << "\n";
    pauseScreen();
}

// Display package selection submenu
void SalesAnalytics::displayPackageSelectionMenu() {
    clearScreen();
    cout << "\n";
    printHeader("╔════════════════════════════════════════╗");
    printHeader("║     PACKAGE-WISE SALES ANALYSIS        ║");
    printHeader("╚════════════════════════════════════════╝");
    
    printInfo("\nSelect Day:");
    printInfo("1. Monday");
    printInfo("2. Tuesday");
    printInfo("3. Wednesday");
    printInfo("4. Thursday");
    printInfo("5. Friday");
    printInfo("6. Saturday");
    printInfo("7. Sunday");
    
    int dayChoice;
    printPrompt("\nEnter day (1-7): ");
    cin >> dayChoice;
    
    if (dayChoice < 1 || dayChoice > 7) {
        printError("\nInvalid day selection.");
        pauseScreen();
        return;
    }
    
    vector<string> days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    string selectedDay = days[dayChoice - 1];
    
    cout << "\n";
    printInfo("\nSelect Meal Type:");
    printInfo("1. Breakfast");
    printInfo("2. Lunch");
    printInfo("3. Dinner");
    
    int mealChoice;
    printPrompt("\nEnter meal type (1-3): ");
    cin >> mealChoice;
    
    if (mealChoice < 1 || mealChoice > 3) {
        printError("\nInvalid meal selection.");
        pauseScreen();
        return;
    }
    
    vector<string> meals = {"Breakfast", "Lunch", "Dinner"};
    string selectedMeal = meals[mealChoice - 1];
    
    cout << "\n";
    printInfo("\nSelect Time Range:");
    printInfo("1. Last 7 Days (Weekly)");
    printInfo("2. Last 30 Days (Monthly)");
    
    int timeChoice;
    printPrompt("\nEnter choice (1-2): ");
    cin >> timeChoice;
    
    if (timeChoice == 1) {
        // Build package name and display graph
        string packageName = selectedDay + " " + selectedMeal + " Package";
        displayPackageWeeklyGraph(packageName);
    } else if (timeChoice == 2) {
        // Build package name and display 30-day graph
        string packageName = selectedDay + " " + selectedMeal + " Package";
        displayPackageMonthlyGraph(packageName);
    } else {
        printError("\nInvalid choice.");
        pauseScreen();
    }
}

// Handle package analysis operations
void SalesAnalytics::handlePackageAnalysisOperations() {
    displayPackageSelectionMenu();
}

// Display Sales Analytics submenu
void SalesAnalytics::displaySalesAnalyticsMenu() {
    clearScreen();
    cout << "\n";
    printHeader("╔════════════════════════════════════════╗");
    printHeader("║       SALES ANALYTICS DASHBOARD        ║");
    printHeader("╚════════════════════════════════════════╝");
    printInfo("\n1. View Top 5 Packages (Last 30 Days)");
    printInfo("2. Package-wise Sales Analysis");
    printInfo("3. Back to Admin Menu");
    printPrompt("\nEnter your choice: ");
}

// Handle Sales Analytics menu operations
void SalesAnalytics::handleSalesAnalyticsOperations() {
    string choice;
    
    while (true) {
        displaySalesAnalyticsMenu();
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (choice == "1") {
            displayTop5PackagesGraph();
        } else if (choice == "2") {
            handlePackageAnalysisOperations();
        } else if (choice == "3") {
            break; // Return to admin menu
        } else {
            printError("\nInvalid choice. Please try again.\n");
            pauseScreen();
        }
    }
}

