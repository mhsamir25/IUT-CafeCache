<div align="center">

<h1>☕ IUT CaféCache</h1>
<p><b>A Smart Cafeteria E-Token & Wallet Management System</b></p>

<p>
  <i>Software Project Lab I (SPL-I)</i><br>
  Islamic University of Technology (IUT)
</p>

<hr width="60%">
</div>

---

## Project Overview

<b>IUT CaféCache</b> is a <b>C++ console-based cafeteria management system</b> designed to
digitalize the manual token and payment workflow of the IUT cafeteria.  
The system aims to improve **transaction speed, data accuracy, transparency**, and
**user convenience** for both non-residential students and faculty members.

The software simulates a backend system with the following components:
- **User management:** student and teacher accounts, authentication, and profile handling  
- **Wallet system:** recharge, balance tracking, and secure e-payment  
- **E-token generation:** for queue-free meal collection  
- **Menu management:** daily menu creation and updates  
- **Transaction logging:** purchase and recharge history  
- **Administrative controls:** monitoring sales, approving requests, and maintaining data integrity  

The project demonstrates real-world file-based data persistence without relying on external frameworks.

---

## Motivation

### User-Focused Motivation
- Reduce waiting time caused by manual token collection  
- Enable real-time access to menus and prices  
- Simplify wallet recharge and e-token generation  
- Provide transparency of spending and purchase history  

### Developer-Focused Motivation
- Gain experience with modular C++ programming  
- Learn backend system design concepts  
- Implement file handling for persistent data storage  
- Prepare for building scalable systems in SPL-II  

---

## Requirement Analysis

### Methods of Requirement Collection
- Survey via Google Forms (50+ responses)  
- Interviews with students, teachers, and cafeteria staff  

### Key Requirements
- Users can generate and use e-tokens  
- Wallet system must support balance tracking and recharges  
- Menu system must allow admin updates and real-time visibility  
- Transactions must be logged for auditing and transparency  
- System must provide refund/cancellation handling  

---

## System Architecture

The project is designed with a **modular approach**, separating concerns into distinct components:

1. **User Module**
   - Handles login, authentication, profile management, and role differentiation (student, teacher, admin)  

2. **Wallet Module**
   - Implements balance management, recharge processing, and secure transactions  

3. **Token Module**
   - Generates unique e-tokens for meal collection, validates token usage, and tracks token history  

4. **Menu Module**
   - Stores daily menu items, updates prices, and maintains availability  

5. **Transaction Module**
   - Logs every purchase and recharge with timestamps for auditing purposes  

6. **Admin Module**
   - Approves recharge requests, updates menu items, monitors sales reports (daily, weekly, monthly), and manages system data  

---

## Key Features

### User Features
- View daily menu with prices and availability  
- Check wallet balance and transaction history  
- Request wallet recharge and generate e-tokens  
- View spending summary and transaction logs  

### Admin Features
- Add, remove, or update menu items  
- Approve or reject wallet recharge requests  
- View detailed transaction logs for auditing  
- Monitor sales trends and generate reports  
- Maintain system data integrity  

---

## Data Management

- All user, wallet, token, and transaction data is stored in **text-based files** for persistence  
- Data structures include **structs, arrays, and file streams** for efficient handling  
- The system ensures data consistency even during concurrent operations  

---

## How to Run

1. Compile the project using a C++ compiler (e.g., `g++` or `Visual Studio`)  
```bash
g++ main.cpp -o IUTCafeCache


