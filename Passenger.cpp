#include "Passenger.h"


Passenger::Passenger()
    : firstName(""), birthDate(QDate()), passportNumber(""), gender("") {}

Passenger::Passenger(const QString& firstName, const QDate& birthDate, const QString& passportNumber, const QString& gender)
    : firstName(firstName), birthDate(birthDate), passportNumber(passportNumber), gender(gender) {}

QString Passenger::getFirstName() const {
    return firstName;
}

void Passenger::setFirstName(const QString& firstName) {
    this->firstName = firstName;
}

QDate Passenger::getBirthDate() const {
    return birthDate;
}

void Passenger::setBirthDate(const QDate& birthDate) {
    this->birthDate = birthDate;
}

QString Passenger::getPassportNumber() const {
    return passportNumber;
}

void Passenger::setPassportNumber(const QString& passportNumber) {
    this->passportNumber = passportNumber;
}

QString Passenger::getGender() const {
    return gender;
}

void Passenger::setGender(const QString& gender) {
    this->gender = gender;
}
