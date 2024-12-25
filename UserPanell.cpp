#include "UserPanell.h"
#include "ui_UserPanell.h"
#include "FlightBooking.h"
#include <QInputDialog>
#include <QComboBox>

UserPanell::UserPanell(DataBase* db, int userId, QWidget* parent)
    : BasePanel(db, parent), ui(new Ui::UserPanell), currentRow(-1), userId(userId) {
    ui->setupUi(this);
    setWindowTitle("Flightmanager");
    loadData("Flights");
    setupTableView(ui->tableView);
    bookedFlightsQueue = new Queue<BookedFlight>();
    bookedFlightsWindow = new BookedFlightsWindow(bookedFlightsQueue, database, userId);
    loadQueue();
}

UserPanell::~UserPanell() {
    delete ui;
}

void UserPanell::loadQueue() {
    auto bookedFlights = database->getBookedFlights(userId);
    for (const auto& bookedFlight : bookedFlights) {
        bookedFlightsQueue->enqueue(bookedFlight);
    }
}

void UserPanell::on_BookFlight_clicked() {
    QModelIndex selectedIndex = ui->tableView->currentIndex();
    if (!selectedIndex.isValid()) {
        QMessageBox::warning(this, "Selection Error", "Please select a flight to book.");
        return;
    }
    int flightNumber = selectedIndex.siblingAtColumn(0).data().toInt();

    auto optionalFlight = database->getFlight(flightNumber);
    if (!optionalFlight.has_value()) {
        QMessageBox::critical(this, "Error", "Flight not found in the database.");
        return;
    }
    Flight flight = optionalFlight.value();
    FlightBooking bookingWindow(flight, database, this);
    if (bookingWindow.exec() == QDialog::Accepted) {
        QVector<PassengerData> passengers = bookingWindow.getPassengerData();
        QString selectedClass = bookingWindow.getSelectedClass();

        try {
            for (const auto& passenger : passengers) {
                database->addBookedFlight(
                    flight.getFlightNumber(),
                    userId,
                    selectedClass,
                    passenger.fullName,
                    passenger.birthDate.toString("yyyy-MM-dd"),
                    passenger.passportNumber,
                    passenger.gender
                    );
            }
            QMessageBox::information(this, "Success", QString("Successfully booked flight #%1.").arg(flight.getFlightNumber()));
        } catch (const DatabaseException& e) {
            e.showErrorMessage();
        } catch (const GeneralException& e) {
            e.showErrorMessage();
        }
    }
}

void UserPanell::on_ListABooked_clicked() {
    loadQueue();
    if (bookedFlightsQueue->isEmpty()) {
        QMessageBox::information(this, "No Booked Flights", "You have not booked any flights yet.");
        return;
    }
    bookedFlightsWindow->loadBookedFlights();
    bookedFlightsWindow->show();
}

void UserPanell::on_tableView_clicked(const QModelIndex& index) {
    currentRow = index.row();
}

void UserPanell::on_UpdateButton_clicked() {
    tableModel->select();
}

void UserPanell::on_logoutButton_clicked() {
    logoutRequested();
}
