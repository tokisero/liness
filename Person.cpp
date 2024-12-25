#include "Person.h"
#include <iostream>

Person::Person(const std::string& username)
    : User(username){}

void Person::menu() {
    int choice;
    do {
        std::cout << "User Menu:\n";
        std::cout << "1. List all flights\n";
        std::cout << "2. Book a flight\n";
        std::cout << "3. Unbook a flight\n";
        std::cout << "4. View booked flights\n";
        std::cout << "5. Exit\n";
        std::cout << "Your choice: ";
        std::cin >> choice;
        switch (choice) {
        case 1:
            listAllFlights();
            break;
        case 2: {
            int flightNumber;
            std::cout << "Enter flight number to book: ";
            std::cin >> flightNumber;
            if (bookFlight(flightNumber)) {
                std::cout << "Flight booked successfully!\n";
                rewind(stdin);
            }
            else {
                std::cout << "No seats available.\n";
            }
            break;
        }
        case 3: {
            int flightNumber;
            std::cout << "Enter flight number to book: ";
            std::cin >> flightNumber;
            unbookFlight(flightNumber);
            std::cout << "Flight unbooked successfully!\n";
            rewind(stdin);
            break;
        }
        case 4: {
            showBookedFlights();
            rewind(stdin);
            break;
        }
        case 5: {
            std::cout << "Exiting user panel.\n";
            break;
        }
        default:
            std::cout << "Invalid choice, try again.\n";
        }
    } while (choice != 5);
}