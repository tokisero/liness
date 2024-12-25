#include "BookedFlight.h"

BookedFlight::BookedFlight()
    : flightNumber(""), seatClass(""), occupiedSeats(0) {}


BookedFlight::BookedFlight(const QString& flightNumber, const QString& seatClass, int occupiedSeats)
    : flightNumber(flightNumber), seatClass(seatClass), occupiedSeats(occupiedSeats) {}

BookedFlight::BookedFlight(const BookedFlight& other)
    : flightNumber(other.flightNumber),
    seatClass(other.seatClass),
    occupiedSeats(other.occupiedSeats),
    passengers(other.passengers) {}

BookedFlight& BookedFlight::operator=(const BookedFlight& other) {
    if (this != &other) {
        flightNumber = other.flightNumber;
        seatClass = other.seatClass;
        occupiedSeats = other.occupiedSeats;
        passengers = other.passengers;
    }
    return *this;
}

void BookedFlight::addPassenger(const Passenger& passenger) {
    passengers.append(passenger);
}

QVector<Passenger> BookedFlight::getPassengers() const {
    return passengers;
}

QString BookedFlight::getFlightNumber() const {
    return flightNumber;
}

QString BookedFlight::getSeatClass() const {
    return seatClass;
}

int BookedFlight::getOccupiedSeats() const {
    return occupiedSeats;
}

void BookedFlight::setOccupiedSeats(int seats) {
    occupiedSeats = seats;
}
