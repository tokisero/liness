#include "BookedFlightsWindow.h"
#include "ui_BookedFlightsWindow.h"
#include "Flight.h"
#include <QMessageBox>
#include <QDebug>
#include "ManagerException.h"

BookedFlightsWindow::BookedFlightsWindow(Queue<BookedFlight>* queue, DataBase* db, int userId, QWidget* parent)
    : QDialog(parent),
    ui(new Ui::BookedFlightsWindow),
    listModel(new QStandardItemModel(this)),
    database(db),
    bookedFlightsQueue(queue),
    userId(userId),
    selectedIndex() {
    ui->setupUi(this);
    ui->listView->setModel(listModel);
    selectedIndex = QModelIndex();
}

BookedFlightsWindow::~BookedFlightsWindow() {
    delete ui;
}

void BookedFlightsWindow::loadBookedFlights() {
    try {
        listModel->clear();
        auto bookedFlights = database->getBookedFlights(userId);
        for (const auto& bookedFlight : bookedFlights) {
            std::optional<Flight> flightOpt = database->getFlight(bookedFlight.getFlightNumber().toInt());
            if (flightOpt) {
                const Flight& flight = flightOpt.value();
                QString flightInfo = QString("Flight #%1\nDestination: %2\nDeparture: %3\nClass: %4\nPassengers: %5\n")
                                         .arg(flight.getFlightNumber())
                                         .arg(flight.getDestination())
                                         .arg(flight.getDepartureTime().toString("HH:mm dd-MM-yyyy"))
                                         .arg(bookedFlight.getSeatClass())
                                         .arg(bookedFlight.getOccupiedSeats());

                listModel->appendRow(new QStandardItem(flightInfo));
            }
        }
    } catch (const Warning& e) {
        e.showErrorMessage();
    } catch (const GeneralException& e) {
        e.showErrorMessage();
    }
}

void BookedFlightsWindow::on_listView_clicked(const QModelIndex& index) {
    selectedIndex = index;
}

void BookedFlightsWindow::on_cancelButton_clicked() {
    if (!selectedIndex.isValid()) {
        QMessageBox::warning(this, "Selection Error", "No valid booking selected.");
        return;
    }

    QString selectedText = listModel->itemFromIndex(selectedIndex)->text();
    QRegularExpression regex(R"(Flight #(\d+)[\s\S]*Class: (\w+))");
    QRegularExpressionMatch match = regex.match(selectedText);

    if (!match.hasMatch()) {
        QMessageBox::warning(this, "Data Error", "Could not extract booking data.");
        return;
    }

    int flightNumber = match.captured(1).toInt();
    QString seatClass = match.captured(2);

    try {
        auto passengers = database->getPassengerDetails(flightNumber, seatClass, userId);
        int passengersCount = passengers.size();

        if (passengersCount > 0) {
            int response = QMessageBox::question(this, "Confirm Cancellation",
                                                 QString("Are you sure you want to cancel the booking for %1 passengers?").arg(passengersCount),
                                                 QMessageBox::Yes | QMessageBox::No);

            if (response == QMessageBox::Yes) {
                database->cancelBooking(flightNumber, userId, seatClass, passengersCount);
                loadBookedFlights();
                QMessageBox::information(this, "Success", "Booking canceled successfully.");
            }
        } else {
            QMessageBox::information(this, "No Passengers", "No passenger details available.");
        }
    } catch (const GeneralException& e) {
        QMessageBox::critical(this, "Error", e.what());
    }
}

void BookedFlightsWindow::on_listView_doubleClicked(const QModelIndex &index) {
    if (!index.isValid()) {
        QMessageBox::warning(this, "Selection Error", "No valid flight selected.");
        return;
    }

    QString selectedText = listModel->itemFromIndex(index)->text();
    QRegularExpression regex(R"(Flight #(\d+)[\s\S]*Class: (\w+))");
    QRegularExpressionMatch match = regex.match(selectedText);

    if (!match.hasMatch()) {
        QMessageBox::warning(this, "Data Error", "Could not extract flight data.");
        return;
    }

    int flightNumber = match.captured(1).toInt();
    QString seatClass = match.captured(2);

    try {
        auto passengers = database->getPassengerDetails(flightNumber, seatClass, userId);
        QString passengerDetails;
        int i = 1;
        for (const auto& passenger : passengers) {
            passengerDetails += QString("Passenger #%1\n\nName: %2\nBirth Date: %3\nPassport number: %4\nGender: %5\n\n")
            .arg(i)
                .arg(passenger.getFirstName())
                .arg(passenger.getBirthDate().toString("yyyy-MM-dd"))
                .arg(passenger.getPassportNumber())
                .arg(passenger.getGender());
            i++;
        }

        if (passengerDetails.isEmpty()) {
            QMessageBox::information(this, "No Passengers", "No passenger details available.");
        } else {
            QMessageBox::information(this, "Passenger Details", passengerDetails);
        }
    } catch (const GeneralException& e) {
        e.showErrorMessage();
    }
}
