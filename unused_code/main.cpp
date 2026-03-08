#include <iostream>
#include <limits>
#include <vector>
#include "Utils.h"
#include "User.h"
#include "MenuItem.h"
#include "Transaction.h"
#include "RechargeRequest.h"
#include "Feedback.h"
#include "SalesAnalytics.h"

using namespace std;

struct OrderItem {
    MenuItem* item;
    int quantity;
    double subtotal;
};

void studentMenu(User* user);
void adminMenu(User* admin);
void orderFood(User* user, const string& category);

void showMainMenu() {
    Utils::clearScreen();
    cout << "\n" << string(60, '=') << endl;
    cout << "        IUT CAFECACHE - CAFETERIA MANAGEMENT" << endl;
    cout << string(60, '=') << endl;
    cout << "\n1. Login" << endl;
    cout << "2. Register" << endl;
    cout << "3. Exit" << endl;
    cout << string(60, '=') << endl;
    cout << "Enter choice: ";
}

void studentMenu(User* user) {
    while (true) {
        Utils::clearScreen();
        cout << "\n" << string(60, '=') << endl;
        cout << "STUDENT MENU - Welcome, " << user->username << endl;
        cout << string(60, '=') << endl;
        cout << "Wallet Balance: Tk " << fixed << setprecision(2) << user->balance << endl;
        cout << "\n1. View Wallet" << endl;
        cout << "2. Order Food" << endl;
        cout << "3. Transaction Log" << endl;
        cout << "4. Recharge Request" << endl;
        cout << "5. Submit Feedback" << endl;
        cout << "6. View All Feedbacks" << endl;
        cout << "7. Logout" << endl;
        cout << string(60, '=') << endl;
        cout << "Enter choice: ";

        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 1) {
            Utils::clearScreen();
            user->viewWallet();
            Utils::pause();
        }
        else if (choice == 2) {
            Utils::clearScreen();
            cout << "\n" << string(60, '=') << endl;
            cout << "SELECT MEAL TYPE" << endl;
            cout << string(60, '=') << endl;
            cout << "B. Breakfast" << endl;
            cout << "L. Lunch" << endl;
            cout << "D. Dinner" << endl;
            cout << "X. Cancel" << endl;
            cout << string(60, '=') << endl;
            cout << "Enter choice: ";

            char mealChoice;
            cin >> mealChoice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (mealChoice == 'B' || mealChoice == 'b') {
                orderFood(user, "breakfast");
            } else if (mealChoice == 'L' || mealChoice == 'l') {
                orderFood(user, "lunch");
            } else if (mealChoice == 'D' || mealChoice == 'd') {
                orderFood(user, "dinner");
            }
        }
        else if (choice == 3) {
            Utils::clearScreen();
            Transaction::viewUserTransactions(user->studentId);
            Utils::pause();
        }
        else if (choice == 4) {
            Utils::clearScreen();
            cout << "\n" << string(60, '=') << endl;
            cout << "RECHARGE REQUEST" << endl;
            cout << string(60, '=') << endl;
            cout << "Enter recharge amount (Tk): ";
            double amount;
            cin >> amount;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            RechargeRequest::createRequest(user->studentId, user->username, amount);
            Utils::pause();
        }
        else if (choice == 5) {
            Utils::clearScreen();
            cout << "\n" << string(60, '=') << endl;
            cout << "SUBMIT FEEDBACK" << endl;
            cout << string(60, '=') << endl;
            cout << "Select meal type:" << endl;
            cout << "1. Breakfast" << endl;
            cout << "2. Lunch" << endl;
            cout << "3. Dinner" << endl;
            cout << "Choice: ";
            
            int mealType;
            cin >> mealType;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            string category;
            if (mealType == 1) category = "breakfast";
            else if (mealType == 2) category = "lunch";
            else if (mealType == 3) category = "dinner";
            else {
                cout << "\nError: Invalid choice!" << endl;
                Utils::pause();
                continue;
            }

            string day = Utils::getDayOfWeek();
            cout << "\nEnter your feedback (max 250 characters):" << endl;
            string feedbackText;
            getline(cin, feedbackText);

            Feedback::submitFeedback(user->studentId, category, day, feedbackText);
            Utils::pause();
        }
        else if (choice == 6) {
            Utils::clearScreen();
            cout << "\n" << string(60, '=') << endl;
            cout << "VIEW FEEDBACKS" << endl;
            cout << string(60, '=') << endl;
            cout << "1. All Feedbacks" << endl;
            cout << "2. Breakfast Feedbacks" << endl;
            cout << "3. Lunch Feedbacks" << endl;
            cout << "4. Dinner Feedbacks" << endl;
            cout << "Choice: ";
            
            int fbChoice;
            cin >> fbChoice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            Utils::clearScreen();
            if (fbChoice == 1) {
                Feedback::viewAllFeedbacks();
            } else if (fbChoice == 2) {
                Feedback::viewFeedbacksByCategory("breakfast");
            } else if (fbChoice == 3) {
                Feedback::viewFeedbacksByCategory("lunch");
            } else if (fbChoice == 4) {
                Feedback::viewFeedbacksByCategory("dinner");
            }
            Utils::pause();
        }
        else if (choice == 7) {
            Utils::clearScreen();
            cout << "\nLogging out... Goodbye, " << user->username << "!" << endl;
            Utils::pause();
            break;
        }
        else {
            cout << "\nError: Invalid choice!" << endl;
            Utils::pause();
        }
    }
}

void orderFood(User* user, const string& category) {
    vector<OrderItem> cart;
    double totalAmount = 0.0;

    while (true) {
        Utils::clearScreen();
        MenuItem::viewMenuByCategory(category);

        if (!cart.empty()) {
            cout << "\n" << string(80, '=') << endl;
            cout << "CURRENT ORDER" << endl;
            cout << string(80, '=') << endl;
            for (size_t i = 0; i < cart.size(); i++) {
                cout << cart[i].item->name << " x" << cart[i].quantity 
                     << " = Tk " << fixed << setprecision(2) << cart[i].subtotal << endl;
            }
            cout << string(80, '-') << endl;
            cout << "Total: Tk " << fixed << setprecision(2) << totalAmount << endl;
            cout << string(80, '=') << endl;
        }

        cout << "\nEnter Item ID to add (0 to finish ordering): ";
        int itemId;
        cin >> itemId;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (itemId == 0) {
            if (cart.empty()) {
                cout << "\nNo items in cart. Cancelling order." << endl;
                Utils::pause();
                return;
            }
            break;
        }

        MenuItem* item = MenuItem::getItem(itemId);
        if (!item) {
            cout << "\nError: Item not found!" << endl;
            Utils::pause();
            continue;
        }

        if (item->category != category) {
            cout << "\nError: Item does not belong to " << category << "!" << endl;
            delete item;
            Utils::pause();
            continue;
        }

        if (item->stock <= 0) {
            cout << "\nError: Item out of stock!" << endl;
            delete item;
            Utils::pause();
            continue;
        }

        cout << "Enter quantity: ";
        int quantity;
        cin >> quantity;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (quantity <= 0 || quantity > item->stock) {
            cout << "\nError: Invalid quantity!" << endl;
            delete item;
            Utils::pause();
            continue;
        }

        OrderItem orderItem;
        orderItem.item = item;
        orderItem.quantity = quantity;
        orderItem.subtotal = item->price * quantity;

        cart.push_back(orderItem);
        totalAmount += orderItem.subtotal;

        cout << "\n" << item->name << " x" << quantity << " added to cart!" << endl;
        cout << "Press Enter to add more items, or 0 in next prompt to finish...";
        cin.get();
    }

    Utils::clearScreen();
    cout << "\n" << string(80, '=') << endl;
    cout << "ORDER SUMMARY" << endl;
    cout << string(80, '=') << endl;
    
    for (const auto& orderItem : cart) {
        cout << orderItem.item->name << " x" << orderItem.quantity 
             << " @ Tk " << fixed << setprecision(2) << orderItem.item->price 
             << " = Tk " << orderItem.subtotal << endl;
    }
    
    cout << string(80, '-') << endl;
    cout << "Total Amount: Tk " << fixed << setprecision(2) << totalAmount << endl;
    cout << string(80, '=') << endl;

    if (user->balance < totalAmount) {
        cout << "\nError: Insufficient balance!" << endl;
        cout << "Your Balance: Tk " << fixed << setprecision(2) << user->balance << endl;
        cout << "Required: Tk " << fixed << setprecision(2) << totalAmount << endl;
        
        for (auto& orderItem : cart) {
            delete orderItem.item;
        }
        Utils::pause();
        return;
    }

    if (!Utils::confirmAction("Confirm and place this order?")) {
        cout << "\nOrder cancelled." << endl;
        for (auto& orderItem : cart) {
            delete orderItem.item;
        }
        Utils::pause();
        return;
    }

    user->balance -= totalAmount;
    user->updateBalance();

    for (auto& orderItem : cart) {
        orderItem.item->stock -= orderItem.quantity;
        orderItem.item->updateStock();

        Transaction::recordTransaction(user->studentId, orderItem.item->name, 
                                      category, orderItem.quantity, orderItem.subtotal);
        
        delete orderItem.item;
    }

    int tokenNumber = Utils::generateRandomId() % 1000 + 1;

    Utils::clearScreen();
    cout << "\n" << string(80, '=') << endl;
    cout << "ORDER PLACED SUCCESSFULLY!" << endl;
    cout << string(80, '=') << endl;
    cout << "Token Number: #" << tokenNumber << endl;
    cout << "Total Items: " << cart.size() << endl;
    cout << "Amount Paid: Tk " << fixed << setprecision(2) << totalAmount << endl;
    cout << "Remaining Balance: Tk " << fixed << setprecision(2) << user->balance << endl;
    cout << string(80, '=') << endl;
    cout << "Please collect your order when token #" << tokenNumber << " is called!" << endl;

    Utils::pause();
}

void adminMenu(User* admin) {
    while (true) {
        Utils::clearScreen();
        cout << "\n" << string(60, '=') << endl;
        cout << "ADMIN MENU - Welcome, " << admin->username << endl;
        cout << string(60, '=') << endl;
        cout << "\n1. View Recharge Requests" << endl;
        cout << "2. Approve/Reject Recharge Request" << endl;
        cout << "3. View All Transactions" << endl;
        cout << "4. Search User Transaction Log" << endl;
        cout << "5. View All Feedbacks" << endl;
        cout << "6. Sales Analytics" << endl;
        cout << "7. View Sales Graph" << endl;
        cout << "8. Logout" << endl;
        cout << string(60, '=') << endl;
        cout << "Enter choice: ";

        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 1) {
            Utils::clearScreen();
            vector<RechargeRequest::RequestInfo> requests = RechargeRequest::viewPendingRequests();
            Utils::pause();
        }
        else if (choice == 2) {
            Utils::clearScreen();
            vector<RechargeRequest::RequestInfo> requests = RechargeRequest::viewPendingRequests();
            
            if (requests.empty()) {
                Utils::pause();
                continue;
            }

            cout << "\nEnter selection (e.g., 1 or 1-5 or 1,3,5): ";
            string selection;
            getline(cin, selection);

            vector<int> selectedIndices;
            
            // Parse selection
            try {
                if (selection.find('-') != string::npos) {
                    // Range selection (e.g., 1-5)
                    size_t dashPos = selection.find('-');
                    int start = stoi(selection.substr(0, dashPos));
                    int end = stoi(selection.substr(dashPos + 1));
                    
                    if (start < 1 || end > (int)requests.size() || start > end) {
                        cout << "\nError: Invalid range!" << endl;
                        Utils::pause();
                        continue;
                    }
                    
                    for (int i = start; i <= end; i++) {
                        selectedIndices.push_back(requests[i-1].requestId);
                    }
                } else if (selection.find(',') != string::npos) {
                    // Multiple selection (e.g., 1,3,5)
                    stringstream ss;
                    ss.str(selection);
                    string token;
                    while (getline(ss, token, ',')) {
                        int index = stoi(Utils::trim(token));
                        if (index < 1 || index > (int)requests.size()) {
                            cout << "\nError: Invalid index " << index << endl;
                            continue;
                        }
                        selectedIndices.push_back(requests[index-1].requestId);
                    }
                } else {
                    // Single selection
                    int index = stoi(selection);
                    if (index < 1 || index > (int)requests.size()) {
                        cout << "\nError: Invalid selection!" << endl;
                        Utils::pause();
                        continue;
                    }
                    selectedIndices.push_back(requests[index-1].requestId);
                }
            } catch (...) {
                cout << "\nError: Invalid input format!" << endl;
                Utils::pause();
                continue;
            }

            if (selectedIndices.empty()) {
                cout << "\nError: No valid selections!" << endl;
                Utils::pause();
                continue;
            }

            cout << "\nSelected " << selectedIndices.size() << " request(s)" << endl;
            cout << "1. Approve selected request(s)" << endl;
            cout << "2. Reject selected request(s)" << endl;
            cout << "Choice: ";
            int action;
            cin >> action;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (action == 1) {
                RechargeRequest::approveRequests(selectedIndices);
            } else if (action == 2) {
                RechargeRequest::rejectRequests(selectedIndices);
            } else {
                cout << "\nError: Invalid choice!" << endl;
            }
            Utils::pause();
        }
        else if (choice == 3) {
            Utils::clearScreen();
            Transaction::viewAllTransactions();
            Utils::pause();
        }
        else if (choice == 4) {
            Utils::clearScreen();
            cout << "\n" << string(60, '=') << endl;
            cout << "SEARCH USER TRANSACTION LOG" << endl;
            cout << string(60, '=') << endl;
            cout << "Enter Student ID or Username: ";
            string identifier;
            getline(cin, identifier);

            User* targetUser = User::getUserByIdentifier(identifier);
            if (targetUser) {
                Utils::clearScreen();
                Transaction::viewUserTransactions(targetUser->studentId);
                delete targetUser;
            } else {
                cout << "\nError: User not found!" << endl;
            }
            Utils::pause();
        }
        else if (choice == 5) {
            Utils::clearScreen();
            Feedback::viewAllFeedbacks();
            Utils::pause();
        }
        else if (choice == 6) {
            Utils::clearScreen();
            cout << "\n" << string(60, '=') << endl;
            cout << "SALES ANALYTICS" << endl;
            cout << string(60, '=') << endl;
            cout << "1. Daily Sales (Specific Date)" << endl;
            cout << "2. Weekly Sales" << endl;
            cout << "3. Monthly Sales" << endl;
            cout << "4. Sales by Day of Week" << endl;
            cout << "Choice: ";

            int analyticsChoice;
            cin >> analyticsChoice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            Utils::clearScreen();
            if (analyticsChoice == 1) {
                cout << "\nEnter date (YYYY-MM-DD) or press Enter for today: ";
                string date;
                getline(cin, date);
                if (date.empty()) {
                    date = Utils::getCurrentDate();
                }
                SalesAnalytics::generateDailySales(date);
            }
            else if (analyticsChoice == 2) {
                int week = Utils::getWeekNumber();
                SalesAnalytics::generateWeeklySales(week);
            }
            else if (analyticsChoice == 3) {
                cout << "\nEnter month-year (YYYY-MM) or press Enter for current month: ";
                string monthYear;
                getline(cin, monthYear);
                if (monthYear.empty()) {
                    monthYear = Utils::getMonthYear();
                }
                SalesAnalytics::generateMonthlySales(monthYear);
            }
            else if (analyticsChoice == 4) {
                cout << "\nSelect day:" << endl;
                cout << "1. Monday" << endl;
                cout << "2. Tuesday" << endl;
                cout << "3. Wednesday" << endl;
                cout << "4. Thursday" << endl;
                cout << "5. Friday" << endl;
                cout << "6. Saturday" << endl;
                cout << "7. Sunday" << endl;
                cout << "Choice: ";
                
                int dayChoice;
                cin >> dayChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                string days[] = {"", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
                if (dayChoice >= 1 && dayChoice <= 7) {
                    Utils::clearScreen();
                    SalesAnalytics::generateSalesByDay(days[dayChoice]);
                }
            }
            Utils::pause();
        }
        else if (choice == 7) {
            Utils::clearScreen();
            SalesAnalytics::displayBarGraph();
            Utils::pause();
        }
        else if (choice == 8) {
            Utils::clearScreen();
            cout << "\nLogging out... Goodbye, " << admin->username << "!" << endl;
            Utils::pause();
            break;
        }
        else {
            cout << "\nError: Invalid choice!" << endl;
            Utils::pause();
        }
    }
}

int main() {
    srand(time(0));
    
    MenuItem::initializeMenu();

    while (true) {
        showMainMenu();
        
        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 1) {
            Utils::clearScreen();
            cout << "\n" << string(60, '=') << endl;
            cout << "LOGIN" << endl;
            cout << string(60, '=') << endl;
            cout << "Enter username: ";
            string username;
            getline(cin, username);

            cout << "Enter password: ";
            string password;
            getline(cin, password);

            User* user = User::login(username, password);
            if (user) {
                Utils::clearScreen();
                cout << "\nLogin successful!" << endl;
                cout << "Welcome, " << user->username << "!" << endl;
                Utils::pause();
                
                if (user->role == "admin") {
                    adminMenu(user);
                } else {
                    studentMenu(user);
                }
                
                delete user;
            } else {
                Utils::pause();
            }
        }
        else if (choice == 2) {
            Utils::clearScreen();
            cout << "\n" << string(60, '=') << endl;
            cout << "REGISTRATION" << endl;
            cout << string(60, '=') << endl;
            
            cout << "Enter Student ID: ";
            string studentId;
            getline(cin, studentId);

            cout << "Enter username: ";
            string username;
            getline(cin, username);

            cout << "Enter password: ";
            string password;
            getline(cin, password);

            cout << "\nSelect role:" << endl;
            cout << "1. Student" << endl;
            cout << "2. Admin" << endl;
            cout << "Choice: ";
            int roleChoice;
            cin >> roleChoice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            string role = (roleChoice == 2) ? "admin" : "student";

            User::registerUser(studentId, username, password, role);
            Utils::pause();
        }
        else if (choice == 3) {
            Utils::clearScreen();
            cout << "\nThank you for using IUT CafeCache!" << endl;
            cout << "Goodbye!" << endl << endl;
            break;
        }
        else {
            cout << "\nError: Invalid choice!" << endl;
            Utils::pause();
        }
    }

    return 0;
}