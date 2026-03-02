#include <iostream>
#include <limits>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include "User.h"
#include "AuthManager.h"
#include "FileManager.h"
#include "RechargeManager.h"
#include "RechargeRequest.h"
#include "Token.h"
#include "TokenGenerator.h"
#include "TerminalSetup.h"
#include "FeedbackManager.h"
#include "SalesAnalytics.h"

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
void manageUserInformation();
void searchUserByID();
void viewStudentInfo();
void viewTeacherInfo();
void submitFeedback();
void viewAllMenuRatings();
void viewFeedbackForDay();
void viewDailyMenuRatings();
void adminViewDailyMenuRatings();
void viewUserFeedbackHistory();
void viewAdminFeedbackHistory();

// Helper function to get food items for a given day and meal
string getFoodItemsForMeal(const string& day, const string& meal) {
    vector<string> menuItems;
    
    if (day == "Monday") {
        if (meal == "Breakfast") menuItems = { "Paratha & Begun Bhaja", "Luchi & Dim", "Tea" };
        else if (meal == "Lunch") menuItems = { "Rice & Chicken Curry", "Daal & Vegetable", "Salad" };
        else menuItems = { "Khichuri", "Fried Fish", "Panta Ilish (small)" };
    } else if (day == "Tuesday") {
        if (meal == "Breakfast") menuItems = { "Panta Bhaat & Bhorta", "Ruti & Omelette", "Tea" };
        else if (meal == "Lunch") menuItems = { "Rice & Beef Curry", "Shorshe Ilish (small)", "Mix Veg" };
        else menuItems = { "Biryani (chicken)", "Raita", "Kebab" };
    } else if (day == "Wednesday") {
        if (meal == "Breakfast") menuItems = { "Shemai", "Chirer Pulao", "Tea" };
        else if (meal == "Lunch") menuItems = { "Rice & Fish Curry", "Daal", "Vegetable Bhaji" };
        else menuItems = { "Kacchi Biryani (small)", "Salad", "Papad" };
    } else if (day == "Thursday") {
        if (meal == "Breakfast") menuItems = { "Ruti & Chana", "Egg Roll", "Tea" };
        else if (meal == "Lunch") menuItems = { "Rice & Chicken Roast", "Mixed Daal", "Aloor Dom" };
        else menuItems = { "Morog Polao (small)", "Salad", "Fried Veg" };
    } else if (day == "Friday") {
        if (meal == "Breakfast") menuItems = { "Luchi & Aloor Dum", "Chana Puri", "Tea" };
        else if (meal == "Lunch") menuItems = { "Rice & Mutton Curry (small)", "Daal", "Green Veg" };
        else menuItems = { "Pulao & Chicken", "Korma (small)", "Raita" };
    }
    
    // Join items with " | "
    string result;
    for (size_t i = 0; i < menuItems.size(); ++i) {
        result += menuItems[i];
        if (i < menuItems.size() - 1) result += " | ";
    }
    return result;
}

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
    printInfo("6. Submit Feedback for Your Orders");
    printInfo("7. View Your Feedback History");
    printInfo("8. View Daily Menu Ratings");
    printInfo("9. Logout");
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
    printInfo("3. Manage User Information");
    printInfo("4. View Menu Feedback & Ratings");
    printInfo("5. View Detailed Feedback History");
    printInfo("6. View Daily Menu Ratings (By Day & Meal Type)");
    printInfo("7. Sales Analytics");
    printInfo("8. Logout");
    printPrompt("\nEnter your choice: ");
}

// Login function
void loginUser() {
    clearScreen();
    printHeader("\n========== USER LOGIN ==========");
    
    string userId, password;
    printPrompt("Enter User ID: ");
    cin >> userId;
    // Clear leftover newline from previous input, then use masked password input
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    printPrompt("Enter Password: ");
    password = readPasswordMasked();
    
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
    // consume leftover newline before reading masked password
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
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
    password = readPasswordMasked();
    
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

        // Define weekday menus: only item names vary per day/meal while prices are fixed per meal type
        // Breakfast price = 40, Lunch price = 70, Dinner price = 70
        vector<string> menuItems;
        double itemPrice = 0.0;

        if (mealChoice == 1) itemPrice = 40.0; // Breakfast
        else itemPrice = 70.0; // Lunch or Dinner

        // Provide different item names per day and meal (prices are uniform per meal type)
        if (selectedDay == "Monday") {
            if (mealChoice == 1) { // Breakfast
                menuItems = { "Paratha & Begun Bhaja", "Luchi & Dim", "Tea" };
            } else if (mealChoice == 2) { // Lunch
                menuItems = { "Rice & Chicken Curry", "Daal & Vegetable", "Salad" };
            } else { // Dinner
                menuItems = { "Khichuri", "Fried Fish", "Panta Ilish (small)" };
            }
        } else if (selectedDay == "Tuesday") {
            if (mealChoice == 1) {
                menuItems = { "Panta Bhaat & Bhorta", "Ruti & Omelette", "Tea" };
            } else if (mealChoice == 2) {
                menuItems = { "Rice & Beef Curry", "Shorshe Ilish (small)", "Mix Veg" };
            } else {
                menuItems = { "Biryani (chicken)", "Raita", "Kebab" };
            }
        } else if (selectedDay == "Wednesday") {
            if (mealChoice == 1) {
                menuItems = { "Shemai", "Chirer Pulao", "Tea" };
            } else if (mealChoice == 2) {
                menuItems = { "Rice & Fish Curry", "Daal", "Vegetable Bhaji" };
            } else {
                menuItems = { "Kacchi Biryani (small)", "Salad", "Papad" };
            }
        } else if (selectedDay == "Thursday") {
            if (mealChoice == 1) {
                menuItems = { "Ruti & Chana", "Egg Roll", "Tea" };
            } else if (mealChoice == 2) {
                menuItems = { "Rice & Chicken Roast", "Mixed Daal", "Aloor Dom" };
            } else {
                menuItems = { "Morog Polao (small)", "Salad", "Fried Veg" };
            }
        } else { // Friday
            if (mealChoice == 1) {
                menuItems = { "Luchi & Aloor Dum", "Chana Puri", "Tea" };
            } else if (mealChoice == 2) {
                menuItems = { "Rice & Mutton Curry (small)", "Daal", "Green Veg" };
            } else {
                menuItems = { "Pulao & Chicken", "Korma (small)", "Raita" };
            }
        }

        // Display items
    string mealLabel = (mealChoice == 1) ? "Breakfast" : (mealChoice == 2 ? "Lunch" : "Dinner");
    printHeader("\nMenu for " + selectedDay + " - " + mealLabel + "\n");

            // Show uniform price once above the menu (package price per plate)
            {
                std::ostringstream priceHdr; priceHdr << fixed << setprecision(2) << itemPrice;
                printLabelValue("Price: ", string("BDT ") + priceHdr.str());
            }

            // Show rating for this day's meal (if any feedback exists)
            {
                auto [avgRating, count] = FeedbackManager::getDayMealRating(selectedDay, mealLabel);
                if (count > 0) {
                    std::ostringstream ratingStr; ratingStr << fixed << setprecision(1) << avgRating;
                    printLabelValue("Average Rating: ", ratingStr.str() + "/5.0 (" + to_string(count) + " ratings)");
                } else {
                    printInfo("Average Rating: No ratings yet");
                }
            }

            // Print menu items in a table: No | ITEM (prices shown above as package price)
            cout << BOLD << WHITE << left << setw(4) << "No" << setw(40) << "ITEM" << RESET << endl;
            cout << GRAY << string(44, '-') << RESET << endl;
            for (size_t i = 0; i < menuItems.size(); ++i) {
                cout << WHITE << left << setw(4) << (to_string(i+1) + ".") << setw(40) << menuItems[i] << RESET << endl;
            }
            printInfo("\nOptions:");
            printInfo("1. Place order for this package");
            printInfo("2. Cancel / Go back");

        // Create token and add the whole package as a single item if confirmed
        string tokenId = TokenGenerator::generateTokenId();
        Token token(tokenId, currentUser->getUserID());

        // Prepare a package name and show confirmation
        string packageName = selectedDay + " " + mealLabel + " Package";

        int opt = 0;
        printPrompt("\nEnter option (1 to place order, 2 to cancel): ");
        cin >> opt;

        if (opt != 1) {
            printInfo("\nOrder cancelled.");
            pauseScreen();
            return;
        }

        int quantity = 1;
        printPrompt("Enter quantity (number of plates, default 1): ");
        cin >> quantity;
        if (quantity <= 0) {
            printError("Quantity must be at least 1. Order cancelled.");
            pauseScreen();
            return;
        }

        // Add package as single token item (packageName) with the uniform meal price
        token.addItem(packageName, quantity, itemPrice);

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
                submitFeedback();
                break;
            case 7:
                viewUserFeedbackHistory();
                break;
            case 8:
                viewDailyMenuRatings();
                break;
            case 9:
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
                manageUserInformation();
                break;
            case 4:
                viewFeedbackForDay();
                break;
            case 5:
                viewAdminFeedbackHistory();
                break;
            case 6:
                adminViewDailyMenuRatings();
                break;
            case 7:
                SalesAnalytics::handleSalesAnalyticsOperations();
                break;
            case 8:
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
    // Show pending requests with an index so admin can choose by number (no more pasting long IDs)
    vector<RechargeRequest> pending = RechargeManager::getPendingRequests();

    if (pending.empty()) {
        printInfo("\nNo pending recharge requests.");
        pauseScreen();
        return;
    }

    printHeader("\n========== PENDING RECHARGE REQUESTS (Select by No) ==========");
    // Print a compact numbered list: No | ShortID | UserID | Amount | Time
    cout << "No  " << "ShortID" << "    " << "UserID" << "    " << "Amount" << "\n";
    cout << string(60, '-') << "\n";
    for (size_t i = 0; i < pending.size(); ++i) {
        string fullId = pending[i].getRequestId();
        string shortId = fullId.substr(0, std::min((size_t)8, fullId.size()));
        // format amount
        std::ostringstream amt; amt << fixed << setprecision(2) << pending[i].getAmount();
        cout << left << setw(3) << (to_string(i+1) + ".") << setw(12) << shortId << setw(12) << pending[i].getUserId() << setw(12) << ("BDT " + amt.str()) << "\n";
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    printInfo("\nOptions:");
    printInfo("  - Single: Enter '5' to accept request #5");
    printInfo("  - Range: Enter '1-10' to accept requests #1 to #10");
    printInfo("  - Multiple: Enter '1,5,7-10,15' for varied selections");
    printInfo("  - Enter '0' to cancel");
    
    string input;
    printPrompt("\nEnter request number(s) to process: ");
    getline(cin, input);

    if (input == "0") return;
    
    try {
        vector<int> selectedIndices = RechargeManager::parseRequestIndices(input, pending.size());
        
        if (selectedIndices.empty()) {
            printError("\n✗ Invalid selection. No valid requests found.");
            pauseScreen();
            return;
        }
        
        // Display selected requests for confirmation
        printInfo("\nSelected requests:");
        for (int idx : selectedIndices) {
            if (idx >= 1 && idx <= (int)pending.size()) {
                string fullId = pending[idx - 1].getRequestId();
                string shortId = fullId.substr(0, std::min((size_t)8, fullId.size()));
                std::ostringstream amt; amt << fixed << setprecision(2) << pending[idx - 1].getAmount();
                cout << "  #" << idx << " | " << shortId << " | " << pending[idx - 1].getUserId() << " | BDT " << amt.str() << "\n";
            }
        }
        
        char decision;
        printPrompt("\nApprove all these requests? (y/n): ");
        cin >> decision;

        if (decision == 'y' || decision == 'Y') {
            int approved = RechargeManager::bulkApproveRequests(selectedIndices);
            printSuccess("\n✓ " + to_string(approved) + " request(s) approved successfully!");
        } else {
            int rejected = RechargeManager::bulkRejectRequests(selectedIndices);
            printSuccess("\n✓ " + to_string(rejected) + " request(s) rejected.");
        }
    } catch (const exception& e) {
        printError("\n✗ Error parsing input: " + string(e.what()));
    }

    pauseScreen();
}

// Manage User Information - Submenu for searching and browsing users
void manageUserInformation() {
    int choice;
    while (true) {
        clearScreen();
        printHeader("\n========== MANAGE USER INFORMATION ==========");
        printInfo("1. Search User by ID");
        printInfo("2. View Student Information (Paginated)");
        printInfo("3. View Teacher Information (Paginated)");
        printInfo("4. Back to Admin Menu");
        printPrompt("\nEnter your choice: ");
        cin >> choice;
        
        switch (choice) {
            case 1:
                searchUserByID();
                break;
            case 2:
                viewStudentInfo();
                break;
            case 3:
                viewTeacherInfo();
                break;
            case 4:
                return;
            default:
                printError("\n✗ Invalid choice. Please try again.\n");
                pauseScreen();
        }
    }
}

// Search for a specific user by ID
void searchUserByID() {
    clearScreen();
    vector<User> users = FileManager::loadUsers();
    
    printHeader("\n========== SEARCH USER BY ID ==========");
    
    string searchId;
    printPrompt("Enter User ID to search: ");
    cin >> searchId;
    
    User* foundUser = FileManager::findUser(searchId, users);
    
    if (foundUser != nullptr) {
        clearScreen();
        printHeader("\n========== USER DETAILS ==========");
        printLabelValue("User ID: ", foundUser->getUserID());
        printLabelValue("Name: ", foundUser->getName());
        printLabelValue("Role: ", foundUser->getRole());
        printLabelValue("Wallet Balance: BDT ", to_string(foundUser->getWalletBalance()));
        printSeparator();
    } else {
        printError("\n✗ User not found with ID: " + searchId);
    }
    
    pauseScreen();
}

// View Student Information with Pagination
void viewStudentInfo() {
    clearScreen();
    vector<User> users = FileManager::loadUsers();
    
    // Filter only students
    vector<User> students;
    for (const auto& user : users) {
        if (user.getRole() == "STUDENT") {
            students.push_back(user);
        }
    }
    
    // Sort by user ID (ascending)
    sort(students.begin(), students.end(), [](const User& a, const User& b) {
        return a.getUserID() < b.getUserID();
    });
    
    if (students.empty()) {
        printError("\n✗ No students found in the system.");
        pauseScreen();
        return;
    }
    
    int itemsPerPage = 10;
    int currentPage = 0;
    int totalPages = (students.size() + itemsPerPage - 1) / itemsPerPage;
    
    while (true) {
        clearScreen();
        printHeader("\n========== STUDENT INFORMATION (Paginated) ==========");
        printLabelValue("Page: ", to_string(currentPage + 1) + " / " + to_string(totalPages));
        printLabelValue("Total Students: ", to_string(students.size()));
        printSeparator();
        
        int startIdx = currentPage * itemsPerPage;
        int endIdx = min(startIdx + itemsPerPage, (int)students.size());
        
        for (int i = startIdx; i < endIdx; ++i) {
            cout << "\n" << (i + 1) << ". ";
            printLabelValue("ID: ", students[i].getUserID());
            cout << "   ";
            printLabelValue("Name: ", students[i].getName());
            cout << "   ";
            printLabelValue("Balance: BDT ", to_string(students[i].getWalletBalance()));
        }
        
        printSeparator();
        printInfo("\nNavigation Options:");
        if (currentPage > 0) {
            printInfo("'p' - Previous Page");
        }
        if (currentPage < totalPages - 1) {
            printInfo("'n' - Next Page");
        }
        printInfo("'q' - Quit");
        
        printPrompt("\nEnter your choice: ");
        char choice;
        cin >> choice;
        
        if (choice == 'n' || choice == 'N') {
            if (currentPage < totalPages - 1) {
                currentPage++;
            } else {
                printError("\n✗ Already on the last page.");
                pauseScreen();
            }
        } else if (choice == 'p' || choice == 'P') {
            if (currentPage > 0) {
                currentPage--;
            } else {
                printError("\n✗ Already on the first page.");
                pauseScreen();
            }
        } else if (choice == 'q' || choice == 'Q') {
            break;
        } else {
            printError("\n✗ Invalid choice.");
            pauseScreen();
        }
    }
}

// View Teacher Information with Pagination
void viewTeacherInfo() {
    clearScreen();
    vector<User> users = FileManager::loadUsers();
    
    // Filter only teachers
    vector<User> teachers;
    for (const auto& user : users) {
        if (user.getRole() == "TEACHER") {
            teachers.push_back(user);
        }
    }
    
    // Sort by user ID (ascending)
    sort(teachers.begin(), teachers.end(), [](const User& a, const User& b) {
        return a.getUserID() < b.getUserID();
    });
    
    if (teachers.empty()) {
        printError("\n✗ No teachers found in the system.");
        pauseScreen();
        return;
    }
    
    int itemsPerPage = 10;
    int currentPage = 0;
    int totalPages = (teachers.size() + itemsPerPage - 1) / itemsPerPage;
    
    while (true) {
        clearScreen();
        printHeader("\n========== TEACHER INFORMATION (Paginated) ==========");
        printLabelValue("Page: ", to_string(currentPage + 1) + " / " + to_string(totalPages));
        printLabelValue("Total Teachers: ", to_string(teachers.size()));
        printSeparator();
        
        int startIdx = currentPage * itemsPerPage;
        int endIdx = min(startIdx + itemsPerPage, (int)teachers.size());
        
        for (int i = startIdx; i < endIdx; ++i) {
            cout << "\n" << (i + 1) << ". ";
            printLabelValue("ID: ", teachers[i].getUserID());
            cout << "   ";
            printLabelValue("Name: ", teachers[i].getName());
            cout << "   ";
            printLabelValue("Balance: BDT ", to_string(teachers[i].getWalletBalance()));
        }
        
        printSeparator();
        printInfo("\nNavigation Options:");
        if (currentPage > 0) {
            printInfo("'p' - Previous Page");
        }
        if (currentPage < totalPages - 1) {
            printInfo("'n' - Next Page");
        }
        printInfo("'q' - Quit");
        
        printPrompt("\nEnter your choice: ");
        char choice;
        cin >> choice;
        
        if (choice == 'n' || choice == 'N') {
            if (currentPage < totalPages - 1) {
                currentPage++;
            } else {
                printError("\n✗ Already on the last page.");
                pauseScreen();
            }
        } else if (choice == 'p' || choice == 'P') {
            if (currentPage > 0) {
                currentPage--;
            } else {
                printError("\n✗ Already on the first page.");
                pauseScreen();
            }
        } else if (choice == 'q' || choice == 'Q') {
            break;
        } else {
            printError("\n✗ Invalid choice.");
            pauseScreen();
        }
    }
}

// Submit feedback for a day's menu
void submitFeedback() {
    clearScreen();
    User* currentUser = AuthManager::getCurrentUser();
    
    printHeader("\n========== SUBMIT FEEDBACK FOR YOUR ORDERS ==========");
    printLabelValue("User: ", currentUser->getName());
    
    // Get all tokens for this user
    vector<Token> userTokens = FileManager::getUserTokens(currentUser->getUserID());
    
    if (userTokens.empty()) {
        printInfo("\nYou have no orders yet. Please place an order first before submitting feedback.");
        pauseScreen();
        return;
    }
    
    // Show orders without feedback
    printInfo("\nYour Orders (without feedback):\n");
    vector<Token> tokensWithoutFeedback;
    int idx = 1;
    
    for (const auto& token : userTokens) {
        if (!FeedbackManager::hasFeedbackForToken(currentUser->getUserID(), token.getTokenId())) {
            tokensWithoutFeedback.push_back(token);
            cout << to_string(idx) << ". Token ID: " << token.getTokenId().substr(0, 8) << " | Amount: BDT " << token.getTotalAmount() << endl;
            idx++;
        }
    }
    
    if (tokensWithoutFeedback.empty()) {
        printInfo("\nYou have already submitted feedback for all your orders. Thank you!");
        pauseScreen();
        return;
    }
    
    printInfo("0. Cancel");
    
    int orderChoice;
    printPrompt("\nSelect an order to give feedback (enter number): ");
    cin >> orderChoice;
    
    if (orderChoice == 0) return;
    if (orderChoice < 1 || orderChoice > (int)tokensWithoutFeedback.size()) {
        printError("\n✗ Invalid selection.");
        pauseScreen();
        return;
    }
    
    Token selectedToken = tokensWithoutFeedback[orderChoice - 1];
    string selectedTokenId = selectedToken.getTokenId();
    
    // Get rating (1-5)
    int rating;
    printPrompt("\nRate your order (1-5): ");
    cin >> rating;
    
    if (rating < 1 || rating > 5) {
        printError("\n✗ Invalid rating. Please enter a number between 1 and 5.");
        pauseScreen();
        return;
    }
    
    // Get remark
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string remark;
    printPrompt("Write a comment about your order (max 200 characters): ");
    getline(cin, remark);
    
    if (remark.empty()) {
        printError("\n✗ Comment cannot be empty.");
        pauseScreen();
        return;
    }
    
    // Submit feedback
    if (FeedbackManager::submitFeedback(currentUser->getUserID(), selectedTokenId, rating, remark)) {
        printSuccess("\n✓ Thank you! Feedback submitted successfully!");
        printLabelValue("Token ID: ", selectedTokenId);
        printLabelValue("Rating: ", to_string(rating) + "/5");
        printLabelValue("Comment: ", remark);
    } else {
        printError("\n✗ Failed to submit feedback. Please try again.");
    }
    
    pauseScreen();
}

// View all feedback user has submitted
void viewAllMenuRatings() {
    clearScreen();
    User* currentUser = AuthManager::getCurrentUser();
    
    printHeader("\n========== YOUR FEEDBACK HISTORY ==========");
    printLabelValue("User: ", currentUser->getName());
    
    vector<Feedback> userFeedbacks = FeedbackManager::getUserFeedback(currentUser->getUserID());
    
    if (userFeedbacks.empty()) {
        printInfo("\nYou have not submitted any feedback yet.");
        pauseScreen();
        return;
    }
    
    printInfo("\nYour Submitted Feedback:\n");
    
    for (size_t i = 0; i < userFeedbacks.size(); ++i) {
        cout << BOLD << CYAN << "Feedback #" << (i+1) << ":" << RESET << endl;
        printLabelValue("Token ID: ", userFeedbacks[i].getTokenId().substr(0, 8));
        printLabelValue("Rating: ", to_string(userFeedbacks[i].getRating()) + "/5");
        printLabelValue("Comment: ", userFeedbacks[i].getRemark());
        time_t t = userFeedbacks[i].getTimestamp();
        printLabelValue("Submitted: ", string(ctime(&t)));
        printSeparator();
    }
    
    pauseScreen();
}

// View daily menu ratings for all days and meals
void viewDailyMenuRatings() {
    clearScreen();
    User* currentUser = AuthManager::getCurrentUser();
    
    printHeader("\n========== DAILY MENU RATINGS & REVIEWS ==========");
    printLabelValue("User: ", currentUser->getName());
    
    vector<string> days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
    vector<string> meals = {"Breakfast", "Lunch", "Dinner"};
    
    printInfo("\nMenu Ratings Across All Days and Meals:\n");
    
    // Print table header
    cout << BOLD << WHITE;
    cout << left << setw(15) << "Day" 
         << left << setw(12) << "Meal Type" 
         << left << setw(45) << "Food Items" 
         << left << setw(20) << "Rating" << RESET << endl;
    cout << string(92, '-') << endl;
    
    for (const auto& day : days) {
        for (size_t i = 0; i < meals.size(); ++i) {
            const auto& meal = meals[i];
            auto [avgRating, count] = FeedbackManager::getDayMealRating(day, meal);
            string foodItems = getFoodItemsForMeal(day, meal);
            
            // Truncate food items if too long
            if (foodItems.length() > 42) {
                foodItems = foodItems.substr(0, 42) + "...";
            }
            
            ostringstream ratingStr;
            if (count > 0) {
                ratingStr << fixed << setprecision(1) << avgRating << "/5.0 (" << count << ")";
            } else {
                ratingStr << "No ratings";
            }
            
            // Print day only on first meal row
            if (i == 0) {
                cout << BOLD << YELLOW << left << setw(15) << day << RESET
                     << left << setw(12) << meal
                     << left << setw(45) << foodItems
                     << left << setw(20) << ratingStr.str() << endl;
            } else {
                cout << left << setw(15) << ""
                     << left << setw(12) << meal
                     << left << setw(45) << foodItems
                     << left << setw(20) << ratingStr.str() << endl;
            }
        }
        cout << endl;
    }
    
    pauseScreen();
}

// Admin: View feedback for all orders
void viewFeedbackForDay() {
    clearScreen();
    
    printHeader("\n========== ADMIN: ORDER FEEDBACK & RATINGS ==========");
    
    vector<Token> allTokens = FileManager::loadTokens();
    vector<Feedback> allFeedbacks = FileManager::loadFeedback();
    
    if (allFeedbacks.empty()) {
        printInfo("\nNo feedback submitted yet.");
        pauseScreen();
        return;
    }
    
    // Group feedbacks by token
    printInfo("\nAll Order Feedback:\n");
    
    for (const auto& token : allTokens) {
        vector<Feedback> tokenFeedbacks = FeedbackManager::getFeedbackForToken(token.getTokenId());
        
        if (!tokenFeedbacks.empty()) {
            cout << BOLD << CYAN << "Token: " << token.getTokenId().substr(0, 8) << " | User: " << token.getUserId() << RESET << endl;
            printLabelValue("Total Amount: ", "BDT " + to_string(token.getTotalAmount()));
            
            for (size_t i = 0; i < tokenFeedbacks.size(); ++i) {
                cout << "  Feedback:" << endl;
                cout << "    Rating: " << tokenFeedbacks[i].getRating() << "/5" << endl;
                cout << "    Comment: " << tokenFeedbacks[i].getRemark() << endl;
            }
            
            printSeparator();
        }
    }
    
    pauseScreen();
}

// Admin: View daily menu ratings by day and meal type
void adminViewDailyMenuRatings() {
    clearScreen();
    
    printHeader("\n========== ADMIN: DAILY MENU RATINGS ANALYSIS ==========");
    printInfo("Assessment of food quality by day and meal type\n");
    
    vector<string> days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
    vector<string> meals = {"Breakfast", "Lunch", "Dinner"};
    
    // Print header for table
    cout << left << setw(12) << "Day" << setw(15) << "Breakfast" << setw(15) << "Lunch" << setw(15) << "Dinner" << endl;
    cout << string(57, '-') << endl;
    
    for (const auto& day : days) {
        cout << left << setw(12) << day;
        
        for (const auto& meal : meals) {
            auto [avgRating, count] = FeedbackManager::getDayMealRating(day, meal);
            
            ostringstream ratingStr;
            if (count > 0) {
                ratingStr << fixed << setprecision(1) << avgRating << "/5.0 (" << count << ")";
            } else {
                ratingStr << "No ratings";
            }
            
            cout << left << setw(15) << ratingStr.str();
        }
        
        cout << endl;
    }
    
    cout << endl;
    printInfo("\nInterpretation:");
    printInfo("  >= 4.5: Excellent - Keep the current food quality");
    printInfo("  4.0-4.4: Good - Minor improvements needed");
    printInfo("  3.5-3.9: Fair - Consider menu changes");
    printInfo("  < 3.5: Poor - Urgent menu revision needed");
    printInfo("  (N) = Number of user ratings for that meal");
    
    pauseScreen();
}

// Admin: View feedback and remarks for a specific day - DEPRECATED

int main() {
    // Initialize console for Windows UTF-8 and ANSI color support
    initializeConsole();
    
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
}

// View user's feedback history
void viewUserFeedbackHistory() {
    clearScreen();
    User* currentUser = AuthManager::getCurrentUser();
    auto userFeedbackHistory = FeedbackManager::getUserFeedbackHistory(currentUser->getUserID());
    
    printHeader("\n========== YOUR FEEDBACK HISTORY (DETAILED) ==========");
    printLabelValue("User: ", currentUser->getName());
    printLabelValue("User ID: ", currentUser->getUserID());
    printInfo("\nYour Feedback History:\n");
    
    // Print table header
    cout << BOLD << WHITE;
    cout << left << setw(5) << "No."
         << left << setw(8) << "Token#"
         << left << setw(16) << "Food Item"
         << left << setw(18) << "Day & Meal"
         << left << setw(8) << "Rating"
         << left << setw(28) << "Your Comment" << RESET << endl;
    cout << string(83, '-') << endl;
    
    int count = 1;
    for (const auto& [feedback, token] : userFeedbackHistory) {
        auto [day, meal] = token.extractDayAndMeal();
        string dayMeal = day + " " + meal;
        string remark = feedback.getRemark();
        string tokenId = feedback.getTokenId();
        
        // Extract last 3 digits of token
        string tokenNum = tokenId;
        if (tokenNum.find("TKN") == 0) {
            tokenNum = tokenNum.substr(3);
        }
        if (tokenNum.length() >= 3) {
            tokenNum = tokenNum.substr(tokenNum.length() - 3);
        }
        
        // Get food item from menu based on package name
        string packageName = token.getItems()[0].itemName;
        string foodItem = FileManager::getFoodItemsFromPackage(packageName);
        
        // If menu lookup failed, extract from token items
        if (foodItem == "N/A") {
            auto items = token.getItems();
            if (items.size() > 1) {
                foodItem = items[1].itemName;
            } else if (items.size() > 0) {
                foodItem = items[0].itemName;
            }
        }
        
        // Clean up and extract first word
        while (!foodItem.empty() && foodItem[0] == ' ') foodItem = foodItem.substr(1);
        while (!foodItem.empty() && foodItem.back() == ' ') foodItem.pop_back();
        
        size_t spacePos = foodItem.find(" ");
        if (spacePos != string::npos) {
            foodItem = foodItem.substr(0, spacePos);
        }
        
        if (foodItem.length() > 12) {
            foodItem = foodItem.substr(0, 10) + "...";
        }
        
        // Truncate remark if too long
        if (remark.length() > 25) {
            remark = remark.substr(0, 25) + "...";
        }
        
        cout << left << setw(5) << (to_string(count) + ".")
             << left << setw(8) << tokenNum
             << left << setw(16) << foodItem
             << left << setw(18) << dayMeal
             << BOLD << CYAN << left << setw(8) << (to_string(feedback.getRating()) + "/5") << RESET
             << left << setw(28) << remark << endl;
        
        count++;
    }
    
    cout << endl;
    printInfo("Total Feedback Submitted: " + to_string(userFeedbackHistory.size()));
    
    pauseScreen();
}

// View admin detailed feedback history
void viewAdminFeedbackHistory() {
    clearScreen();
    auto allFeedbackHistory = FeedbackManager::getAllFeedbackHistory();
    
    if (allFeedbackHistory.empty()) {
        printInfo("\nNo feedback submitted yet.");
        pauseScreen();
        return;
    }
    
    printHeader("\n========== ALL FEEDBACK HISTORY (DETAILED) ==========");
    printInfo("\nFeedback History:\n");
    
    // Print table header
    cout << BOLD << WHITE;
    cout << left << setw(5) << "No."
         << left << setw(8) << "Token#"
         << left << setw(16) << "Food Item"
         << left << setw(12) << "User ID"
         << left << setw(15) << "Day & Meal"
         << left << setw(8) << "Rating"
         << left << setw(20) << "Feedback" << RESET << endl;
    cout << string(84, '-') << endl;
    
    int count = 1;
    double totalRating = 0.0;
    int excellentCount = 0, goodCount = 0, fairCount = 0, poorCount = 0;
    
    for (const auto& [feedback, token, dayMeal] : allFeedbackHistory) {
        string userId = feedback.getUserId();
        string remark = feedback.getRemark();
        string tokenId = feedback.getTokenId();
        
        // Extract last 3 digits of token
        string tokenNum = tokenId;
        if (tokenNum.find("TKN") == 0) {
            tokenNum = tokenNum.substr(3);
        }
        if (tokenNum.length() >= 3) {
            tokenNum = tokenNum.substr(tokenNum.length() - 3);
        }
        
        // Get food item from menu based on package name
        string packageName = token.getItems()[0].itemName;
        string foodItem = FileManager::getFoodItemsFromPackage(packageName);
        
        // If menu lookup failed, extract from token items
        if (foodItem == "N/A") {
            auto items = token.getItems();
            if (items.size() > 1) {
                foodItem = items[1].itemName;
            } else if (items.size() > 0) {
                foodItem = items[0].itemName;
            }
        }
        
        // Clean up and extract first word
        while (!foodItem.empty() && foodItem[0] == ' ') foodItem = foodItem.substr(1);
        while (!foodItem.empty() && foodItem.back() == ' ') foodItem.pop_back();
        
        size_t spacePos = foodItem.find(" ");
        if (spacePos != string::npos) {
            foodItem = foodItem.substr(0, spacePos);
        }
        
        if (foodItem.length() > 12) {
            foodItem = foodItem.substr(0, 10) + "...";
        }
        
        // Truncate remark if too long
        string displayRemark = remark;
        if (displayRemark.length() > 17) {
            displayRemark = displayRemark.substr(0, 17) + "...";
        }
        
        // Color code the rating
        string ratingStr = to_string(feedback.getRating()) + "/5";
        string ratingColor = RESET;
        int rating = feedback.getRating();
        totalRating += rating;
        
        if (rating == 5) {
            ratingColor = GREEN;
            excellentCount++;
        } else if (rating == 4) {
            ratingColor = GREEN;
            goodCount++;
        } else if (rating == 3) {
            ratingColor = YELLOW;
            fairCount++;
        } else {
            ratingColor = RED;
            poorCount++;
        }
        
        cout << left << setw(5) << (to_string(count) + ".")
             << left << setw(8) << tokenNum
             << left << setw(16) << foodItem
             << left << setw(12) << userId
             << left << setw(15) << dayMeal
             << ratingColor << BOLD << left << setw(8) << ratingStr << RESET
             << left << setw(20) << displayRemark << endl;
        
        count++;
    }
    
    cout << endl;
    printInfo("Total Feedback: " + to_string(allFeedbackHistory.size()));
    
    double avgRating = totalRating / allFeedbackHistory.size();
    cout << "Average Rating: " << fixed << setprecision(2) << avgRating << "/5" << endl;
    cout << "Excellent (5): " << excellentCount << " | Good (4): " << goodCount 
         << " | Fair (3): " << fairCount << " | Poor (1-2): " << poorCount << endl;
    
    pauseScreen();
}