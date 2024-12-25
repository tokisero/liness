#include "Flight.h"

Flight::Flight(int flightNumber, const QString& destination, const QDateTime& departureTime, const Aircraft& aircraft)
    : flightNumber(flightNumber), destination(destination), departureTime(departureTime), aircraft(aircraft) {}

int Flight::getFlightNumber() const {
    return flightNumber;
}

const QString& Flight::getDestination() const {
    return destination;
}

const QDateTime& Flight::getDepartureTime() const {
    return departureTime;
}

const Aircraft& Flight::getAircraft() const {
    return aircraft;
}

void Flight::setFlightNumber(int number) {
    flightNumber = number;
}

void Flight::setDestination(const QString& dest) {
    destination = dest;
}

void Flight::setDepartureTime(const QDateTime& time) {
    departureTime = time;
}

void Flight::setAircraft(const Aircraft& newAircraft) {
    aircraft = newAircraft;
}

bool Flight::bookSeat(const QString& seatClass) {
    return aircraft.bookSeat(seatClass);
}

bool Flight::unbookSeat(const QString& seatClass) {
    return aircraft.unbookSeat(seatClass);
}

bool Flight::operator==(const Flight& other) const {
    return flightNumber == other.flightNumber;
}

std::ostream& operator<<(std::ostream& os, const Flight& flight) {
    os << "Flight " << flight.flightNumber << " to " << flight.destination.toStdString()
    << " departs at " << flight.departureTime.toString(Qt::ISODate).toStdString();
    return os;
}
