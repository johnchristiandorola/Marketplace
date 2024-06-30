#include <iostream.h>
#include <fstream.h>
#include <conio.h>
#include <string.h>
#include <stdio.h>
#include <process.h>

// Structure to represent a user
struct User {
    char username[20];
    char password[20];
    int money;
};

//Structure to represent an item in the marketplacee
struct Item {
    int itemId; // Unique identifier for each item
    char name[50];
    int price;
    int stock;
    char sellerUsername[20]; // Seller's username
};


const char* USERS_FILE = "users.txt";
const char* MARKET_FILE = "market.txt";

// Admin credentials
const char* ADMIN_USERNAME = "admin";
const char* ADMIN_PASSWORD = "admin123";

// Function to register a new user
void registerUser(const char* username, const char* password, int initialMoney) {
    ofstream outFile(USERS_FILE, ios::app);

    if (!outFile) {
	cout << "Error opening users file for writing.\n";
	return;
    }

    // Append the new user information to the file
    outFile << username << ' ' << password << ' ' << initialMoney << endl;

    cout << "User registered successfully.\n";

    outFile.close();
}

// Function to log in a user
int userLogin(char* username, char* password, User& loggedInUser) {
    ifstream inFile(USERS_FILE);



    char storedUsername[20], storedPassword[20];
    int storedMoney;

    while (inFile >> storedUsername >> storedPassword >> storedMoney) {
	if (strcmp(username, storedUsername) == 0 && strcmp(password, storedPassword) == 0) {
	    cout << "Login successful. Welcome, " << username << "!\n";
	    strcpy(loggedInUser.username, storedUsername);
	    strcpy(loggedInUser.password, storedPassword);
	    loggedInUser.money = storedMoney;
	    inFile.close();
	    return 1;
	}
    }

    // cout << "Login failed. Invalid username or password.\n";
    inFile.close();
    return 0;
}



// Function to sign up a new user
void signUp() {
    clrscr();

    char username[20], password[20];
    int initialMoney;

    cout << "Enter a new username: ";
    cin >> username;

    cout << "Enter a new password: ";
    cin >> password;

    cout << "Enter your initial amount of money: ";
    cin >> initialMoney;

    registerUser(username, password, initialMoney);
}

// Function to log in a user
int loginUser(User& loggedInUser) {
    clrscr();

    char username[20], password[20];

    cout << "Enter your username: ";
    cin >> username;

    cout << "Enter your password: ";
    cin >> password;

    if (userLogin(username, password, loggedInUser)) {
	// Perform actions after successful login
	// For example, display a menu, etc.
	return 1;
    } else {
	cout << "Login failed. Invalid username or password.\n";
	return 0;
    }
}


// Function to update the money of a user in the users file
void updateMoneyInUsersFile(const char* username, int newMoney) {
    ifstream inFile(USERS_FILE);
    ofstream outFile("temp_users.txt");

    char storedUsername[20], storedPassword[20];
    int storedMoney;

    while (inFile >> storedUsername >> storedPassword >> storedMoney) {
	if (strcmp(username, storedUsername) == 0) {
	    // Update the money for the specific user
	    outFile << username << ' ' << storedPassword << ' ' << newMoney << endl;
	} else {
	    // Copy other users without modification
	    outFile << storedUsername << ' ' << storedPassword << ' ' << storedMoney << endl;
	}
    }

    inFile.close();
    outFile.close();

    // Replace the original users file with the updated file

    remove(USERS_FILE);
    rename("temp_users.txt", USERS_FILE);
}


void loadMarketFromFile(Item market[], int& itemCount) {
    ifstream inFile(MARKET_FILE);

    if (!inFile) {
	cout << "Market file not found. Starting with an empty marketplace.\n";
	return;
    }

    ifstream sellerInfoFile("seller_info.txt");

    if (!sellerInfoFile) {
	cout << "Seller info file not found. Starting with an empty seller info.\n";
	return;
    }

    itemCount = 0;

    while (inFile >> market[itemCount].itemId >> market[itemCount].name >> market[itemCount].price >> market[itemCount].stock) {
	sellerInfoFile >> market[itemCount].itemId >> market[itemCount].sellerUsername;
	itemCount++;
	if (itemCount >= 100) {
	    cout << "Maximum number of items reached. Some items may not be loaded.\n";
	    break;
	}
    }

    inFile.close();
    sellerInfoFile.close();
}

void addItem(Item market[], int& itemCount, const User& seller) {
    clrscr();

    if (itemCount >= 100) {
	cout << "Marketplace is full. Cannot add more items.\n";
	return;
    }

    cout << "Enter the name of the new item: ";
    cin >> market[itemCount].name;

    cout << "Enter the price of the item: ";
    cin >> market[itemCount].price;

    cout << "Enter the stock of the item: ";
    cin >> market[itemCount].stock;

    // Assign a unique ID to the item
    market[itemCount].itemId = itemCount + 1;

    // Store the seller's username
    strcpy(market[itemCount].sellerUsername, seller.username);

    itemCount++;
}



// Function to purchase an item from the marketplace
void purchaseItem(User& buyer, Item market[], int& itemCount) {
    clrscr();

    char itemName[50];
    int quantity;

    cout << "Enter the name of the item you want to purchase: ";
    cin >> itemName;

    for (int i = 0; i < itemCount; i++) {
	if (strcmp(itemName, market[i].name) == 0 && market[i].stock > 0) {
	    cout << "Enter the quantity you want to purchase: ";
	    cin >> quantity;

	    if (quantity <= 0 || quantity > market[i].stock) {
		cout << "Invalid quantity.\n";
		return;
	    }

	    int totalCost = quantity * market[i].price;

	    if (buyer.money < totalCost) {
		cout << "Insufficient funds.\n";
		return;
	    }

	    // Deduct money from the buyer
	    buyer.money -= totalCost;

	    // Add money to the seller
	    User seller;
	    if (userLogin(market[i].sellerUsername, "", seller)) {
		seller.money += totalCost;
		updateMoneyInUsersFile(market[i].sellerUsername, seller.money);
	    }

	    // Deduct the stock of the item
	    market[i].stock -= quantity;

	    cout << "Purchase successful. You spent " << totalCost << " money.\n";
	    return;
	}
    }

    cout << "Item not found in the marketplace or out of stock.\n";
}



void saveMarketToFile(Item market[], int itemCount) {
    ofstream outFile(MARKET_FILE);


    ofstream sellerInfoFile("seller_info.txt");

    if (!sellerInfoFile) {
	cout << "Error opening seller info file for writing.\n";
	return;
    }

    for (int i = 0; i < itemCount; i++) {
	outFile << market[i].itemId << ' ' << market[i].name << ' ' << market[i].price << ' ' << market[i].stock << endl;
	sellerInfoFile << market[i].itemId << ' ' << market[i].sellerUsername << endl;
    }

    outFile.close();
    sellerInfoFile.close();
}





// Function to display items in the marketplace
void displayMarket(Item market[], int itemCount, const User& loggedInUser) {
    clrscr();

    cout << "Marketplace:\n";
    cout << "---------------------------------\n";
    cout << "Item\t\tPrice\tStock\tSeller\n";

    for (int i = 0; i < itemCount; i++) {
	cout << market[i].name << "\t\t" << market[i].price << "\t" << market[i].stock << "\t" << market[i].sellerUsername << endl;
    }

    cout << "---------------------------------\n";
    cout << "Your Remaining Money: " << loggedInUser.money << endl;
}

// Function to display the user menu
void userMenu(User& loggedInUser, Item market[], int& itemCount) {
    int userChoice;

    do {
	clrscr();
	cout << "User Menu\n";
	cout << "1. View Marketplace\n";
	cout << "2. Add Item to Marketplace\n";
	cout << "3. Purchase Item\n";
	cout << "4. Save and Exit\n";
	cout << "Enter your choice: ";
	cin >> userChoice;

	switch (userChoice) {
	    case 1:
		displayMarket(market, itemCount, loggedInUser);
		break;

	    case 2:
		addItem(market, itemCount, loggedInUser);
		break;

	    case 3:
		purchaseItem(loggedInUser, market, itemCount);
		break;

	    case 4:
		saveMarketToFile(market, itemCount);
		updateMoneyInUsersFile(loggedInUser.username, loggedInUser.money);
		cout << "Saving and exiting.\n";
		break;

	    default:
		cout << "Invalid choice. Please enter a valid option.\n";
	}

	cout << "\nPress any key to continue...";
	getch();

    } while (userChoice != 4);
}


// Function to remove a user from the users file
void removeUser(const char* username) {

    ifstream inFile(USERS_FILE);
    ofstream outFile("temp_users.txt");

    char storedUsername[20], storedPassword[20];
    int storedMoney;

    while (inFile >> storedUsername >> storedPassword >> storedMoney) {
	if (strcmp(username, storedUsername) != 0) {
	    // Copy other users without modification
	    outFile << storedUsername << ' ' << storedPassword << ' ' << storedMoney << endl;
	}
    }

    inFile.close();
    outFile.close();

    // Replace the original users file with the updated file
    remove(USERS_FILE);
    rename("temp_users.txt", USERS_FILE);

    cout << "User '" << username << "' removed successfully.\n";
}

// Function to remove an item from the marketplace
void removeItem(Item market[], int& itemCount) {

    if (itemCount <= 0) {
	cout << "No items in the marketplace to remove.\n";
	return;
    }

    char itemName[50];

    cout << "Enter the name of the item you want to remove: ";
    cin >> itemName;

    for (int i = 0; i < itemCount; i++) {
	if (strcmp(itemName, market[i].name) == 0) {
	    // Shift items to fill the gap
	    for (int j = i; j < itemCount - 1; j++) {
		strcpy(market[j].name, market[j + 1].name);
		market[j].price = market[j + 1].price;
		market[j].stock = market[j + 1].stock;
	    }

	    // Decrement item count
	    itemCount--;

	    cout << "Item '" << itemName << "' removed successfully.\n";
	    return;
	}
    }

    cout << "Item not found in the marketplace.\n";
}



void adminMenu(Item market[], int& itemCount,User& loggedInUser) {
	clrscr();
    int adminChoice;

    do {
	clrscr();
	cout << "Admin Menu\n";
	cout << "1. Add Item to Marketplace\n";
	cout << "2. Remove User\n";
	cout << "3. Remove Item from Marketplace\n";
	cout << "4. View Users\n";
	cout << "5. View Items\n";
	cout << "6. Logout\n";
	cout << "Enter your choice: ";
	cin >> adminChoice;

	switch (adminChoice) {
	    case 1:
		addItem(market, itemCount, loggedInUser);
		break;

	    case 2:
		{
		    char usernameToRemove[20];
		    cout << "Enter the username to remove: ";
		    cin >> usernameToRemove;
		    removeUser(usernameToRemove);
		}
		break;

	    case 3:
		removeItem(market, itemCount);
		break;

	    case 4:
		clrscr();
		cout << "List of Users:\n";
		cout << "------------------------\n";
		// Load user data from the file and display it here
		ifstream usersFile(USERS_FILE);
		if (usersFile) {
		    char username[20];
		    while (usersFile >> username) {
			cout << username << endl;
		    }
		    usersFile.close();
		}
		cout << "------------------------\n";
		cout << "Press any key to continue...";
		getch();
		break;

	    case 5:
		clrscr();
		cout << "List of Items:\n";
		cout << "---------------------------------\n";
		for (int i = 0; i < itemCount; i++) {
		    cout << market[i].name << endl;
		}
		cout << "---------------------------------\n";
		cout << "Press any key to continue...";
		getch();
		break;

	    case 6:
		cout << "Logging out from admin account.\n";
		break;

	    default:
		cout << "Invalid choice. Please enter a valid option.\n";
	}

	cout << "\nPress any key to continue...";
	getch();

    } while (adminChoice != 6);
}


// Main function
int main() {
	clrscr();
    User loggedInUser;
    Item market[100];  // Maximum 100 items in the marketplace
    int itemCount = 0;

    // Load initial marketplace items from file
    loadMarketFromFile(market, itemCount);

    int choice;

    do {
	clrscr();
	cout << "1. Sign up\n";
	cout << "2. Login\n";
	cout << "3. Admin Login\n";
	cout << "4. Exit\n";
	cout << "Enter your choice: ";
	cin >> choice;

	switch (choice) {
	    case 1:
		signUp();
		break;

	    case 2:
		if (loginUser(loggedInUser)) {
		    userMenu(loggedInUser, market, itemCount);
		}
		break;

	    case 3:
		{
		    char adminUsername[20], adminPassword[20];
		    cout << "Enter admin username: ";
		    cin >> adminUsername;
		    cout << "Enter admin password: ";
		    cin >> adminPassword;

		    if (strcmp(adminUsername, ADMIN_USERNAME) == 0 && strcmp(adminPassword, ADMIN_PASSWORD) == 0) {
			adminMenu(market, itemCount, loggedInUser);
		    } else {
			cout << "Admin login failed. Incorrect username or password.\n";
		    }
		}
		break;

	    case 4:
		cout << "Exiting the program.\n";
		// Save the user's information when exiting
		updateMoneyInUsersFile(loggedInUser.username, loggedInUser.money);
		break;

	    default:
		cout << "Invalid choice. Please enter a valid option.\n";
	}

	cout << "\nPress Enter to continue...";

		getch();

    } while (choice != 4);

    return 0;
}
