#include "TerminalSetup.h"
#include<iostream>
#include <chrono>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <sys/time.h>
    #include <sys/select.h>
#endif

using namespace std;

#ifdef _WIN32
    static HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    static HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    static DWORD originalConsoleMode;
#else
    struct termios originalSettings;
#endif

static bool colorsEnabled = true;  // Enable colors by default on Windows

void setColorsEnabled(bool enabled) {
    colorsEnabled = enabled;
}

bool isColorsEnabled() {
    return colorsEnabled;
}

void setTerminal() {
#ifdef _WIN32
    GetConsoleMode(hStdin, &originalConsoleMode);
    DWORD newMode = originalConsoleMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    SetConsoleMode(hStdin, newMode);
#else
    struct termios newSettings;
    tcgetattr(STDIN_FILENO, &originalSettings);
    newSettings = originalSettings;
    newSettings.c_lflag &= ~(ICANON | ECHO);
    newSettings.c_cc[VMIN] = 0;
    newSettings.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
#endif
}

void restoreTerminal(){
#ifdef _WIN32
    SetConsoleMode(hStdin, originalConsoleMode);
#else
    tcsetattr(STDIN_FILENO, TCSANOW, &originalSettings);
#endif
}

bool isKeyPressed() {
#ifdef _WIN32
    return _kbhit() != 0;
#else
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 50000;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
#endif
}

char readKey() {
#ifdef _WIN32
    return _getch();
#else
    char ch;
    read(STDIN_FILENO, &ch, 1);
    return ch;
#endif
}

// Wait for any key (blocking)
char waitForKey() {
#ifdef _WIN32
    return _getch();
#else
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
#endif
}

double getCurrentTime() {
#ifdef _WIN32
    return static_cast<double>(GetTickCount()) / 1000.0;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (tv.tv_usec / 1000000.0);
#endif
}

int getLength(const char text[]) {
    int len = 0;
    while (text[len] != '\0') {
        len++;
    }
    return len;
}

// Initialize console for Windows UTF-8 and ANSI color support
void initializeConsole() {
#ifdef _WIN32
    // Enable UTF-8 code page for proper Unicode/box drawing character support
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    
    // Enable ANSI escape sequences and Virtual Terminal Processing
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
    
    // Ensure stdin supports UTF-8 as well
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hIn != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hIn, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
            SetConsoleMode(hIn, dwMode);
        }
    }
#endif
}

// Colored printing helper implementations
#include <sstream>

#ifdef _WIN32
void enableWindowsConsoleColors() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#endif

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
// Handles backspace and Enter. Returns the entered password (no newline).
std::string readPasswordMasked() {
    std::string password;
    
#ifdef _WIN32
    // Windows using conio
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
