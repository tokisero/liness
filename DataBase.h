#pragma once

#include "Flight.h"
#include <vector>
#include <QString>
#include <QSqlDatabase>
#include <optional>
#include "ManagerException.h"
#include "BookedFlight.h"

class DataBase {
private:
    QString filename;
    QSqlDatabase db;
    void close();
    void initialize();

public:
    explicit DataBase(const QString& file);
    void open();
    ~DataBase();

    std::vector<Flight> readFromFile() const;
    void writeToFile(const std::vector<Flight>& flights) const;
    void addFlight(const Flight& flight) const;
    void editFlight(const Flight& flight) const;
    void deleteFlight(int flightNumber) const;
    void addBookedFlight(int flightNumber, int userId, const QString& seatClass,
    const QString& passengerName, const QString& birthDate,
    const QString& passport, const QString& gender);
    QSqlDatabase& getDatabase();
    void unbookSeat(int flightNumber, int userId, const QString& seatClass);
    std::vector<BookedFlight> getBookedFlights(int userId) const;
    int getUserIdByUsername(const QString& username) const;
    bool authenticateUser(const QString& username, const QString& password) const;
    int getUserRole(const QString& username) const;
    void registerUser(const QString& username, const QString& password, int role) const;
    std::vector<BookedFlight> getUserBookedFlights(int userId) const;
    std::vector<Passenger> getPassengerDetails(int flightNumber, const QString& seatClass, int userId) const;
    std::optional<Flight> getFlight(int flightNumber) const;
    void updateAvailableSeats(int flightNumber, const QString& seatClass, int seatsBooked);
    int getAvailableSeats(int flightNumber, const QString& seatClass) const;
    void cancelBooking(int flightNumber, int userId, const QString& seatClass, int passengersCount);
};
