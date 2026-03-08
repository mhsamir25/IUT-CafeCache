#ifndef MENUITEM_H
#define MENUITEM_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include "Utils.h"

using namespace std;

class MenuItem {
public:
    int itemId;
    string name;
    string category;
    double price;
    int stock;

    MenuItem() : itemId(0), price(0.0), stock(0) {}

    MenuItem(int id, string n, string cat, double p, int s) 
        : itemId(id), name(n), category(cat), price(p), stock(s) {}

    static void initializeMenu() {
        ifstream checkFile("menu_items.txt");
        if (checkFile.good()) {
            checkFile.close();
            return;
        }
        checkFile.close();

        ofstream file("menu_items.txt");
        if (!file) return;

        file << "1|Egg Paratha|breakfast|50.00|100" << endl;
        file << "2|Chicken Sandwich|breakfast|80.00|100" << endl;
        file << "3|Noodles|breakfast|60.00|100" << endl;
        file << "4|Tea|breakfast|15.00|200" << endl;

        file << "5|Rice with Chicken Curry|lunch|120.00|80" << endl;
        file << "6|Rice with Beef Curry|lunch|150.00|60" << endl;
        file << "7|Fried Rice|lunch|100.00|80" << endl;
        file << "8|Chicken Biryani|lunch|140.00|70" << endl;

        file << "9|Chicken Burger|dinner|150.00|60" << endl;
        file << "10|Beef Burger|dinner|180.00|50" << endl;
        file << "11|Pasta|dinner|120.00|70" << endl;
        file << "12|Pizza Slice|dinner|100.00|80" << endl;

        file.close();
    }

    static void viewMenuByCategory(const string& category) {
        ifstream file("menu_items.txt");
        if (!file) {
            cout << "\nError: Menu not available!" << endl;
            return;
        }

        cout << "\n" << string(80, '=') << endl;
        string displayCategory = category;
        displayCategory[0] = toupper(displayCategory[0]);
        cout << displayCategory << " MENU" << endl;
        cout << string(80, '=') << endl;
        
        string line;
        bool found = false;
        
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            while (getline(ss, token, '|')) {
                tokens.push_back(Utils::trim(token));
            }

            if (tokens.size() >= 5 && tokens[2] == category) {
                found = true;
                cout << "[" << tokens[0] << "] " << tokens[1] << endl;
                cout << "    Price: Tk " << tokens[3] << " | Stock: " << tokens[4] << endl;
            }
        }
        
        file.close();
        
        if (!found) {
            cout << "No items available in this category." << endl;
        }
        cout << string(80, '=') << endl;
    }

    static MenuItem* getItem(int itemId) {
        ifstream file("menu_items.txt");
        if (!file) return nullptr;

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            while (getline(ss, token, '|')) {
                tokens.push_back(Utils::trim(token));
            }

            if (tokens.size() >= 5 && stoi(tokens[0]) == itemId) {
                MenuItem* item = new MenuItem();
                item->itemId = stoi(tokens[0]);
                item->name = tokens[1];
                item->category = tokens[2];
                item->price = stod(tokens[3]);
                item->stock = stoi(tokens[4]);
                file.close();
                return item;
            }
        }
        
        file.close();
        return nullptr;
    }

    void updateStock() {
        vector<string> allItems;
        ifstream file("menu_items.txt");
        string line;

        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            while (getline(ss, token, '|')) {
                tokens.push_back(Utils::trim(token));
            }

            if (tokens.size() >= 5 && stoi(tokens[0]) == itemId) {
                stringstream newLine;
                newLine << itemId << "|" << name << "|" << category << "|" 
                       << fixed << setprecision(2) << price << "|" << stock;
                allItems.push_back(newLine.str());
            } else {
                allItems.push_back(line);
            }
        }
        file.close();

        ofstream outFile("menu_items.txt");
        for (const auto& itemLine : allItems) {
            outFile << itemLine << endl;
        }
        outFile.close();
    }
};

#endif