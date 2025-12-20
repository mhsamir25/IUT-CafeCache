#include <iostream>
#include <limits>
#include "User.h"
#include "AuthManager.h"
#include "FileManager.h"
#include "RechargeManager.h"
#include "Token.h"
#include "TokenGenerator.h"

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
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Main menu display
void displayMainMenu() {
    clearScreen();
    cout << "\n╔═══════════════════════════════════════╗" << endl;
    cout << "║       IUT CAFECACHE SYSTEM           ║" << endl;
    cout << "║     Cafe Management System           ║" << endl;
    cout << "╚═══════════════════════════════════════╝" << endl;
    cout << "\n1. Login" << endl;
    cout << "2. Register New User" << endl;
    cout << "3. Exit" << endl;
    cout << "\nEnter your choice: ";
}

// Student menu
void displayStudentMenu() {
    clearScreen();
    User* currentUser = AuthManager::getCurrentUser();
    
    cout << "\n╔═══════════════════════════════════════╗" << endl;
    cout << "║         STUDENT/TEACHER MENU         ║" << endl;
    cout << "╚═══════════════════════════════════════╝" << endl;
    cout << "\nWelcome, " << currentUser->getName() << "!" << endl;
    cout << "User ID: " << currentUser->getUserID() << endl;
    cout << "\n1. View Wallet Balance" << endl;
    cout << "2. Submit Recharge Request" << endl;
    cout << "3. View Recharge Request Status" << endl;
    cout << "4. Place Order (Weekday Menu)" << endl;
    cout << "5. View Purchase History" << endl;
    cout << "6. Logout" << endl;
    cout << "\nEnter your choice: ";
}

// Admin menu
void displayAdminMenu() {
    clearScreen();
    User* currentUser = AuthManager::getCurrentUser();

    cout << "\n╔═══════════════════════════════════════╗" << endl;
    cout << "║           ADMIN MENU                 ║" << endl;
    cout << "╚═══════════════════════════════════════╝" << endl;
    cout << "\nWelcome, " << currentUser->getName() << "!" << endl;
    cout << "\n1. View Pending Recharge Requests" << endl;
    cout << "2. Process Recharge Request" << endl;
    cout << "3. View All Users" << endl;
    cout << "4. Logout" << endl;
    cout << "\nEnter your choice: ";
}

// Login function
void loginUser() {
    clearScreen();
    cout << "\n========== USER LOGIN ==========" << "\n" << endl;
    
    string userId, password;
    cout << "Enter User ID: ";
    cin >> userId;
    cout << "Enter Password: ";
    cin >> password;
    
    if (AuthManager::login(userId, password)) {
        cout << "\n✓ Login successful!\n" << endl;
        pauseScreen();
        
        if (AuthManager::isAdmin()) {
            handleAdminOperations();
        } else {
            handleStudentOperations();
        }
    } else {
        cout << "\n✗ Invalid credentials. Please try again.\n" << endl;
        pauseScreen();
    }
}

// Register new user
void registerNewUser() {
    clearScreen();
    cout << "\n========== USER REGISTRATION ==========" << "\n" << endl;
    
    string userId, password, name, role;
    
    cout << "Enter User ID (e.g., 230042101): ";
    cin >> userId;
    
    // Check if user already exists
    vector<User> users = FileManager::loadUsers();
    for (const auto& user : users) {
        if (user.getUserID() == userId) {
            cout << "\n✗ Error: User ID already exists!\n" << endl;
            pauseScreen();
            return;
        }
    }
    
    cout << "Enter Password: ";
    cin >> password;
    
    cin.ignore();
    cout << "Enter Full Name: ";
    getline(cin, name);
    
    cout << "\nSelect Role:" << endl;
    cout << "1. Student" << endl;
    cout << "2. Teacher" << endl;
    cout << "Enter choice (1 or 2): ";
    
    int roleChoice;
    cin >> roleChoice;
    
    if (roleChoice == 1) {
        role = "STUDENT";
    } else if (roleChoice == 2) {
        role = "TEACHER";
    } else {
        cout << "\n✗ Invalid role selection!\n" << endl;
        pauseScreen();
        return;
    }
    
    User newUser(userId, password, name, role, 0.0);
    
    if (FileManager::addUser(newUser)) {
        cout << "\n✓ Registration successful!" << endl;
        cout << "User ID: " << userId << endl;
        cout << "Role: " << role << "\n" << endl;
    } else {
        cout << "\n✗ Registration failed. Please try again.\n" << endl;
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

        cout << "\n========== PLACE ORDER (WEEKDAY MENU) ==========" << "\n" << endl;
        cout << "Wallet Balance: BDT " << currentUser->getWalletBalance() << "\n" << endl;

        // Weekday selection
        vector<string> days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};

        for (size_t i = 0; i < days.size(); ++i) {
            cout << i+1 << ". " << days[i] << endl;
        }
        cout << "0. Cancel" << endl;

        int dayChoice;
        cout << "\nSelect day (1-5): ";
        cin >> dayChoice;

        if (dayChoice == 0) return;
        if (dayChoice < 1 || dayChoice > 5) {
            cout << "\n✗ Invalid day selection." << endl;
            pauseScreen();
            return;
        }

        string selectedDay = days[dayChoice-1];

        // Meal category
        cout << "\nSelect meal:" << endl;
        cout << "1. Breakfast" << endl;
        cout << "2. Lunch" << endl;
        cout << "3. Dinner" << endl;
        cout << "0. Cancel" << endl;

        int mealChoice;
        cout << "\nEnter choice: ";
        cin >> mealChoice;

        if (mealChoice == 0) return;
        if (mealChoice < 1 || mealChoice > 3) {
            cout << "\n✗ Invalid meal selection." << endl;
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
        cout << "\nMenu for " << selectedDay << " - ";
        if (mealChoice == 1) cout << "Breakfast";
        else if (mealChoice == 2) cout << "Lunch";
        else cout << "Dinner";
        cout << "\n" << endl;

        for (size_t i = 0; i < menuItems.size(); ++i) {
            cout << i+1 << ". " << menuItems[i].name << " - BDT " << menuItems[i].price << endl;
        }
        cout << "0. Finish order / Go back" << endl;

        // Create token and let user add multiple items
        string tokenId = TokenGenerator::generateTokenId();
        Token token(tokenId, currentUser->getUserID());

        while (true) {
            cout << "\nSelect item number to add (0 to finish): ";
            int itemChoice;
            cin >> itemChoice;

            if (itemChoice == 0) break;
            if (itemChoice < 1 || itemChoice > (int)menuItems.size()) {
                cout << "Invalid item selection." << endl;
                continue;
            }

            int quantity;
            cout << "Enter quantity: ";
            cin >> quantity;
            if (quantity <= 0) {
                cout << "Quantity must be at least 1." << endl;
                continue;
            }

            token.addItem(menuItems[itemChoice-1].name, quantity, menuItems[itemChoice-1].price);
            cout << "✓ Added: " << menuItems[itemChoice-1].name << " x" << quantity << endl;
        }

        if (token.getTotalAmount() == 0) {
            cout << "\n✗ No items selected. Order cancelled." << endl;
            pauseScreen();
            return;
        }

        // Check wallet balance
        if (currentUser->getWalletBalance() < token.getTotalAmount()) {
            cout << "\n✗ Insufficient wallet balance!" << endl;
            cout << "Required: BDT " << token.getTotalAmount() << endl;
            cout << "Available: BDT " << currentUser->getWalletBalance() << "\n" << endl;
            pauseScreen();
            return;
        }

        char confirm;
        cout << "\nTotal Amount: BDT " << token.getTotalAmount() << endl;
        cout << "Confirm order and generate token automatically? (y/n): ";
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
                cout << "✓ Order placed successfully and token generated!" << endl;
                cout << "Remaining Balance: BDT " << currentUser->getWalletBalance() << "\n" << endl;
            } else {
                cout << "\n✗ Failed to deduct from wallet. Order not placed." << endl;
            }
        } else {
            cout << "\nOrder cancelled." << endl;
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
                cout << "\n✓ Logged out successfully!\n" << endl;
                pauseScreen();
                return;
            default:
                cout << "\n✗ Invalid choice. Please try again.\n" << endl;
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
    
    cout << "\n========== WALLET BALANCE ==========\n" << endl;
    cout << "User: " << currentUser->getName() << endl;
    cout << "User ID: " << currentUser->getUserID() << endl;
    cout << "\nCurrent Balance: BDT " << fixed << currentUser->getWalletBalance() << "\n" << endl;
    
        pauseScreen();
    }
// Submit recharge request
void submitRechargeRequest() {
    clearScreen();
    User* currentUser = AuthManager::getCurrentUser();
    
    cout << "\n========== SUBMIT RECHARGE REQUEST ==========\n" << endl;
    cout << "Current Balance: BDT " << currentUser->getWalletBalance() << "\n" << endl;
    
    double amount;
    cout << "Enter amount to recharge: BDT ";
    cin >> amount;
    
    char confirm;
    cout << "\nConfirm recharge request of BDT " << amount << "? (y/n): ";
    cin >> confirm;
    
    if (confirm == 'y' || confirm == 'Y') {
        RechargeManager::submitRechargeRequest(currentUser->getUserID(), amount);
    } else {
        cout << "\nRecharge request cancelled.\n" << endl;
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
    
    cout << "\n========== PURCHASE HISTORY ==========\n" << endl;

    // Debug: show how many tokens found
    cout << "Found " << tokens.size() << " token(s) for user " << currentUser->getUserID() << ".\n" << endl;

    if (tokens.empty()) {
        cout << "No purchase history found.\n" << endl;
    } else {
        // Print each token (most recent last as stored in file)
        int idx = 1;
        for (const auto& token : tokens) {
            cout << "----- Order #" << idx++ << " -----\n";
            token.displayToken();
            cout << "--------------------------\n\n";
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
                cout << "\n✓ Logged out successfully!\n" << endl;
                pauseScreen();
                return;
            default:
                cout << "\n✗ Invalid choice. Please try again.\n" << endl;
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
    cout << "\nEnter Request ID to process (or 0 to cancel): ";
    cin >> requestId;
    
    if (requestId == "0") {
        return;
    }
    
    char decision;
    cout << "Approve this request? (y/n): ";
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
    
    cout << "\n========== ALL USERS ==========\n" << endl;
    
    for (const auto& user : users) {
        cout << "User ID: " << user.getUserID() << endl;
        cout << "Name: " << user.getName() << endl;
        cout << "Role: " << user.getRole() << endl;
        cout << "Wallet Balance: BDT " << user.getWalletBalance() << endl;
        cout << "----------------------------------------" << endl;
    }
    
    pauseScreen();
}

// Main function
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
                cout << "\nThank you for using IUT CafeCache System!" << endl;
                cout << "Goodbye!\n" << endl;
                return 0;
            default:
                cout << "\n✗ Invalid choice. Please try again.\n" << endl;
                pauseScreen();
        }
    }
    
    return 0;
}