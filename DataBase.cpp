#include "DataBase.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <iostream>
#include "VectorIt.h"

DataBase::DataBase(const QString& file) : filename(file) {
    open();
    initialize();
}

DataBase::~DataBase() {
    close();
}

void DataBase::open() {
    if (QSqlDatabase::contains("my_unique_connection")) {
        db = QSqlDatabase::database("my_unique_connection");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE", "my_unique_connection");
        db.setDatabaseName(filename);
        if (!db.open()) {
            throw DatabaseException(db.lastError().text());
        }
    }
}

void DataBase::close() {
    if (db.isOpen()) {
        db.close();
    }
}

void DataBase::initialize() {
    QSqlQuery query(db);

    QString createFlightTable = R"(
    CREATE TABLE IF NOT EXISTS Flights (
        flightNumber INTEGER PRIMARY KEY,
        destination TEXT NOT NULL,
        departureDate TEXT NOT NULL,
        departureTime TEXT NOT NULL,
        economySeats INTEGER NOT NULL,
        comfortSeats INTEGER NOT NULL,
        businessSeats INTEGER NOT NULL
    );
)";
    query.exec(createFlightTable);

    QString createUserTable = R"(
        CREATE TABLE IF NOT EXISTS Users (
            userId INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL,
            password TEXT NOT NULL,
            role INTEGER NOT NULL DEFAULT 0
        );
    )";
    query.exec(createUserTable);

    QString createBookedFlightsTable = R"(
    CREATE TABLE IF NOT EXISTS BookedFlights (
        flightNumber INTEGER NOT NULL,
        userId INTEGER NOT NULL,
        seatClass TEXT NOT NULL,
        passengerName TEXT NOT NULL,
        passengerBirthDate TEXT NOT NULL,
        passengerPassport TEXT NOT NULL,
        passengerGender TEXT NOT NULL,
        PRIMARY KEY (flightNumber, userId, seatClass, passengerPassport),
        FOREIGN KEY (flightNumber) REFERENCES Flights(flightNumber),
        FOREIGN KEY (userId) REFERENCES Users(userId)
    );
)";

    query.exec(createBookedFlightsTable);
}

void DataBase::addFlight(const Flight& flight) const {
    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO Flights (flightNumber, destination, departureDate, departureTime, economySeats, comfortSeats, businessSeats)
        VALUES (:flightNumber, :destination, :departureDate, :departureTime, :economySeats, :comfortSeats, :businessSeats)
    )");
    query.bindValue(":flightNumber", flight.getFlightNumber());
    query.bindValue(":destination", flight.getDestination());
    query.bindValue(":departureDate", flight.getDepartureTime().toString("dd-MM-yyyy"));
    query.bindValue(":departureTime", flight.getDepartureTime().toString("HH:mm"));
    query.bindValue(":economySeats", flight.getAircraft().getEconomySeats());
    query.bindValue(":comfortSeats", flight.getAircraft().getComfortSeats());
    query.bindValue(":businessSeats", flight.getAircraft().getBusinessSeats());

    if (!query.exec()) {
        throw DatabaseException(query.lastError().text());
    }
}


std::optional<Flight> DataBase::getFlight(int flightNumber) const {
    QSqlQuery query(db);
    query.prepare(R"(
        SELECT flightNumber, destination, departureDate, departureTime, economySeats, comfortSeats, businessSeats
        FROM Flights WHERE flightNumber = :flightNumber
    )");
    query.bindValue(":flightNumber", flightNumber);

    if (query.exec() && query.next()) {
        int flightNum = query.value("flightNumber").toInt();
        QString destination = query.value("destination").toString();
        QDate departureDate = QDate::fromString(query.value("departureDate").toString(), "dd-MM-yyyy");
        QTime departureTime = QTime::fromString(query.value("departureTime").toString(), "HH:mm");
        QDateTime departureDateTime(departureDate, departureTime);
        int economySeats = query.value("economySeats").toInt();
        int comfortSeats = query.value("comfortSeats").toInt();
        int businessSeats = query.value("businessSeats").toInt();
        QString name = "Airobous";
        Aircraft aircraft(name, economySeats, comfortSeats, businessSeats);
        return Flight(flightNum, destination, departureDateTime, aircraft);
    }
    return std::nullopt;
}


// void DataBase::editFlight(const Flight& flight) const {
//     QSqlQuery query(db);
//     query.prepare(R"(
//         UPDATE Flights
//         SET destination = :destination,
//             departureDate = :departureDate,
//             departureTime = :departureTime,
//             economySeats = :economySeats,
//             comfortSeats = :comfortSeats,
//             businessSeats = :businessSeats
//         WHERE flightNumber = :flightNumber
//     )");
//     query.bindValue(":flightNumber", flight.getFlightNumber());
//     query.bindValue(":destination", flight.getDestination());
//     query.bindValue(":departureDate", flight.getDepartureTime().toString("dd-MM-yyyy"));
//     query.bindValue(":departureTime", flight.getDepartureTime().toString("HH:mm"));

//     query.bindValue(":economySeats", flight.getAircraft().getEconomySeats());
//     query.bindValue(":comfortSeats", flight.getAircraft().getComfortSeats());
//     query.bindValue(":businessSeats", flight.getAircraft().getBusinessSeats());

//     if (!query.exec()) {
//         throw DatabaseException(query.lastError().text());
//     }
// }



void DataBase::registerUser(const QString& username, const QString& password, int role) const {
    try {
        QSqlQuery checkQuery(db);
        checkQuery.prepare("SELECT COUNT(*) FROM Users WHERE username = :username");
        checkQuery.bindValue(":username", username);

        if (!checkQuery.exec()) {
            throw DatabaseException(checkQuery.lastError().text());
        }

        if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
            throw InputException("User with this username already exists.");
            return;
        }

        QSqlQuery query(db);
        query.prepare("INSERT INTO Users (username, password, role) VALUES (:username, :password, :role)");
        query.bindValue(":username", username);
        query.bindValue(":password", password);
        query.bindValue(":role", role);
        if (!query.exec()) {
            throw DatabaseException(query.lastError().text());
        }
        QMessageBox::information(nullptr, "Success", "Registration successful!");
    } catch (const ManagerException& e) {
        e.showErrorMessage();
    }
}

int DataBase::getUserRole(const QString& username) const {
    QSqlQuery query(db);
    query.prepare("SELECT role FROM Users WHERE username = :username");
    query.bindValue(":username", username);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}

bool DataBase::authenticateUser(const QString& username, const QString& password) const {
    try {
        if (!db.isOpen()) {
            throw DatabaseException("Database is not open!");
        }

        QSqlQuery query(db);
        query.prepare("SELECT password FROM Users WHERE username = :username");
        query.bindValue(":username", username);

        if (query.exec() && query.next()) {
            QString storedPassword = query.value(0).toString();
            if (storedPassword != password) {
                return false;
            }
            return true;
        }
    } catch (const ManagerException& e) {
        e.showErrorMessage();
    }
    return false;
}
std::vector<BookedFlight> DataBase::getBookedFlights(int userId) const {
    std::vector<BookedFlight> bookedFlights;
    QSqlQuery query(db);
    query.prepare(R"(
        SELECT flightNumber, seatClass, COUNT(*) AS occupiedSeats
        FROM BookedFlights
        WHERE userId = :userId
        GROUP BY flightNumber, seatClass
    )");
    query.bindValue(":userId", userId);

    if (query.exec()) {
        while (query.next()) {
            int flightNumber = query.value("flightNumber").toInt();
            QString seatClass = query.value("seatClass").toString();
            int occupiedSeats = query.value("occupiedSeats").toInt();

            BookedFlight bookedFlight(QString::number(flightNumber), seatClass, occupiedSeats);
            bookedFlights.push_back(bookedFlight);
        }
    } else {
        throw DatabaseException(query.lastError().text());
    }

    return bookedFlights;
}


std::vector<Passenger> DataBase::getPassengerDetails(int flightNumber, const QString& seatClass, int userId) const {
    std::vector<Passenger> passengers;
    QSqlQuery query(db);
    query.prepare(R"(
        SELECT passengerName, passengerBirthDate, passengerPassport, passengerGender
        FROM BookedFlights
        WHERE flightNumber = ? AND seatClass = ? AND userId = ?;
    )");
    query.addBindValue(flightNumber);
    query.addBindValue(seatClass);
    query.addBindValue(userId);

    if (!query.exec()) {
        throw GeneralException("Failed to retrieve passenger details.");
    }

    while (query.next()) {
        Passenger passenger;
        passenger.setFirstName(query.value(0).toString());
        passenger.setBirthDate(QDate::fromString(query.value(1).toString(), "yyyy-MM-dd"));
        passenger.setPassportNumber(query.value(2).toString());
        passenger.setGender(query.value(3).toString());

        passengers.push_back(passenger);
    }

    return passengers;
}




QSqlDatabase& DataBase::getDatabase() {
    return db;
}

int DataBase::getUserIdByUsername(const QString& username) const {
    QSqlQuery query(db);
    query.prepare("SELECT userId FROM Users WHERE username = :username");
    query.bindValue(":username", username);
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    } else {
        std::cerr << "Error: " << query.lastError().text().toStdString() << std::endl;
        return -1;
    }
}

void DataBase::addBookedFlight(int flightNumber, int userId, const QString& seatClass,
                               const QString& passengerName, const QString& birthDate,
                               const QString& passport, const QString& gender) {
    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO BookedFlights (flightNumber, userId, seatClass, passengerName, passengerBirthDate, passengerPassport, passengerGender)
        VALUES (:flightNumber, :userId, :seatClass, :passengerName, :passengerBirthDate, :passengerPassport, :passengerGender);
    )");
    query.bindValue(":flightNumber", flightNumber);
    query.bindValue(":userId", userId);
    query.bindValue(":seatClass", seatClass);
    query.bindValue(":passengerName", passengerName);
    query.bindValue(":passengerBirthDate", birthDate);
    query.bindValue(":passengerPassport", passport);
    query.bindValue(":passengerGender", gender);
    if (!query.exec()) {
        throw GeneralException("Failed to add booked flight to the database.");
    }
}

void DataBase::cancelBooking(int flightNumber, int userId, const QString& seatClass, int passengersCount) {
    QSqlQuery query(db);
    query.prepare(R"(
        DELETE FROM BookedFlights
        WHERE flightNumber = :flightNumber AND userId = :userId AND seatClass = :seatClass
    )");
    query.bindValue(":flightNumber", flightNumber);
    query.bindValue(":userId", userId);
    query.bindValue(":seatClass", seatClass);

    if (!query.exec()) {
        throw GeneralException("Failed to cancel booking in the database.");
    }
    updateAvailableSeats(flightNumber, seatClass, -passengersCount);
}

void DataBase::updateAvailableSeats(int flightNumber, const QString& seatClass, int seatsBooked) {
    QSqlQuery query(db);
    QString updateQuery;
    if (seatClass == "Economy") {
        updateQuery = "UPDATE Flights SET economySeats = economySeats - :seatsBooked WHERE flightNumber = :flightNumber";
    } else if (seatClass == "Comfort") {
        updateQuery = "UPDATE Flights SET comfortSeats = comfortSeats - :seatsBooked WHERE flightNumber = :flightNumber";
    } else if (seatClass == "Business") {
        updateQuery = "UPDATE Flights SET businessSeats = businessSeats - :seatsBooked WHERE flightNumber = :flightNumber";
    }

    query.prepare(updateQuery);
    query.bindValue(":seatsBooked", seatsBooked);
    query.bindValue(":flightNumber", flightNumber);

    if (!query.exec()) {
        throw GeneralException("Failed to update available seats in the database.");
    }
}

int DataBase::getAvailableSeats(int flightNumber, const QString& seatClass) const {
    QSqlQuery query(db);
    query.prepare(R"(
        SELECT COUNT(*) AS bookedSeats
        FROM BookedFlights
        WHERE flightNumber = :flightNumber AND seatClass = :seatClass
    )");
    query.bindValue(":flightNumber", flightNumber);
    query.bindValue(":seatClass", seatClass);

    if (query.exec() && query.next()) {
        int bookedSeats = query.value("bookedSeats").toInt();
        std::optional<Flight> flight = getFlight(flightNumber);
        if (flight) {
            if (seatClass == "Economy") {
                return flight->getAircraft().getEconomySeats() - bookedSeats;
            } else if (seatClass == "Comfort") {
                return flight->getAircraft().getComfortSeats() - bookedSeats;
            } else if (seatClass == "Business") {
                return flight->getAircraft().getBusinessSeats() - bookedSeats;
            }
        }
    }
    return 0;
}







