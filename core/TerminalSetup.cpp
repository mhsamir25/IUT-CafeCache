#include "TerminalSetup.h"
#include<iostream>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <unistd.h> // isatty

using namespace std;


struct termios originalSettings;

// Colors enabled by default when stdout is a tty
static bool colorsEnabled = isatty(STDOUT_FILENO);

void setColorsEnabled(bool enabled) {
    colorsEnabled = enabled;
}

bool isColorsEnabled() {
    return colorsEnabled;
}

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
    if (colorsEnabled) cout << BOLD << CYAN << text << RESET << endl;
    else cout << text << std::endl;
}

void printInfo(const std::string &text) {
    if (colorsEnabled) cout << CYAN << text << RESET << endl;
    else cout << text << std::endl;
}

void printLabelValue(const std::string &label, const std::string &value) {
    if (colorsEnabled) cout << WHITE << label << RESET << GREEN << value << RESET << endl;
    else cout << label << value << std::endl;
}

void printLabelValueColored(const std::string &label, const std::string &value, const std::string &valueColor) {
    if (colorsEnabled) cout << WHITE << label << RESET << valueColor << value << RESET << endl;
    else cout << label << value << std::endl;
}

void printSuccess(const std::string &text) {
    if (colorsEnabled) cout << GREEN << text << RESET << endl;
    else cout << text << std::endl;
}

void printError(const std::string &text) {
    if (colorsEnabled) cout << RED << text << RESET << endl;
    else cout << text << std::endl;
}

void printPrompt(const std::string &text) {
    if (colorsEnabled) cout << YELLOW << text << RESET;
    else cout << text;
    cout.flush();
}

void printSeparator() {
    cout << GRAY << "----------------------------------------" << RESET << endl;
}

// Read password from terminal and display '*' for each character typed.
// Handles backspace (127) and Enter. Returns the entered password (no newline).
std::string readPasswordMasked() {
    std::string password;
    
#ifdef _WIN32
    // Windows fallback using conio
    #include <conio.h>
    char ch;
    while (true) {
        ch = _getch();
        if (ch == '\r' || ch == '\n') break;
        if (ch == 8) { // backspace
            if (!password.empty()) {
                password.pop_back();
                // Erase star
                std::cout << "\b \b";
            }
            continue;
        }
        password.push_back(ch);
        std::cout << '*';
    }
    std::cout << std::endl;
    return password;
#else
    struct termios oldt, newt;

    // Get current terminal settings
    if (tcgetattr(STDIN_FILENO, &oldt) != 0) {
        return "";
    }

    newt = oldt;
    // Turn off echo
    newt.c_lflag &= ~(ECHO);
    // Set the new attributes
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    char ch;
    while (true) {
        ssize_t n = read(STDIN_FILENO, &ch, 1);
        if (n <= 0) continue;
        if (ch == '\n' || ch == '\r') {
            break;
        }
        if (ch == 127 || ch == '\b') { // backspace
            if (!password.empty()) {
                password.pop_back();
                // Erase a star from screen
                std::cout << "\b \b";
                std::cout.flush();
            }
            continue;
        }
        password.push_back(ch);
        std::cout << '*';
        std::cout.flush();
    }

    // Restore terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    std::cout << std::endl;
    return password;
#endif
}
