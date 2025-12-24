#include <iostream>
#include <limits>
#include "User.h"
#include "AuthManager.h"
#include "FileManager.h"
#include "RechargeManager.h"
#include "Token.h"
#include "TokenGenerator.h"
#include "TerminalSetup.h"

using namespace std;

// Forward declarations
void clearScreen();
void pauseScreen();
void displayMainMenu();
void displayStudentMenu();
void displayAdminMenu();
void loginUser();
void handleStudentOperations();
void handleAdminOperations();
void registerNewUser();
void submitRechargeRequest();
void viewRechargeStatus();
void viewWalletBalance();
void generateOrderToken();
void viewPurchaseHistory();
void viewPendingRecharges();
void processRechargeRequest();
void viewAllUsers();

// Utility functions
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pauseScreen() {
    printPrompt("\nPress Enter to continue...");
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Main menu display
void displayMainMenu() {
    clearScreen();
    cout<<"\n";
    printHeader("╔═══════════════════════════════════════╗");
    printHeader("║        IUT CAFECACHE SYSTEM           ║");
    printHeader("║       Cafe Management System          ║");
    printHeader("╚═══════════════════════════════════════╝");
    printInfo("\n1. Login");
    printInfo("2. Register New User");
    printInfo("3. Exit");
    printPrompt("\nEnter your choice: ");
}

// Student menu
void displayStudentMenu() {
    clearScreen();
    User* currentUser = AuthManager::getCurrentUser();
    
    cout<<"\n";
    printHeader("╔═══════════════════════════════════════╗");
    printHeader("║         STUDENT/TEACHER MENU          ║");
    printHeader("╚═══════════════════════════════════════╝");
    printLabelValue("Welcome: ", currentUser->getName());
    printLabelValue("User ID: ", currentUser->getUserID());
    printInfo("\n1. View Wallet Balance");
    printInfo("2. Submit Recharge Request");
    printInfo("3. View Recharge Request Status");
    printInfo("4. Place Order (Weekday Menu)");
    printInfo("5. View Purchase History");
    printInfo("6. Logout");
    printPrompt("\nEnter your choice: ");
}

// Admin menu
void displayAdminMenu() {
    clearScreen();
    User* currentUser = AuthManager::getCurrentUser();
    cout<<"\n";
    printHeader("╔═══════════════════════════════════════╗");
    printHeader("║           ADMIN MENU                  ║");
    printHeader("╚═══════════════════════════════════════╝");
    printLabelValue("Welcome: ", currentUser->getName());
    printInfo("\n1. View Pending Recharge Requests");
    printInfo("2. Process Recharge Request");
    printInfo("3. View All Users");
    printInfo("4. Logout");
    printPrompt("\nEnter your choice: ");
}

// Login function
void loginUser() {
    clearScreen();
    printHeader("\n========== USER LOGIN ==========");
    
    string userId, password;
    printPrompt("Enter User ID: ");
    cin >> userId;
    printPrompt("Enter Password: ");
    cin >> password;
    
    if (AuthManager::login(userId, password)) {
    printSuccess("\n✓ Login successful!\n");
        pauseScreen();
        
        if (AuthManager::isAdmin()) {
            handleAdminOperations();
        } else {
            handleStudentOperations();
        }
    } else {
        printError("\n✗ Invalid credentials. Please try again.\n");
        pauseScreen();
    }
}

// Register new user
void registerNewUser() {
    clearScreen();
    printHeader("\n========== USER REGISTRATION ==========");
    
    string userId, password, name, role;
    
    printPrompt("Enter User ID (e.g., 230042101): ");
    cin >> userId;
    
    // Check if user already exists
    vector<User> users = FileManager::loadUsers();
    for (const auto& user : users) {
        if (user.getUserID() == userId) {
            printError("\n✗ Error: User ID already exists!\n");
            pauseScreen();
            return;
        }
    }
    
    printPrompt("Enter Password: ");
    cin >> password;
    
    cin.ignore();
    printPrompt("Enter Full Name: ");
    getline(cin, name);
    
    printInfo("\nSelect Role:");
    printInfo("1. Student");
    printInfo("2. Teacher");
    printPrompt("Enter choice (1 or 2): ");
    
    int roleChoice;
    cin >> roleChoice;
    
    if (roleChoice == 1) {
        role = "STUDENT";
    } else if (roleChoice == 2) {
        role = "TEACHER";
    } else {
        printError("\n✗ Invalid role selection!\n");
        pauseScreen();
        return;
    }
    
    User newUser(userId, password, name, role, 0.0);
    
    if (FileManager::addUser(newUser)) {
        printSuccess("\n✓ Registration successful!");
        printLabelValue("User ID: ", userId);
        printLabelValue("Role: ", role);
    } else {
        printError("\n✗ Registration failed. Please try again.\n");
    }
    
    pauseScreen();
}

void generateOrderToken() {
        clearScreen();
        User* currentUser = AuthManager::getCurrentUser();

        // Reload user data to get latest wallet balance
        vector<User> users = FileManager::loadUsers();
        User* updatedUser = FileManager::findUser(currentUser->getUserID(), users);

        if (updatedUser != nullptr) {
            currentUser->setWalletBalance(updatedUser->getWalletBalance());
        }

    printHeader("\n========== PLACE ORDER (WEEKDAY MENU) ==========");
    printLabelValue("Wallet Balance: BDT ", to_string(currentUser->getWalletBalance()));

        // Weekday selection
        vector<string> days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};

        for (size_t i = 0; i < days.size(); ++i) {
            printInfo(to_string(i+1) + ". " + days[i]);
        }
        printInfo("0. Cancel");

        int dayChoice;
        printPrompt("\nSelect day (1-5): ");
        cin >> dayChoice;

        if (dayChoice == 0) return;
        if (dayChoice < 1 || dayChoice > 5) {
            printError("\n✗ Invalid day selection.");
            pauseScreen();
            return;
        }

        string selectedDay = days[dayChoice-1];

        // Meal category
    printInfo("\nSelect meal:");
    printInfo("1. Breakfast");
    printInfo("2. Lunch");
    printInfo("3. Dinner");
    printInfo("0. Cancel");

    int mealChoice;
    printPrompt("\nEnter choice: ");
    cin >> mealChoice;

        if (mealChoice == 0) return;
        if (mealChoice < 1 || mealChoice > 3) {
            printError("\n✗ Invalid meal selection.");
            pauseScreen();
            return;
        }

        // Define simple weekday Bangladeshi menus (examples)
        // For clarity we hardcode some items; these can be moved to a file later.
        struct MenuItem { string name; double price; };

        vector<MenuItem> menuItems;

        // Provide different menus per day and meal
        if (selectedDay == "Monday") {
            if (mealChoice == 1) { // Breakfast
                menuItems = { {"Paratha & Begun Bhaja", 25}, {"Luchi & Dim", 30}, {"Tea", 10} };
            } else if (mealChoice == 2) { // Lunch
                menuItems = { {"Rice & Chicken Curry", 80}, {"Daal & Vegetable", 50}, {"Salad", 20} };
            } else { // Dinner
                menuItems = { {"Khichuri", 70}, {"Fried Fish", 90}, {"Panta Ilish (small)", 120} };
            }
        } else if (selectedDay == "Tuesday") {
            if (mealChoice == 1) {
                menuItems = { {"Panta Bhaat & Bhorta", 35}, {"Ruti & Omelette", 30}, {"Tea", 10} };
            } else if (mealChoice == 2) {
                menuItems = { {"Rice & Beef Curry", 100}, {"Shorshe Ilish (small)", 140}, {"Mix Veg", 45} };
            } else {
                menuItems = { {"Biryani (chicken)", 120}, {"Raita", 20}, {"Kebab", 90} };
            }
        } else if (selectedDay == "Wednesday") {
            if (mealChoice == 1) {
                menuItems = { {"Shemai", 40}, {"Chirer Pulao", 35}, {"Tea", 10} };
            } else if (mealChoice == 2) {
                menuItems = { {"Rice & Fish Curry", 90}, {"Daal", 40}, {"Vegetable Bhaji", 35} };
            } else {
                menuItems = { {"Kacchi Biryani (small)", 150}, {"Salad", 20}, {"Papad", 10} };
            }
        } else if (selectedDay == "Thursday") {
            if (mealChoice == 1) {
                menuItems = { {"Ruti & Chana", 30}, {"Egg Roll", 50}, {"Tea", 10} };
            } else if (mealChoice == 2) {
                menuItems = { {"Rice & Chicken Roast", 95}, {"Mixed Daal", 40}, {"Aloor Dom", 45} };
            } else {
                menuItems = { {"Morog Polao (small)", 130}, {"Salad", 20}, {"Fried Veg", 35} };
            }
        } else { // Friday
            if (mealChoice == 1) {
                menuItems = { {"Luchi & Aloor Dum", 35}, {"Chana Puri", 30}, {"Tea", 10} };
            } else if (mealChoice == 2) {
                menuItems = { {"Rice & Mutton Curry (small)", 150}, {"Daal", 40}, {"Green Veg", 45} };
            } else {
                menuItems = { {"Pulao & Chicken", 110}, {"Korma (small)", 80}, {"Raita", 20} };
            }
        }

        // Display items
    string mealLabel = (mealChoice == 1) ? "Breakfast" : (mealChoice == 2 ? "Lunch" : "Dinner");
    printHeader("\nMenu for " + selectedDay + " - " + mealLabel + "\n");

        for (size_t i = 0; i < menuItems.size(); ++i) {
            string line = to_string(i+1) + ". " + menuItems[i].name + " - BDT " + to_string(menuItems[i].price);
            printInfo(line);
        }
        printInfo("0. Finish order / Go back");

        // Create token and let user add multiple items
        string tokenId = TokenGenerator::generateTokenId();
        Token token(tokenId, currentUser->getUserID());

        while (true) {
            printPrompt("\nSelect item number to add (0 to finish): ");
            int itemChoice;
            cin >> itemChoice;

            if (itemChoice == 0) break;
            if (itemChoice < 1 || itemChoice > (int)menuItems.size()) {
                printError("Invalid item selection.");
                continue;
            }

            int quantity;
            printPrompt("Enter quantity: ");
            cin >> quantity;
            if (quantity <= 0) {
                printError("Quantity must be at least 1.");
                continue;
            }

            token.addItem(menuItems[itemChoice-1].name, quantity, menuItems[itemChoice-1].price);
            printSuccess("✓ Added: " + menuItems[itemChoice-1].name + " x" + to_string(quantity));
        }

        if (token.getTotalAmount() == 0) {
            printError("\n✗ No items selected. Order cancelled.");
            pauseScreen();
            return;
        }

        // Check wallet balance
            if (currentUser->getWalletBalance() < token.getTotalAmount()) {
            printError("\n✗ Insufficient wallet balance!");
            printLabelValue("Required: BDT ", to_string(token.getTotalAmount()));
            printLabelValue("Available: BDT ", to_string(currentUser->getWalletBalance()));
            pauseScreen();
            return;
        }

        char confirm;
    printLabelValue("\nTotal Amount: BDT ", to_string(token.getTotalAmount()));
    printPrompt("Confirm order and generate token automatically? (y/n): ");
        cin >> confirm;

        if (confirm == 'y' || confirm == 'Y') {
            // Deduct from wallet and persist
            vector<User> allUsers = FileManager::loadUsers();
            User* user = FileManager::findUser(currentUser->getUserID(), allUsers);

            if (user != nullptr && user->deductFromWallet(token.getTotalAmount())) {
                FileManager::saveUsers(allUsers);
                FileManager::addToken(token);

                // Update current user's balance
                currentUser->setWalletBalance(user->getWalletBalance());

                clearScreen();
                token.displayToken();
                printSuccess("✓ Order placed successfully and token generated!");
                printLabelValue("Remaining Balance: BDT ", to_string(currentUser->getWalletBalance()));
            } else {
                printError("\n✗ Failed to deduct from wallet. Order not placed.");
            }
        } else {
            printInfo("\nOrder cancelled.");
        }

    pauseScreen();
}

// Student operations
void handleStudentOperations() {
    int choice;
    
    while (true) {
        displayStudentMenu();
        cin >> choice;
        
        switch (choice) {
            case 1:
                viewWalletBalance();
                break;
            case 2:
                submitRechargeRequest();
                break;
            case 3:
                viewRechargeStatus();
                break;
            case 4:
                generateOrderToken();
                break;
            case 5:
                viewPurchaseHistory();
                break;
            case 6:
                AuthManager::logout();
                printSuccess("\n✓ Logged out successfully!\n");
                pauseScreen();
                return;
            default:
                printError("\n✗ Invalid choice. Please try again.\n");
                pauseScreen();
        }
    }
}

// View wallet balance
void viewWalletBalance() {
    clearScreen();
    User* currentUser = AuthManager::getCurrentUser();
    
    // Reload user data to get latest wallet balance
    vector<User> users = FileManager::loadUsers();
    User* updatedUser = FileManager::findUser(currentUser->getUserID(), users);
    
    if (updatedUser != nullptr) {
        currentUser->setWalletBalance(updatedUser->getWalletBalance());
    }
    
    printHeader("\n========== WALLET BALANCE ==========");
    printLabelValue("User: ", currentUser->getName());
    printLabelValue("User ID: ", currentUser->getUserID());
    printLabelValue("\nCurrent Balance: BDT ", to_string(currentUser->getWalletBalance()));
    
        pauseScreen();
    }
// Submit recharge request
void submitRechargeRequest() {
    clearScreen();
    User* currentUser = AuthManager::getCurrentUser();
    
    printHeader("\n========== SUBMIT RECHARGE REQUEST ==========");
    printLabelValue("Current Balance: BDT ", to_string(currentUser->getWalletBalance()));
    
    double amount;
    printPrompt("Enter amount to recharge: BDT ");
    cin >> amount;
    
    char confirm;
    printPrompt("\nConfirm recharge request of BDT " + to_string(amount) + "? (y/n): ");
    cin >> confirm;
    
    if (confirm == 'y' || confirm == 'Y') {
        RechargeManager::submitRechargeRequest(currentUser->getUserID(), amount);
    } else {
        printInfo("\nRecharge request cancelled.\n");
    }
    
    pauseScreen();
}

// View recharge status
void viewRechargeStatus() {
    clearScreen();
    User* currentUser = AuthManager::getCurrentUser();
    
    RechargeManager::viewUserRequests(currentUser->getUserID());
    pauseScreen();
}



// View purchase history
void viewPurchaseHistory() {
    clearScreen();
    User* currentUser = AuthManager::getCurrentUser();
    
    // Get all tokens for this user
    vector<Token> tokens = FileManager::getUserTokens(currentUser->getUserID());
    
    printHeader("\n========== PURCHASE HISTORY ==========");

    // Debug: show how many tokens found
    printLabelValue("Found ", to_string(tokens.size()) + " token(s) for user " + currentUser->getUserID() + ".");

    if (tokens.empty()) {
        printInfo("No purchase history found.\n");
    } else {
        // Print each token (most recent last as stored in file)
        int idx = 1;
        for (const auto& token : tokens) {
            printInfo("----- Order #" + to_string(idx++) + " -----");
            token.displayToken();
            printSeparator();
            printInfo("\n");
        }
    }
    
    pauseScreen();
}

// Admin operations
void handleAdminOperations() {
    int choice;
    
    while (true) {
        displayAdminMenu();
        cin >> choice;
        
        switch (choice) {
            case 1:
                viewPendingRecharges();
                break;
            case 2:
                processRechargeRequest();
                break;
            case 3:
                viewAllUsers();
                break;
            case 4:
                AuthManager::logout();
                printSuccess("\n✓ Logged out successfully!\n");
                pauseScreen();
                return;
            default:
                printError("\n✗ Invalid choice. Please try again.\n");
                pauseScreen();
        }
    }
}

// View pending recharges
void viewPendingRecharges() {
    clearScreen();
    RechargeManager::viewPendingRequests();
    pauseScreen();
}

// Process recharge request
void processRechargeRequest() {
    clearScreen();
    RechargeManager::viewPendingRequests();
    
    string requestId;
    printPrompt("\nEnter Request ID to process (or 0 to cancel): ");
    cin >> requestId;
    
    if (requestId == "0") {
        return;
    }
    
    char decision;
    printPrompt("Approve this request? (y/n): ");
    cin >> decision;
    
    if (decision == 'y' || decision == 'Y') {
        RechargeManager::approveRequest(requestId);
    } else {
        RechargeManager::rejectRequest(requestId);
    }
    
    pauseScreen();
}

// View all users
void viewAllUsers() {
    clearScreen();
    vector<User> users = FileManager::loadUsers();
    
    printHeader("\n========== ALL USERS ==========");
    
    for (const auto& user : users) {
        printLabelValue("User ID: ", user.getUserID());
        printLabelValue("Name: ", user.getName());
        printLabelValue("Role: ", user.getRole());
        printLabelValue("Wallet Balance: BDT ", to_string(user.getWalletBalance()));
        printSeparator();
    }
    
    pauseScreen();
}

int main() {
    int choice;

    while (true) {
        displayMainMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                loginUser();
                break;
            case 2:
                registerNewUser();
                break;
            case 3:
                clearScreen();
                printInfo("\nThank you for using IUT CafeCache System!");
                printInfo("Goodbye!\n");
                return 0;
            default:
                printError("\n✗ Invalid choice. Please try again.\n");
                pauseScreen();
        }
    }

    return 0;
}