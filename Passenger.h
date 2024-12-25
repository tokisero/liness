#ifndef PASSENGER_H
#define PASSENGER_H

#include <QString>
#include <QDate>

class Passenger {
public:
    Passenger();

    Passenger(const QString& firstName, const QDate& birthDate, const QString& passportNumber, const QString& gender);

    QString getFirstName() const;
    void setFirstName(const QString& firstName);

    QDate getBirthDate() const;
    void setBirthDate(const QDate& birthDate);

    QString getPassportNumber() const;
    void setPassportNumber(const QString& passportNumber);

    QString getGender() const;
    void setGender(const QString& gender);

private:
    QString firstName;
    QDate birthDate;
    QString passportNumber;
    QString gender;
};

#endif // PASSENGER_H
