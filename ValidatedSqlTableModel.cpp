#include "ValidatedSqlTableModel.h"
#include <QRegularExpression>
#include <QDate>
#include <QTime>
#include <QDebug>

ValidatedSqlTableModel::ValidatedSqlTableModel(QObject* parent, QSqlDatabase db)
    : QSqlTableModel(parent, db) {
    columnHeaders << "Number" << "Destination" << "Date" << "Time";
}

QVariant ValidatedSqlTableModel::data(const QModelIndex& index, int role) const {
    if (index.column() == columnCount() - 1) {
        if (role == Qt::DisplayRole) {
            int economy = QSqlTableModel::data(this->index(index.row(), fieldIndex("economySeats"))).toInt();
            int comfort = QSqlTableModel::data(this->index(index.row(), fieldIndex("comfortSeats"))).toInt();
            int business = QSqlTableModel::data(this->index(index.row(), fieldIndex("businessSeats"))).toInt();
            return economy + comfort + business;
        }
        if (role == Qt::BackgroundRole) {
            return QSqlTableModel::data(this->index(index.row(), 0), role);
        }
        if (role == Qt::ForegroundRole) {
            return QSqlTableModel::data(this->index(index.row(), 0), role);
        }
        if (role == Qt::FontRole) {
            return QSqlTableModel::data(this->index(index.row(), 0), role);
        }
    }

    return QSqlTableModel::data(index, role);
}

int ValidatedSqlTableModel::columnCount(const QModelIndex& parent) const {
    return QSqlTableModel::columnCount(parent) + 1;
}

QVariant ValidatedSqlTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section >= 0 && section < columnHeaders.size()) {
            return columnHeaders[section]; // Возвращаем название для соответствующего столбца
        }
    }
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal && section == columnCount() - 1) {
        return "Seats";
    }
    return QSqlTableModel::headerData(section, orientation, role);
}


Qt::ItemFlags ValidatedSqlTableModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags flags = QSqlTableModel::flags(index);
    if (index.column() == columnCount() - 1) {
        flags |= Qt::ItemIsEnabled;
    }
    if (index.column() == fieldIndex("economySeats") ||
        index.column() == fieldIndex("comfortSeats") ||
        index.column() == fieldIndex("businessSeats")) {
        flags |= Qt::ItemIsEditable;
    }

    return flags;
}

bool ValidatedSqlTableModel::submitAll() {
    try {
        for (int row = 0; row < rowCount(); ++row) {
            for (int col = 0; col < columnCount() - 1; ++col) {
                QModelIndex index = this->index(row, col);
                QVariant value = this->data(index);
                validateData(index, value);
            }
        }
        return QSqlTableModel::submitAll();
    } catch (const InputException& e) {
        e.showErrorMessage();
        return false;
    }
}

bool ValidatedSqlTableModel::validateData(const QModelIndex& index, const QVariant& value) const {
    try {
        switch (index.column()) {
        case 0:
            if (value.toInt() <= 0) {
                throw InputException("FlightNumber must be a positive integer.");
            }
            break;
        case 1:
            if (!QRegularExpression("^[a-zA-Z\\s\\-]+$").match(value.toString()).hasMatch()) {
                throw InputException("Destination must contain only letters, spaces, and hyphens.");
            }
            break;
        case 2:
            if (!QDate::fromString(value.toString(), "dd-MM-yyyy").isValid()) {
                throw InputException("Departure date must be in the format dd-MM-yyyy.");
            }
            break;
        case 3:
            if (!QTime::fromString(value.toString(), "HH:mm").isValid()) {
                throw InputException("Departure time must be in the format HH:mm.");
            }
            break;
        case 4:
        case 5:
        case 6:
            if (value.toInt() < 0) {
                throw InputException("Seat values must be non-negative integers.");
            }
            break;
        default:
            break;
        }
        return true;
    } catch (const InputException& e) {
        e.showErrorMessage();
        return false;
    }
}
bool ValidatedSqlTableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!validateData(index, value)) {
        return false;
    }
    return QSqlTableModel::setData(index, value, role);
}

bool ValidatedSqlTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role) {
    if (role != Qt::DisplayRole) return false;
    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 4:
        case 5:
        case 6:
            return false;
        }
    }
    return QSqlTableModel::setHeaderData(section, orientation, value, role);
}
