#include "AirCraft.h"

Aircraft::Aircraft(const QString& name, int economy, int comfort, int business)
    : name(name), economySeats(economy), comfortSeats(comfort), businessSeats(business) {}

const QString& Aircraft::getName() const {
    return name;
}

int Aircraft::getEconomySeats() const {
    return economySeats;
}

int Aircraft::getComfortSeats() const {
    return comfortSeats;
}

int Aircraft::getBusinessSeats() const {
    return businessSeats;
}

void Aircraft::setName(const QString& newName) {
    name = newName;
}

void Aircraft::setEconomySeats(int seats) {
    economySeats = seats;
}

void Aircraft::setComfortSeats(int seats) {
    comfortSeats = seats;
}

void Aircraft::setBusinessSeats(int seats) {
    businessSeats = seats;
}

bool Aircraft::bookSeat(const QString& seatClass) {
    if (seatClass == "Economy" && economySeats > 0) {
        --economySeats;
        return true;
    } else if (seatClass == "Comfort" && comfortSeats > 0) {
        --comfortSeats;
        return true;
    } else if (seatClass == "Business" && businessSeats > 0) {
        --businessSeats;
        return true;
    }
    return false;
}

bool Aircraft::unbookSeat(const QString& seatClass) {
    if (seatClass == "Economy") {
        ++economySeats;
        return true;
    } else if (seatClass == "Comfort") {
        ++comfortSeats;
        return true;
    } else if (seatClass == "Business") {
        ++businessSeats;
        return true;
    }
    return false;
}
