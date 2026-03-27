#include "TokenGenerator.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <fstream>

using namespace std;

int TokenGenerator::tokenCounter = 0;

void TokenGenerator::initializeCounter() {
    ifstream file("counter.txt");
    if (file.is_open()) {
        file >> tokenCounter;
        file.close();
    } else {
        tokenCounter = 1000;  // Default starting point
    }
}

void TokenGenerator::saveCounter() {
    ofstream file("counter.txt");
    if (file.is_open()) {
        file << tokenCounter;
        file.close();
    }
}

string TokenGenerator::generateTokenId() {
    tokenCounter++;
    saveCounter();
    
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);
    
    stringstream ss;
    ss << "TKN" 
       << setfill('0') << setw(4) << (1900 + ltm->tm_year)
       << setw(2) << (1 + ltm->tm_mon)
       << setw(2) << ltm->tm_mday
       << "-" << tokenCounter;
    
    return ss.str();
}

string TokenGenerator::generateRequestId() {
    tokenCounter++;
    saveCounter();
    
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);
    
    stringstream ss;
    ss << "REQ" 
       << setfill('0') << setw(4) << (1900 + ltm->tm_year)
       << setw(2) << (1 + ltm->tm_mon)
       << setw(2) << ltm->tm_mday
       << "-" << tokenCounter;
    
    return ss.str();
}