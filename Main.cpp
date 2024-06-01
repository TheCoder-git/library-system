#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cstdlib> // For system("cls") or system("clear")
#include "json.hpp" // Include the JSON library
#include <ctime> // For time functions

#ifdef _WIN32
#define CLEAR_SCREEN "cls"
#else
#define CLEAR_SCREEN "clear"
#endif

using json = nlohmann::json;

class Book {
private:
    int number;
    std::string title;
    bool borrowed;
    std::string borrower;
    std::string borrowedDate;
    std::string returnDate;

public:
    Book(int num, const std::string& t) : number(num), title(t), borrowed(false) {}

    int getNumber() const { return number; }
    std::string getTitle() const { return title; }
    bool isBorrowed() const { return borrowed; }
    std::string getBorrower() const { return borrower; }
    std::string getBorrowedDate() const { return borrowedDate; }
    std::string getReturnDate() const { return returnDate; }

    void borrowBook(const std::string& name) {
        borrowed = true;
        borrower = name;

        std::time_t currentTime = std::time(nullptr);
        std::tm* localTime = std::localtime(&currentTime);
        char buffer[20];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", localTime);
        borrowedDate = buffer;

        // Set return date to 15 days from borrowed date
        std::tm returnTime = *localTime;
        returnTime.tm_mday += 15;
        std::mktime(&returnTime);
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &returnTime);
        returnDate = buffer;
    }

    void returnBook() {
        borrowed = false;
        borrower = "";
        borrowedDate = "";
        returnDate = "";
    }
};

class Library {
private:
    std::vector<Book> books;

    void saveToFile() {
        json j;
        for (const auto& book : books) {
            json bookData;
            bookData["number"] = book.getNumber();
            bookData["title"] = book.getTitle();
            bookData["borrowed"] = book.isBorrowed();
            bookData["borrower"] = book.getBorrower();
            bookData["borrowedDate"] = book.getBorrowedDate();
            bookData["returnDate"] = book.getReturnDate();
            j.push_back(bookData);
        }

        std::ofstream file("books.json");
        if (file.is_open()) {
            file << std::setw(4) << j << std::endl;
            file.close();
        } else {
            std::cerr << "Unable to open file";
        }
    }

    void loadFromFile() {
        std::ifstream file("Books.json");
        if (file.is_open()) {
            json j;
            file >> j;
            file.close();

            for (const auto& bookData : j) {
                int number = bookData["number"];
                std::string title = bookData["title"];
                Book book(number, title);
                if (bookData["borrowed"]) {
                    book.borrowBook(bookData["borrower"]);
                }
                books.push_back(book);
            }
        }
    }

public:
    Library() {
        loadFromFile();
    }

    void displayAvailableBooks() const {
        std::cout << "List of Available Books:" << std::endl;
        for (const auto& book : books) {
            if (!book.isBorrowed()) {
                std::cout << book.getNumber() << ": " << book.getTitle() << std::endl;
            }
        }
    }

    void displayBorrowedBooks() const {
        std::cout << "List of Borrowed Books:" << std::endl;
        for (const auto& book : books) {
            if (book.isBorrowed()) {
                std::cout << "Book: " << book.getTitle() << std::endl;
                std::cout << "Borrower: " << book.getBorrower() << std::endl;
                std::cout << "Borrowed Date: " << book.getBorrowedDate() << std::endl;
                std::cout << "Return Date: " << book.getReturnDate() << std::endl;
                std::cout << std::endl;
            }
        }
    }

    void borrowBook(int number, const std::string& name) {
        for (auto& book : books) {
            if (book.getNumber() == number && !book.isBorrowed()) {
                book.borrowBook(name);
                saveToFile();
                std::cout << "Book borrowed successfully." << std::endl;
                return;
            }
        }
        std::cout << "Book not available for borrowing." << std::endl;
    }

    void returnBook(int number) {
        for (auto& book : books) {
            if (book.getNumber() == number && book.isBorrowed()) {
                book.returnBook();
                saveToFile();
                std::cout << "Book returned successfully." << std::endl;
                return;
            }
        }
        std::cout << "Book not found or already returned." << std::endl;
    }
};

void displayMenu() {
    std::cout << "Library Management System Menu:" << std::endl;
    std::cout << "1. Display Available Books" << std::endl;
    std::cout << "2. Display Borrowed Books" << std::endl;
    std::cout << "3. Borrow a Book" << std::endl;
    std::cout << "4. Return a Book" << std::endl;
    std::cout << "5. Exit" << std::endl;
}

void holdScreen() {
    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

int main() {
    Library library;
    std::string borrowerName; // Declare borrowerName outside the switch statement

    int choice;
    do {
        system(CLEAR_SCREEN);
        displayMenu();
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                library.displayAvailableBooks();
                holdScreen(); // Hold the screen after displaying available books
                break;
            case 2:
                library.displayBorrowedBooks();
                holdScreen(); // Hold the screen after displaying borrowed books
                break;
            case 3:
                std::cout << "Enter your name: ";
                std::cin.ignore(); // Ignore previous newline character
                std::getline(std::cin, borrowerName); // Get borrower's name

                int bookNumber;
                std::cout << "Enter book number to borrow: ";
                std::cin >> bookNumber;

                if (std::cin.fail()) {
                    std::cout << "Invalid input. Please enter a valid book number." << std::endl;
                    std::cin.clear(); // Clear the error flag
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
                } else {
                    library.borrowBook(bookNumber, borrowerName);
                    holdScreen();
                }
                break;
            case 4:
                std::cout << "Enter book number to return: ";
                std::cin >> bookNumber;
                library.returnBook(bookNumber);
                holdScreen();
                break;
            case 5:
                std::cout << "Exiting program..." << std::endl;
                holdScreen(); // Hold the screen before exiting
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                holdScreen(); // Hold the screen after displaying error message
        }
    } while (choice != 5);

    return 0;
}
