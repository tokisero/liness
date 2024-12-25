#include "FlightManager.h"
#include <iostream>

FlightManager::FlightManager(const QString& filename) : db(filename) {
    flights = db.readFromFile();
}

FlightManager::~FlightManager() {
    flights.clear();
}


template <typename Vector>
void printVector(const Vector& vector, const std::string& header = "Elements:") {
    std::cout << header << std::endl;
    for (const auto& element : vector) {
        std::cout << element << std::endl;
    }
    std::cout << "-----------------" << std::endl;
}

void updateFlightDetails(Flight& flight) {
    std::string newDestination;
    int newDay; 
    int newMonth; 
    int newTime;

    std::cout << "Enter new destination (0 to skip): ";
    std::cin >> newDestination;
    if (newDestination != "0") {
        flight.setDestination(newDestination);
    }

    std::cout << "Enter new day of departure (0 to skip): ";
    std::cin >> newDay;
    if (newDay != 0 && newDay <= 31) {
        flight.setDay(newDay);
    }

    std::cout << "Enter new month of departure (0 to skip): ";
    std::cin >> newMonth;
    if (newMonth != 0 && newMonth <= 12) {
        flight.setMonth(newMonth);
    }

    std::cout << "Enter new time of departure (0 to skip): ";
    std::cin >> newTime;
    if (newTime != 0 && newTime <= 24) {
        flight.setTime(newTime);
    }

    std::cout << "Flight details successfully updated!" << std::endl;
}

void FlightManager::addFlight(int flightNumber, const std::string& destination, int day, int month, int time, int seats) {
    flights = db.readFromFile();
    listAllFlights();
    Flight newFlight(flightNumber, destination, day, month, time, seats);
    db.addFlight(newFlight);
    flights.push_back(newFlight);
    listAllFlights();
}

void FlightManager::editFlight(int flightNumber) {
    for (auto& flight : flights) {
        if (flight.getFlightNumber() == flightNumber) {
            updateFlightDetails(flight);
            db.editFlight(flight);
            return;
        }
    }
    std::cout << "Flight with that number not found!" << std::endl;
}

void FlightManager::deleteFlight(int flightNumber) {
    flights = db.readFromFile();
    listAllFlights();
    auto it = std::ranges::find_if(flights, [flightNumber](const Flight& f) { return f.getFlightNumber() == flightNumber; });
    if (it != flights.end()) {
        db.deleteFlight(flightNumber);
        flights.erase(it);
        std::cout << "Flight deleted!" << std::endl;
    }
    else {
        std::cout << "Flight with that number not found!" << std::endl;
    }
    listAllFlights();
}

std::string FlightManager::findFlight(std::string yn, int flightNumber) {
    if (yn == "n") {
        results = flights;
    }
    int i = 0;
    bool found = false;
    while (i < results.size()) {
        const auto& flight = results[i];
        if (flight.getFlightNumber() != flightNumber) {
            results.erase(results.begin() + i);
        }
        else {
            found = true;
            i++;
        }
    }
    yn = filter(yn);
    if (!found) {
        std::cout << "Flight with that number not found!" << std::endl;
    }
    return yn;
}

std::string FlightManager::findFlight(std::string yn, std::string destination) {
    if (yn == "n") {
        results = flights;
    }
    int i = 0;
    bool found = false;

    while (i < results.size()) {
        const auto& flight = results[i];
        if (flight.getDestination() != destination) {
            results.erase(results.begin() + i);
        }
        else {
            found = true;
            i++;
        }
    }
    yn = filter(yn);
    if (!found) {
        std::cout << "Flight with that destination not found!" << std::endl;
    }
    return yn;
}

std::string FlightManager::findFlight(std::string yn, int day, int month) {
    if (yn == "n") {
        results = flights;
    }
    bool found = false;
    int i = 0;
    while (i < results.size()) {
        const auto& flight = results[i];
        if (flight.getDay() != day || flight.getMonth() != month) {
            results.erase(results.begin() + i);
        }
        else {
            found = true;
            i++; 
        }
    }
    yn = filter(yn);
    if (!found) {
        std::cout << "Flight with that date not found!" << std::endl;
    }
    return yn;
}
std::string FlightManager::filter(std::string yn) {
    std::cout << "Add a filter? (y/n): ";
    std::cin >> yn;
    while (true)
    {
        if (yn == "y") {
            rewind(stdin);
            break;
        }
        else if (yn == "n") {
            printVector(results, "Search results: ");
            rewind(stdin);
            break;
        }
        else {
            std::cout << "Try again\n";
            rewind(stdin);
        }
    }
    return yn;
}

void FlightManager::findFlight() {
    int choice = 0;
    do {
        std::cin >> choice;
        std::string yn = "n";
        switch (choice) {
        case 1: {
            int flightNumber;
            std::cout << "Enter flight number: ";
            std::cin >> flightNumber;
            findFlight(yn, flightNumber);
            break;
        }
        case 2: {
            std::string destination;
            std::cout << "Enter destination: ";
            std::cin >> destination;
            findFlight(yn, destination);
            break;
        }
        case 3:
            int day;
            int month;
            std::cout << "Enter day: ";
            std::cin >> day;
            std::cout << "Enter month: ";
            std::cin >> month;
            yn = findFlight(yn, day, month);
            break;
        case 4: {

        }
        case 5:
            results.clear();
            std::cout << "Exiting search." << std::endl;
            break;
        default:
            std::cout << "Invalid choice, try again." << std::endl;
            break;
        }
    } while (choice != 5);
}

void FlightManager::listAllFlights() const {
    if (flights.empty()) {
        std::cout << "No available flights." << std::endl;
    }
    else {
        printVector(flights, "List of all flights:");
    }
}

bool FlightManager::bookFlight(int flightNumber) {
    for (auto& flight : flights) {
        if (flight.getFlightNumber() == flightNumber) {
            if (flight.getSeats() > 0) {
               // flight.bookSeat();
                db.editFlight(flight);
                bookedFlightsQueue.enqueue(flight);
                std::cout << "Flight booked successfully!" << std::endl;
                return true;
            }
            else {
                std::cout << "No available seats on this flight." << std::endl;
                return false;
            }
        }
    }
    std::cout << "Flight with that number not found!" << std::endl;
    return false;
}

bool FlightManager::unbookFlight(int flightNumber) {
    Queue<Flight> tempQueue; // Временная очередь для перебора
    bool found = false;
    while (!bookedFlightsQueue.isEmpty()) {
        Flight flight = bookedFlightsQueue.dequeue();
        if (flight.getFlightNumber() == flightNumber) {
            found = true;
            for (auto& f : flights) {
                if (f.getFlightNumber() == flightNumber) {
                    //f.unbookSeat();
                    break;
                }
            }
        }
        else {
            tempQueue.enqueue(flight);
        }
    }
    bookedFlightsQueue = tempQueue;
    if (found) {   
        std::cout << "Booking canceled for flight " << flightNumber << ".\n";
    }
    else {
        std::cout << "Flight not found in booked flights.\n";
    }
    return found;
}

void FlightManager::showBookedFlights() const {
    if (bookedFlightsQueue.isEmpty()) {
        std::cout << "No booked flights.\n";
        return;
    }
}


FlightManager& FlightManager::operator += (const Flight& flight) {
    flights.emplace_back(flight.getFlightNumber(), flight.getDestination(), flight.getDay(), flight.getMonth(), flight.getTime(), flight.getSeats());
    return *this;
}
