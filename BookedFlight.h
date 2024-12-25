#ifndef BOOKEDFLIGHT_H
#define BOOKEDFLIGHT_H

#include <QString>
#include <QVector>
#include "Passenger.h"

class BookedFlight {
public:
    BookedFlight();
    BookedFlight(const QString& flightNumber, const QString& seatClass, int occupiedSeats = 0);
    BookedFlight(const BookedFlight& other);
    BookedFlight& operator=(const BookedFlight& other);
    void addPassenger(const Passenger& passenger);
    QVector<Passenger> getPassengers() const;
    QString getFlightNumber() const;
    QString getSeatClass() const;
    int getOccupiedSeats() const;
    void setOccupiedSeats(int seats);
private:
    QString flightNumber;
    QString seatClass;
    int occupiedSeats;
    QVector<Passenger> passengers;
};

#endif // BOOKEDFLIGHT_H
