#pragma once
#include <QString>

class Aircraft {
private:
    QString name;
    int economySeats;
    int comfortSeats;
    int businessSeats;

public:
    Aircraft(const QString& name = "", int economy = 0, int comfort = 0, int business = 0);

    const QString& getName() const;
    int getEconomySeats() const;
    int getComfortSeats() const;
    int getBusinessSeats() const;

    void setName(const QString& name);
    void setEconomySeats(int seats);
    void setComfortSeats(int seats);
    void setBusinessSeats(int seats);

    bool bookSeat(const QString& seatClass);
    bool unbookSeat(const QString& seatClass);
};
