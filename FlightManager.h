#pragma once
#include "Flight.h"
#include "DataBase.h"
#include "Queue.h"
#include <vector>
#include <string>

class FlightManager {
private:
    std::vector<Flight> flights;
    std::vector<Flight> results;
    Queue<Flight> bookedFlightsQueue;
    DataBase db;

public:
    explicit FlightManager(const QString& filename = "flight.db");
    ~FlightManager();
    void addFlight(int flightNumber, const std::string& destination, int day, int month, int time, int seats);
    void editFlight(int flightNumber);
    void deleteFlight(int flightNumber);
    void findFlight();
    std::string findFlight(std::string yn, int day, int month);
    std::string findFlight(std::string yn, std::string destination);
    std::string findFlight(std::string yn, int flightNumber);
    std::string filter(std::string yn);
    void listAllFlights() const;
    bool bookFlight(int flightNumber);
    bool unbookFlight(int flightNumber);
    void showBookedFlights() const;
    FlightManager& operator+=(const Flight& flight);
};
