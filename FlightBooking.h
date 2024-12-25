#ifndef FLIGHTBOOKING_H
#define FLIGHTBOOKING_H

#include <QDialog>
#include <QDate>
#include <QString>
#include <QVector>
#include <QLineEdit>
#include <QDateEdit>
#include <QRadioButton>
#include "ManagerException.h"
#include "Flight.h"
#include "DataBase.h"

namespace Ui {
class FlightBooking;
}

struct PassengerData {
    QString fullName;
    QDate birthDate;
    QString passportNumber;
    QString gender;

    PassengerData(const QString& name, const QDate& date, const QString& passport, const QString& gender)
        : fullName(name), birthDate(date), passportNumber(passport), gender(gender) {}
};

struct PassengerWidgets {
    QLineEdit* fullNameField;
    QDateEdit* birthDateField;
    QLineEdit* passportNumberField;
    QRadioButton* maleButton;
    QRadioButton* femaleButton;
};

class FlightBooking : public QDialog {
    Q_OBJECT

public:
    explicit FlightBooking(Flight& flight, DataBase* db, QWidget* parent = nullptr);
    ~FlightBooking();

    QString getSelectedClass() const;
    QVector<PassengerData> getPassengerData() const;

private slots:
    void onBookButtonClicked();
    void onPassengerCountChanged(int count);
    void onSeatClassChanged(int index);
    void validateFullName();
    void validateBirthDate();
    void validatePassportNumber();
    void validateGender();


    void on_cancelButton_clicked();

private:
    void addPassengerFields(int passengerIndex);
    DataBase* database;
    void highlightField(QWidget* field, bool valid);
    void setupStyles();

    Ui::FlightBooking* ui;
    Flight& selectedFlight;
    QString selectedClass;
    QVector<PassengerData> passengers;
    QVector<PassengerWidgets> passengerWidgets;
    void updateSeatAvailability();
};

#endif // FLIGHTBOOKING_H
