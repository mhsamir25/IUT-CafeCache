#include "TokenGenerator.h"
#include <sstream>
#include <iomanip>
#include <ctime>

using namespace std;

int TokenGenerator::tokenCounter = 1000;

string TokenGenerator::generateTokenId() {
    tokenCounter++;
    
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