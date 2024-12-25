#pragma once
#include <QString>
#include <QDateTime>
#include "AirCraft.h"

class Flight {
private:
    int flightNumber;
    QString destination;
    QDateTime departureTime;
    Aircraft aircraft;

public:
    Flight(int flightNumber = 0, const QString& destination = "", const QDateTime& departureTime = QDateTime(),
           const Aircraft& aircraft = Aircraft());

    int getFlightNumber() const;
    const QString& getDestination() const;
    const QDateTime& getDepartureTime() const;
    const Aircraft& getAircraft() const;

    void setFlightNumber(int number);
    void setDestination(const QString& destination);
    void setDepartureTime(const QDateTime& time);
    void setAircraft(const Aircraft& aircraft);

    bool bookSeat(const QString& seatClass);
    bool unbookSeat(const QString& seatClass);

    bool operator==(const Flight& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Flight& flight);
};
