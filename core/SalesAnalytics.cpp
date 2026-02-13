#include "SalesAnalytics.h"
#include "TerminalSetup.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>

// External function declarations from main.cpp
extern void pauseScreen();
extern void clearScreen();

SalesAnalytics::SalesAnalytics() {}

void SalesAnalytics::loadFeedbackData() {
    // Clear existing data
    packageData.clear();
    
    // Initialize all packages with empty data
    for (const string& pkg : allPackages) {
        packageData[pkg] = vector<pair<int, time_t>>();
    }
    
    ifstream file("feedback.txt");
    if (!file.is_open()) return;
    
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        // Parse: REQ20260124-1001|230042101|TKN20251218-1001|5|Comment|1769229099
        size_t pos = 0;
        vector<string> parts;
        
        for (int i = 0; i < 6; i++) {
            size_t nextPos = line.find('|', pos);
            if (nextPos == string::npos) nextPos = line.length();
            parts.push_back(line.substr(pos, nextPos - pos));
            pos = nextPos + 1;
        }
        
        if (parts.size() < 6) continue;
        
        string tokenId = parts[2];          // TKN20251218-1001
        int rating = stoi(parts[3]);        // 5
        string comment = parts[4];          // Comment text
        time_t timestamp = stol(parts[5]); // timestamp
        
        // Extract package name from token/comment
        string package = extractPackageFromToken(tokenId, comment);
        
        if (!package.empty() && packageData.find(package) != packageData.end()) {
            packageData[package].push_back({rating, timestamp});
        }
    }
    
    file.close();
}

bool SalesAnalytics::isInRange(time_t timestamp, int days) {
    if (days == 0) return true;  // All-time
    
    time_t now = time(nullptr);
    time_t rangeStart = now - (days * 86400);  // 86400 seconds per day
    
    return timestamp >= rangeStart;
}

string SalesAnalytics::extractPackageFromToken(string tokenId, string comment) {
    // For demo: extract package from comment if available, else use sequence
    // In real implementation, this would be linked to Token class data
    
    // Try to detect from comment text
    string lower = comment;
    transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    // Day detection
    string day;
    if (lower.find("monday") != string::npos) day = "Monday";
    else if (lower.find("tuesday") != string::npos) day = "Tuesday";
    else if (lower.find("wednesday") != string::npos) day = "Wednesday";
    else if (lower.find("thursday") != string::npos) day = "Thursday";
    else if (lower.find("friday") != string::npos) day = "Friday";
    else if (lower.find("saturday") != string::npos) day = "Saturday";
    else if (lower.find("sunday") != string::npos) day = "Sunday";
    
    // Meal detection
    string meal;
    if (lower.find("breakfast") != string::npos || lower.find("paratha") != string::npos) meal = "Breakfast";
    else if (lower.find("lunch") != string::npos || lower.find("rice") != string::npos || lower.find("curry") != string::npos) meal = "Lunch";
    else if (lower.find("dinner") != string::npos || lower.find("khichuri") != string::npos || lower.find("fried") != string::npos) meal = "Dinner";
    
    if (!day.empty() && !meal.empty()) {
        return day + " " + meal;
    }
    
    // Fallback based on token sequence pattern
    return "Monday Breakfast";
}

vector<PackageStats> SalesAnalytics::getAllPackagesStats(int timeDays) {
    vector<PackageStats> stats;
    
    for (const string& packageName : allPackages) {
        if (packageData.find(packageName) == packageData.end()) continue;
        
        vector<int> filtered_ratings;
        vector<time_t> filtered_timestamps;
        
        for (auto& [rating, timestamp] : packageData[packageName]) {
            if (isInRange(timestamp, timeDays)) {
                filtered_ratings.push_back(rating);
                filtered_timestamps.push_back(timestamp);
            }
        }
        
        if (filtered_ratings.empty()) {
            stats.push_back({packageName, 0.0, 0, filtered_ratings, filtered_timestamps});
            continue;
        }
        
        double sum = 0;
        for (int r : filtered_ratings) sum += r;
        double avg = sum / filtered_ratings.size();
        
        stats.push_back({packageName, avg, (int)filtered_ratings.size(), filtered_ratings, filtered_timestamps});
    }
    
    return stats;
}

vector<PackageStats> SalesAnalytics::getTopPackages(int topN, int timeDays) {
    vector<PackageStats> allStats = getAllPackagesStats(timeDays);
    
    // Filter out zero-order packages and sort
    vector<PackageStats> filtered;
    for (auto& s : allStats) {
        if (s.totalOrders > 0) {
            filtered.push_back(s);
        }
    }
    
    // Sort by average rating descending
    sort(filtered.begin(), filtered.end(), 
         [](const PackageStats& a, const PackageStats& b) {
             return a.avgRating > b.avgRating;
         });
    
    if (filtered.size() > topN) filtered.resize(topN);
    return filtered;
}

PackageStats SalesAnalytics::getPackageStats(string package, int timeDays) {
    if (packageData.find(package) == packageData.end()) {
        return {package, 0.0, 0, {}, {}};
    }
    
    vector<int> filtered_ratings;
    vector<time_t> filtered_timestamps;
    
    for (auto& [rating, timestamp] : packageData[package]) {
        if (isInRange(timestamp, timeDays)) {
            filtered_ratings.push_back(rating);
            filtered_timestamps.push_back(timestamp);
        }
    }
    
    if (filtered_ratings.empty()) {
        return {package, 0.0, 0, filtered_ratings, filtered_timestamps};
    }
    
    double sum = 0;
    for (int r : filtered_ratings) sum += r;
    double avg = sum / filtered_ratings.size();
    
    return {package, avg, (int)filtered_ratings.size(), filtered_ratings, filtered_timestamps};
}

vector<DailyData> SalesAnalytics::getDailyBreakdown(string package, int days) {
    vector<DailyData> dailyStats;
    
    if (packageData.find(package) == packageData.end()) {
        return dailyStats;
    }
    
    // Create daily buckets for all days in range
    map<string, vector<int>> dayBuckets;
    time_t now = time(nullptr);
    time_t rangeStart = now - (days * 86400);
    
    // Fill all data points
    for (auto& [rating, timestamp] : packageData[package]) {
        if (timestamp < rangeStart) continue;
        
        struct tm* timeinfo = localtime(&timestamp);
        char buffer[11];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
        string dateStr(buffer);
        
        dayBuckets[dateStr].push_back(rating);
    }
    
    // Create entries for all days in range (even empty ones)
    for (int i = days - 1; i >= 0; i--) {
        time_t dayTime = now - (i * 86400);
        struct tm* timeinfo = localtime(&dayTime);
        char buffer[11];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
        string dateStr(buffer);
        
        if (dayBuckets.find(dateStr) != dayBuckets.end() && !dayBuckets[dateStr].empty()) {
            vector<int>& ratings = dayBuckets[dateStr];
            double sum = 0;
            for (int r : ratings) sum += r;
            double avg = sum / ratings.size();
            dailyStats.push_back({dateStr, avg, (int)ratings.size()});
        } else {
            // Empty day with no data
            dailyStats.push_back({dateStr, 0.0, 0});
        }
    }
    
    return dailyStats;
}

vector<DailyData> SalesAnalytics::getWeeklyBreakdown(string package, int days) {
    vector<DailyData> weeklyStats;
    
    if (packageData.find(package) == packageData.end()) {
        return weeklyStats;
    }
    
    // Create weekly buckets
    map<int, vector<int>> weekBuckets;
    
    time_t now = time(nullptr);
    time_t rangeStart = now - (days * 86400);
    
    for (auto& [rating, timestamp] : packageData[package]) {
        if (timestamp < rangeStart) continue;
        
        int secondsAgo = now - timestamp;
        int daysAgo = secondsAgo / 86400;
        int week = daysAgo / 7;
        
        if (week > 3) week = 3;
        
        weekBuckets[week].push_back(rating);
    }
    
    // Convert to WeeklyData - include all 4 weeks even if empty
    for (int w = 3; w >= 0; w--) {
        string weekLabel = "Week " + to_string(4 - w);
        
        if (weekBuckets.find(w) != weekBuckets.end() && !weekBuckets[w].empty()) {
            vector<int>& ratings = weekBuckets[w];
            double sum = 0;
            for (int r : ratings) sum += r;
            double avg = sum / ratings.size();
            weeklyStats.push_back({weekLabel, avg, (int)ratings.size()});
        } else {
            // Empty week with no data
            weeklyStats.push_back({weekLabel, 0.0, 0});
        }
    }
    
    return weeklyStats;
}

void SalesAnalytics::plotBarGraphDaily(vector<DailyData> dailyStats, string title) {
    printHeader("\n" + title);
    
    if (dailyStats.empty()) {
        printError("\nNo data available.\n");
        return;
    }
    
    double maxRating = 0;
    for (auto& d : dailyStats) {
        maxRating = max(maxRating, d.avgRating);
    }
    
    if (maxRating < 1) maxRating = 1;
    maxRating = ceil(maxRating * 2) / 2.0;
    
    int graphHeight = 10;
    int barWidth = 8;
    
    cout << "\n";
    
    cout << BOLD << CYAN;
    cout << setw(4) << fixed << setprecision(1) << maxRating << " +";
    for (size_t i = 0; i < dailyStats.size(); i++) {
        cout << string(barWidth, '-');
        if (i < dailyStats.size() - 1) cout << "+";
    }
    cout << "+" << RESET << "\n";
    
    for (int level = graphHeight; level > 0; level--) {
        double levelValue = (level / (double)graphHeight) * maxRating;
        
        cout << setw(4) << fixed << setprecision(1) << levelValue << " " << CYAN << "|" << RESET;
        
        for (size_t i = 0; i < dailyStats.size(); i++) {
            double barHeight = (dailyStats[i].avgRating / maxRating) * graphHeight;
            
            // Render bar at this level
            if (level <= barHeight) {
                // Inside bar - check if at top
                if (level > barHeight - 0.5) {  // At top of bar
                    cout << GREEN << "  ____  " << RESET;
                } else {
                    cout << GREEN << "  |  |  " << RESET;
                }
            } else {
                // Empty space above bar
                cout << "        ";
            }
            
            if (i < dailyStats.size() - 1) cout << CYAN << "|" << RESET;
        }
        
        cout << CYAN << "|" << RESET << "\n";
    }
    
    cout << "      " << CYAN << "+";
    for (size_t i = 0; i < dailyStats.size(); i++) {
        cout << string(barWidth, '-');
        if (i < dailyStats.size() - 1) cout << "+";
    }
    cout << "+" << RESET << "\n";
    
    cout << "      ";
    for (size_t i = 0; i < dailyStats.size(); i++) {
        cout << setw(8) << dailyStats[i].dateStr;
        if (i < dailyStats.size() - 1) cout << " ";
    }
    cout << "\n\n";
    
    cout << BOLD << YELLOW << "Daily Details:" << RESET << "\n";
    for (size_t i = 0; i < dailyStats.size(); i++) {
        cout << dailyStats[i].dateStr << ":  " << fixed << setprecision(2) << dailyStats[i].avgRating 
             << " rating  (" << dailyStats[i].orders << " orders)\n";
    }
    cout << "\n" << flush;
}

void SalesAnalytics::plotBarGraphWeekly(vector<DailyData> weeklyStats, string title) {
    printHeader("\n" + title);
    
    if (weeklyStats.empty()) {
        printError("\nNo data available.\n");
        return;
    }
    
    double maxRating = 0;
    for (auto& w : weeklyStats) {
        maxRating = max(maxRating, w.avgRating);
    }
    
    if (maxRating < 1) maxRating = 1;
    maxRating = ceil(maxRating * 2) / 2.0;
    
    int graphHeight = 10;
    int barWidth = 8;
    
    cout << "\n";
    
    cout << BOLD << CYAN;
    cout << setw(4) << fixed << setprecision(1) << maxRating << " +";
    for (size_t i = 0; i < weeklyStats.size(); i++) {
        cout << string(barWidth, '-');
        if (i < weeklyStats.size() - 1) cout << "+";
    }
    cout << "+" << RESET << "\n";
    
    for (int level = graphHeight; level > 0; level--) {
        double levelValue = (level / (double)graphHeight) * maxRating;
        
        cout << setw(4) << fixed << setprecision(1) << levelValue << " " << CYAN << "|" << RESET;
        
        for (size_t i = 0; i < weeklyStats.size(); i++) {
            double barHeight = (weeklyStats[i].avgRating / maxRating) * graphHeight;
            
            // Render bar at this level
            if (level <= barHeight) {
                // Inside bar - check if at top
                if (level > barHeight - 0.5) {  // At top of bar
                    cout << GREEN << "  ____  " << RESET;
                } else {
                    cout << GREEN << "  |  |  " << RESET;
                }
            } else {
                // Empty space above bar
                cout << "        ";
            }
            
            if (i < weeklyStats.size() - 1) cout << CYAN << "|" << RESET;
        }
        
        cout << CYAN << "|" << RESET << "\n";
    }
    
    cout << "      " << CYAN << "+";
    for (size_t i = 0; i < weeklyStats.size(); i++) {
        cout << string(barWidth, '-');
        if (i < weeklyStats.size() - 1) cout << "+";
    }
    cout << "+" << RESET << "\n";
    
    cout << "      ";
    for (size_t i = 0; i < weeklyStats.size(); i++) {
        cout << setw(8) << weeklyStats[i].dateStr;
        if (i < weeklyStats.size() - 1) cout << " ";
    }
    cout << "\n\n";
    
    cout << BOLD << YELLOW << "Weekly Details:" << RESET << "\n";
    for (size_t i = 0; i < weeklyStats.size(); i++) {
        cout << weeklyStats[i].dateStr << ":  " << fixed << setprecision(2) << weeklyStats[i].avgRating 
             << " rating  (" << weeklyStats[i].orders << " orders)\n";
    }
    cout << "\n" << flush;
}

void SalesAnalytics::plotBarGraph(vector<PackageStats> stats, string title) {
    printHeader("\n" + title);
    
    if (stats.empty() || (stats.size() == 1 && stats[0].totalOrders == 0)) {
        printError("\nNo data available for this period.\n");
        return;
    }
    
    // Filter out empty packages
    vector<PackageStats> filteredStats;
    for (auto& s : stats) {
        if (s.totalOrders > 0) {
            filteredStats.push_back(s);
        }
    }
    
    if (filteredStats.empty()) {
        printError("\nNo data available for this period.\n");
        return;
    }
    
    // Find max rating
    double maxRating = 0;
    for (auto& p : filteredStats) {
        maxRating = max(maxRating, p.avgRating);
    }
    
    if (maxRating < 1) maxRating = 1;
    maxRating = ceil(maxRating * 2) / 2.0;  // Round up to nearest 0.5
    
    int graphHeight = 10;
    int barWidth = 8;
    
    cout << "\n";
    
    // Y-axis labels and top border
    cout << BOLD << CYAN;
    cout << setw(4) << fixed << setprecision(1) << maxRating << " +";
    for (size_t i = 0; i < filteredStats.size(); i++) {
        cout << string(barWidth, '-');
        if (i < filteredStats.size() - 1) cout << "+";
    }
    cout << "+" << RESET << "\n";
    
    // Draw bars from top to bottom
    for (int level = graphHeight; level > 0; level--) {
        double levelValue = (level / (double)graphHeight) * maxRating;
        
        cout << setw(4) << fixed << setprecision(1) << levelValue << " " << CYAN << "|" << RESET;
        
        for (size_t i = 0; i < filteredStats.size(); i++) {
            double barHeight = (filteredStats[i].avgRating / maxRating) * graphHeight;
            
            // Render bar at this level
            if (level <= barHeight) {
                // Inside bar - check if at top
                if (level > barHeight - 0.5) {  // At top of bar
                    cout << GREEN << "  ____  " << RESET;
                } else {
                    cout << GREEN << "  |  |  " << RESET;
                }
            } else {
                // Empty space above bar
                cout << "        ";
            }
            
            if (i < filteredStats.size() - 1) cout << CYAN << "|" << RESET;
        }
        
        cout << CYAN << "|" << RESET << "\n";
    }
    
    // Bottom border
    cout << "      " << CYAN << "+";
    for (size_t i = 0; i < filteredStats.size(); i++) {
        cout << string(barWidth, '-');
        if (i < filteredStats.size() - 1) cout << "+";
    }
    cout << "+" << RESET << "\n";
    
    // X-axis labels
    cout << "      ";
    for (size_t i = 0; i < filteredStats.size(); i++) {
        string label = filteredStats[i].packageName;
        // Abbreviate label
        string abbrev;
        size_t spacePos = label.find(' ');
        if (spacePos != string::npos) {
            abbrev = label.substr(0, 3) + label.substr(spacePos + 1, 3);  // "Mon Bk"
        } else {
            abbrev = label.substr(0, 8);
        }
        cout << setw(8) << abbrev;
        if (i < filteredStats.size() - 1) cout << " ";
    }
    cout << "\n\n" << flush;
}

void SalesAnalytics::plotTopSellingPackages(vector<PackageStats> topStats) {
    cout << "\n" << BOLD << CYAN << "╔═══════════════════════════════════════════════════════════════╗" << RESET << "\n";
    cout << BOLD << CYAN << "║  No. │ Package Name                   │ Avg Rating │ Orders   ║" << RESET << "\n";
    cout << BOLD << CYAN << "╠═══════════════════════════════════════════════════════════════╣" << RESET << "\n";
    
    for (size_t i = 0; i < topStats.size(); i++) {
        cout << BOLD << CYAN << "║" << RESET << setw(5) << right << (i + 1) << BOLD << CYAN << " │" << RESET 
             << " " << setw(29) << left << topStats[i].packageName << " " 
             << BOLD << CYAN << "│" << RESET << " " << fixed << setprecision(1) << setw(9) << topStats[i].avgRating << " " 
             << BOLD << CYAN << "│" << RESET << " " << setw(7) << right << topStats[i].totalOrders << BOLD << CYAN << " ║" << RESET << "\n";
    }
    
    cout << BOLD << CYAN << "╚═══════════════════════════════════════════════════════════════╝" << RESET << "\n\n" << flush;
}

void SalesAnalytics::displayTopSellingPackages() {
    loadFeedbackData();
    
    printHeader("\n📊 TOP SELLING PACKAGES (Last 30 Days)");
    
    auto allStats = getAllPackagesStats(30);
    auto topPackages = getTopPackages(10, 30);
    
    if (topPackages.empty()) {
        printError("\nNo sales data available for the last 30 days.\n");
        pauseScreen();
        return;
    }
    
    plotTopSellingPackages(topPackages);
    
    // Show bar graph visualization
    plotBarGraph(topPackages, "RATINGS VISUALIZATION (Top 10 Packages)");
    
    cout << flush;
    pauseScreen();
}

void SalesAnalytics::displayPackageAnalysis() {
    loadFeedbackData();
    
    auto allPackages = getAllPackagesStats(7);
    
    if (allPackages.empty()) {
        printError("\nNo packages data available.\n");
        return;
    }
    
    int selectedDay = 0;
    int selectedMeal = 0;
    int timePeriod = 7;
    
    while (true) {
        cout << "\n" << BOLD << CYAN << "╔════════════════════════════════════════╗" << RESET << "\n";
        cout << BOLD << CYAN << "║  SELECT DAY                              ║" << RESET << "\n";
        cout << BOLD << CYAN << "╠════════════════════════════════════════╣" << RESET << "\n";
        cout << BOLD << CYAN << "║" << RESET << "  1. Monday         2. Tuesday           " << BOLD << CYAN << "║" << RESET << "\n";
        cout << BOLD << CYAN << "║" << RESET << "  3. Wednesday      4. Thursday          " << BOLD << CYAN << "║" << RESET << "\n";
        cout << BOLD << CYAN << "║" << RESET << "  5. Friday         6. Saturday          " << BOLD << CYAN << "║" << RESET << "\n";
        cout << BOLD << CYAN << "║" << RESET << "  7. Sunday         0. Back              " << BOLD << CYAN << "║" << RESET << "\n";
        cout << BOLD << CYAN << "╚════════════════════════════════════════╝" << RESET << "\n";
        
        cout << BOLD << CYAN << "Enter your choice: " << RESET;
        cin >> selectedDay;
        
        if (selectedDay == 0) return;
        if (selectedDay < 1 || selectedDay > 7) {
            printError("Invalid choice. Please try again.");
            continue;
        }
        
        vector<string> dayNames = {"", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
        
        // Meal selection
        while (true) {
            cout << "\n" << BOLD << CYAN << "╔════════════════════════════════════════╗" << RESET << "\n";
            cout << BOLD << CYAN << "║  SELECT MEAL                             ║" << RESET << "\n";
            cout << BOLD << CYAN << "╠════════════════════════════════════════╣" << RESET << "\n";
            cout << BOLD << CYAN << "║" << RESET << "  1. Breakfast                            " << BOLD << CYAN << "║" << RESET << "\n";
            cout << BOLD << CYAN << "║" << RESET << "  2. Lunch                                " << BOLD << CYAN << "║" << RESET << "\n";
            cout << BOLD << CYAN << "║" << RESET << "  3. Dinner                               " << BOLD << CYAN << "║" << RESET << "\n";
            cout << BOLD << CYAN << "║" << RESET << "  0. Back                                 " << BOLD << CYAN << "║" << RESET << "\n";
            cout << BOLD << CYAN << "╚════════════════════════════════════════╝" << RESET << "\n";
            
            cout << BOLD << CYAN << "Enter your choice: " << RESET;
            cin >> selectedMeal;
            
            if (selectedMeal == 0) break;
            if (selectedMeal < 1 || selectedMeal > 3) {
                printError("Invalid choice. Please try again.");
                continue;
            }
            
            vector<string> mealNames = {"", "Breakfast", "Lunch", "Dinner"};
            
            // Time period selection
            while (true) {
                cout << "\n" << BOLD << CYAN << "╔════════════════════════════════════════╗" << RESET << "\n";
                cout << BOLD << CYAN << "║  SELECT TIME PERIOD                      ║" << RESET << "\n";
                cout << BOLD << CYAN << "╠════════════════════════════════════════╣" << RESET << "\n";
                cout << BOLD << CYAN << "║" << RESET << "  1. Last 7 Days (Daily Breakdown)       " << BOLD << CYAN << "║" << RESET << "\n";
                cout << BOLD << CYAN << "║" << RESET << "  2. Last 30 Days (Weekly Breakdown)     " << BOLD << CYAN << "║" << RESET << "\n";
                cout << BOLD << CYAN << "║" << RESET << "  0. Back                                " << BOLD << CYAN << "║" << RESET << "\n";
                cout << BOLD << CYAN << "╚════════════════════════════════════════╝" << RESET << "\n";
                
                cout << BOLD << CYAN << "Enter your choice: " << RESET;
                cin >> timePeriod;
                
                if (timePeriod == 0) break;
                if (timePeriod < 1 || timePeriod > 2) {
                    printError("Invalid choice. Please try again.");
                    continue;
                }
                
                string packageName = dayNames[selectedDay] + " " + mealNames[selectedMeal];
                int days = (timePeriod == 1) ? 7 : 30;
                
                cout << "\n";
                
                if (timePeriod == 1) {
                    auto dailyStats = getDailyBreakdown(packageName, 7);
                    if (dailyStats.empty()) {
                        printError("No data available for " + packageName + ".\n");
                    } else {
                        plotBarGraphDaily(dailyStats, "📈 " + packageName + " - Last 7 Days (Daily Breakdown)");
                        cout << flush;
                    }
                } else {
                    auto weeklyStats = getWeeklyBreakdown(packageName, 30);
                    if (weeklyStats.empty()) {
                        printError("No data available for " + packageName + ".\n");
                    } else {
                        plotBarGraphWeekly(weeklyStats, "📈 " + packageName + " - Last 30 Days (Weekly Breakdown)");
                        cout << flush;
                    }
                }
                
                pauseScreen();
                break;
            }
            
            break;
        }
        
        break;
    }
}

// Helper function for fixed precision string conversion
string fixed_to_string(double val, int precision) {
    ostringstream oss;
    oss << fixed << setprecision(precision) << val;
    return oss.str();
}
