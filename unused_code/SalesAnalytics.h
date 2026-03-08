#ifndef SALESANALYTICS_H
#define SALESANALYTICS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <iomanip>
#include <algorithm>
#include "Utils.h"

using namespace std;

class SalesAnalytics {
public:
    struct SalesData {
        string itemName;
        int quantity;
        double revenue;
        
        SalesData() : quantity(0), revenue(0.0) {}
    };

    static void generateDailySales(const string& targetDate) {
        map<string, SalesData> salesMap;
        double totalRevenue = 0.0;
        int totalItems = 0;

        ifstream file("transactions.txt");
        if (!file) {
            cout << "\nError: No transaction data available!" << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            while (getline(ss, token, '|')) {
                tokens.push_back(Utils::trim(token));
            }

            if (tokens.size() >= 8 && tokens[6] == targetDate) {
                string itemName = tokens[1];
                int quantity = stoi(tokens[3]);
                double amount = stod(tokens[4]);

                salesMap[itemName].itemName = itemName;
                salesMap[itemName].quantity += quantity;
                salesMap[itemName].revenue += amount;

                totalRevenue += amount;
                totalItems += quantity;
            }
        }
        file.close();

        cout << "\n" << string(100, '=') << endl;
        cout << "DAILY SALES REPORT - " << targetDate << endl;
        cout << string(100, '=') << endl;

        if (salesMap.empty()) {
            cout << "\nNo sales data available for this date." << endl;
        } else {
            for (const auto& pair : salesMap) {
                cout << "\n" << pair.second.itemName << endl;
                cout << "Quantity Sold: " << pair.second.quantity << endl;
                cout << "Revenue: Tk " << fixed << setprecision(2) << pair.second.revenue << endl;
            }

            cout << "\n" << string(100, '-') << endl;
            cout << "Total Revenue: Tk " << fixed << setprecision(2) << totalRevenue << endl;
            cout << "Total Items Sold: " << totalItems << endl;
        }
        cout << string(100, '=') << endl;
    }

    static void generateWeeklySales(int weekNumber) {
        map<string, SalesData> salesMap;
        double totalRevenue = 0.0;

        ifstream file("transactions.txt");
        if (!file) {
            cout << "\nError: No transaction data available!" << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            while (getline(ss, token, '|')) {
                tokens.push_back(Utils::trim(token));
            }

            if (tokens.size() >= 8) {
                string itemName = tokens[1];
                int quantity = stoi(tokens[3]);
                double amount = stod(tokens[4]);

                salesMap[itemName].itemName = itemName;
                salesMap[itemName].quantity += quantity;
                salesMap[itemName].revenue += amount;

                totalRevenue += amount;
            }
        }
        file.close();

        cout << "\n" << string(100, '=') << endl;
        cout << "WEEKLY SALES REPORT - Week " << weekNumber << endl;
        cout << string(100, '=') << endl;

        for (const auto& pair : salesMap) {
            cout << "\n" << pair.second.itemName << endl;
            cout << "Quantity Sold: " << pair.second.quantity << endl;
            cout << "Revenue: Tk " << fixed << setprecision(2) << pair.second.revenue << endl;
        }

        cout << "\n" << string(100, '-') << endl;
        cout << "Total Revenue: Tk " << fixed << setprecision(2) << totalRevenue << endl;
        cout << string(100, '=') << endl;
    }

    static void generateMonthlySales(const string& monthYear) {
        map<string, SalesData> salesMap;
        double totalRevenue = 0.0;

        ifstream file("transactions.txt");
        if (!file) {
            cout << "\nError: No transaction data available!" << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            while (getline(ss, token, '|')) {
                tokens.push_back(Utils::trim(token));
            }

            if (tokens.size() >= 8 && tokens[6].substr(0, 7) == monthYear) {
                string itemName = tokens[1];
                int quantity = stoi(tokens[3]);
                double amount = stod(tokens[4]);

                salesMap[itemName].itemName = itemName;
                salesMap[itemName].quantity += quantity;
                salesMap[itemName].revenue += amount;

                totalRevenue += amount;
            }
        }
        file.close();

        cout << "\n" << string(100, '=') << endl;
        cout << "MONTHLY SALES REPORT - " << monthYear << endl;
        cout << string(100, '=') << endl;

        for (const auto& pair : salesMap) {
            cout << "\n" << pair.second.itemName << endl;
            cout << "Quantity Sold: " << pair.second.quantity << endl;
            cout << "Revenue: Tk " << fixed << setprecision(2) << pair.second.revenue << endl;
        }

        cout << "\n" << string(100, '-') << endl;
        cout << "Total Revenue: Tk " << fixed << setprecision(2) << totalRevenue << endl;
        cout << string(100, '=') << endl;
    }

    static void generateSalesByDay(const string& dayName) {
        map<string, SalesData> salesMap;
        double totalRevenue = 0.0;

        ifstream file("transactions.txt");
        if (!file) {
            cout << "\nError: No transaction data available!" << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            while (getline(ss, token, '|')) {
                tokens.push_back(Utils::trim(token));
            }

            if (tokens.size() >= 8 && tokens[7] == dayName) {
                string itemName = tokens[1];
                int quantity = stoi(tokens[3]);
                double amount = stod(tokens[4]);

                salesMap[itemName].itemName = itemName;
                salesMap[itemName].quantity += quantity;
                salesMap[itemName].revenue += amount;

                totalRevenue += amount;
            }
        }
        file.close();

        cout << "\n" << string(100, '=') << endl;
        cout << "SALES REPORT - All " << dayName << "s" << endl;
        cout << string(100, '=') << endl;

        if (salesMap.empty()) {
            cout << "\nNo sales data available for " << dayName << "." << endl;
        } else {
            for (const auto& pair : salesMap) {
                cout << "\n" << pair.second.itemName << endl;
                cout << "Quantity Sold: " << pair.second.quantity << endl;
                cout << "Revenue: Tk " << fixed << setprecision(2) << pair.second.revenue << endl;
            }

            cout << "\n" << string(100, '-') << endl;
            cout << "Total Revenue: Tk " << fixed << setprecision(2) << totalRevenue << endl;
        }
        cout << string(100, '=') << endl;
    }

    static void displayBarGraph() {
        map<string, int> itemSales;
        int maxSales = 0;

        ifstream file("transactions.txt");
        if (!file) {
            cout << "\nError: No transaction data available!" << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            while (getline(ss, token, '|')) {
                tokens.push_back(Utils::trim(token));
            }

            if (tokens.size() >= 4) {
                string itemName = tokens[1];
                int quantity = stoi(tokens[3]);
                itemSales[itemName] += quantity;
                maxSales = max(maxSales, itemSales[itemName]);
            }
        }
        file.close();

        cout << "\n" << string(100, '=') << endl;
        cout << "SALES BAR GRAPH (Total Quantities Sold)" << endl;
        cout << string(100, '=') << endl << endl;

        if (itemSales.empty()) {
            cout << "No sales data available." << endl;
        } else {
            int scale = max(1, maxSales / 50);
            
            for (const auto& pair : itemSales) {
                cout << setw(30) << left << pair.first << " | ";
                int bars = pair.second / scale;
                for (int i = 0; i < bars; i++) {
                    cout << (char)219;
                }
                cout << " " << pair.second << endl;
            }

            cout << "\n" << string(100, '=') << endl;
            cout << "Scale: Each block = " << scale << " items" << endl;
        }
        cout << string(100, '=') << endl;
    }
};

#endif