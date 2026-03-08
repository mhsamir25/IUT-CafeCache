#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <iostream>

using namespace std;

class Utils {
public:
    static string getCurrentDateTime() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        stringstream ss;
        ss << 1900 + ltm->tm_year << "-"
           << setfill('0') << setw(2) << 1 + ltm->tm_mon << "-"
           << setfill('0') << setw(2) << ltm->tm_mday << " "
           << setfill('0') << setw(2) << ltm->tm_hour << ":"
           << setfill('0') << setw(2) << ltm->tm_min << ":"
           << setfill('0') << setw(2) << ltm->tm_sec;
        return ss.str();
    }

    static string getCurrentDate() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        stringstream ss;
        ss << 1900 + ltm->tm_year << "-"
           << setfill('0') << setw(2) << 1 + ltm->tm_mon << "-"
           << setfill('0') << setw(2) << ltm->tm_mday;
        return ss.str();
    }

    static string getDayOfWeek() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        string days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
        return days[ltm->tm_wday];
    }

    static int getWeekNumber() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        return (ltm->tm_yday / 7) + 1;
    }

    static string getMonthYear() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        stringstream ss;
        ss << 1900 + ltm->tm_year << "-" << setfill('0') << setw(2) << 1 + ltm->tm_mon;
        return ss.str();
    }

    static string trim(const string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, last - first + 1);
    }

    static int generateRandomId() {
        return rand() % 900000 + 100000;
    }

    static void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    static void pause() {
        cout << "\nPress Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    static bool confirmAction(const string& message) {
        cout << "\n" << message << " (y/n): ";
        char choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return (choice == 'y' || choice == 'Y');
    }
};

#endif