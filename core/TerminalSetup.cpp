#include "TerminalSetup.h"
#include<iostream>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

using namespace std;


struct termios originalSettings;

void setTerminal() {
    struct termios newSettings;
    tcgetattr(STDIN_FILENO, &originalSettings);
    newSettings = originalSettings;
    newSettings.c_lflag &= ~(ICANON | ECHO);
    newSettings.c_cc[VMIN] = 0;
    newSettings.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);    
}

void restoreTerminal(){
    tcsetattr(STDIN_FILENO, TCSANOW, &originalSettings);
}

bool isKeyPressed() {
    struct timeval tv;
    fd_set fds;
    tv. tv_sec = 0;
    tv.tv_usec = 50000;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

char readKey() {
    char ch;
    read(STDIN_FILENO, &ch, 1);
    return ch;
}

// Wait for any key (blocking)
char waitForKey() {
    // Temporarily make input blocking
    struct termios temp;
    tcgetattr(STDIN_FILENO, &temp);
    temp.c_cc[VMIN] = 1;  // Wait for at least 1 character
    temp.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &temp);
    
    char ch;
    read(STDIN_FILENO, &ch, 1);
    
    // Restore non-blocking
    temp.c_cc[VMIN] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &temp);
    
    return ch;
}

double getCurrentTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (tv.tv_usec / 1000000.0);
}

int getLength(const char text[]) {
    int len = 0;
    while (text[len] != '\0') {
        len++;
    }
    return len;
}

// Colored printing helper implementations
#include <sstream>

void printHeader(const std::string &text) {
    cout << BOLD << CYAN << text << RESET << endl;
}

void printInfo(const std::string &text) {
    cout << CYAN << text << RESET << endl;
}

void printLabelValue(const std::string &label, const std::string &value) {
    cout << WHITE << label << RESET << GREEN << value << RESET << endl;
}

void printSuccess(const std::string &text) {
    cout << GREEN << text << RESET << endl;
}

void printError(const std::string &text) {
    cout << RED << text << RESET << endl;
}

void printPrompt(const std::string &text) {
    cout << YELLOW << text << RESET;
}

void printSeparator() {
    cout << GRAY << "----------------------------------------" << RESET << endl;
}
