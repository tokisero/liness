#include "AdminPanell.h"
#include "ui_AdminPanell.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QDate>
#include <QTime>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QSqlQuery>
#include "DataSaver.h"
#include "FilterTable.h"

AdminPanell::AdminPanell(DataBase* db, QWidget* parent)
    : BasePanel(db, parent), ui(new Ui::AdminPanell), currentRow(-1), isDialogOpen(false),
    dialogPassengerTableView(nullptr), dialogPassengerModel(nullptr) {
    setWindowTitle("Flightmanager");
    ui->setupUi(this);
    loadData("Flights");
    setupTableView(ui->tableView);

}


AdminPanell::~AdminPanell() {
    delete ui;
}

void AdminPanell::on_AddButton_clicked() {
    int newRow = tableModel->rowCount();
    tableModel->insertRow(newRow);
    tableModel->setData(tableModel->index(newRow, tableModel->fieldIndex("economySeats")), 0);
    tableModel->setData(tableModel->index(newRow, tableModel->fieldIndex("comfortSeats")), 0);
    tableModel->setData(tableModel->index(newRow, tableModel->fieldIndex("businessSeats")), 0);
}

void AdminPanell::setupTableBooked(QTableView* tableView) {
    if (!tableView) return;
    dialogPassengerModel = new QSqlTableModel(this, database->getDatabase());
    dialogPassengerModel->setTable("BookedFlights");
    dialogPassengerModel->select();

    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(dialogPassengerModel);

    tableView->setModel(proxyModel);
    FilterTable* filterHeader = new FilterTable(Qt::Horizontal, tableView);
    filterHeader->setTableView(tableView);
    filterHeader->setFilterModel(proxyModel);
    tableView->setHorizontalHeader(filterHeader);
    QStringList columnNames = {"Flight Number", "ID","Seat Type", "Passenger Name", "Seat Number", "Passport number", "Gender"};
    applyTableStyling(tableView, dialogPassengerModel, columnNames);
    for (int i = 0; i < columnNames.size(); ++i) {
        dialogPassengerModel->setHeaderData(i, Qt::Horizontal, columnNames[i]);
    }

    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->resizeColumnsToContents();
    filterHeader->updateFilters();

    connect(tableView, &QTableView::doubleClicked, this, &AdminPanell::on_tableView_doubleClicked);
}



void AdminPanell::on_DeleteButton_clicked() {
    try {
        if (currentRow < 0) {
            throw Warning("No row selected for deletion.");
        }

        tableModel->removeRow(currentRow);
        if (!tableModel->submitAll()) {
            throw DatabaseException("Failed to delete row from the database.");
        }
        currentRow = -1;
    } catch (const Warning& e) {
        e.showErrorMessage();
    } catch (const DatabaseException& e) {
        e.showErrorMessage();
    }
}

void AdminPanell::on_UpdateButton_clicked() {
    tableModel->select();
}

void AdminPanell::on_tableView_clicked(const QModelIndex& index) {
    currentRow = index.row();
    if (index.column() == 7) {
        QDialog dialog(this);
        dialog.setWindowTitle("Edit Seats");
        dialog.resize(200, 215);
        QVBoxLayout* layout = new QVBoxLayout(&dialog);
        QFont labelFont("Arial", 10);

        QLabel* economyLabel = new QLabel("Economy seats:", &dialog);
        economyLabel->setFont(labelFont);
        QLineEdit* economyInput = new QLineEdit(&dialog);
        economyInput->setText(tableModel->data(tableModel->index(index.row(), tableModel->fieldIndex("economySeats"))).toString());
        layout->addWidget(economyLabel);
        layout->addWidget(economyInput);

        QLabel* comfortLabel = new QLabel("Comfort seats:", &dialog);
        comfortLabel->setFont(labelFont);
        QLineEdit* comfortInput = new QLineEdit(&dialog);
        comfortInput->setText(tableModel->data(tableModel->index(index.row(), tableModel->fieldIndex("comfortSeats"))).toString());
        layout->addWidget(comfortLabel);
        layout->addWidget(comfortInput);

        QLabel* businessLabel = new QLabel("Business seats:", &dialog);
        businessLabel->setFont(labelFont);
        QLineEdit* businessInput = new QLineEdit(&dialog);
        businessInput->setText(tableModel->data(tableModel->index(index.row(), tableModel->fieldIndex("businessSeats"))).toString());
        layout->addWidget(businessLabel);
        layout->addWidget(businessInput);

        QPushButton* okButton = new QPushButton("OK", &dialog);
        layout->addWidget(okButton);

        QObject::connect(okButton, &QPushButton::clicked, [&]() {
            try {
                bool success = true;
                int economySeats = economyInput->text().toInt();
                int comfortSeats = comfortInput->text().toInt();
                int businessSeats = businessInput->text().toInt();

                if (economySeats < 0 || comfortSeats < 0 || businessSeats < 0) {
                    throw InputException("Seat values must be non-negative integers.");
                }
                success &= tableModel->setData(tableModel->index(index.row(), tableModel->fieldIndex("economySeats")), economySeats);
                success &= tableModel->setData(tableModel->index(index.row(), tableModel->fieldIndex("comfortSeats")), comfortSeats);
                success &= tableModel->setData(tableModel->index(index.row(), tableModel->fieldIndex("businessSeats")), businessSeats);

                if (!success) {
                    throw InputException("Failed to update seat values.");
                }

                dialog.accept();
            } catch (const InputException& e) {
                e.showErrorMessage();
            }
        });

        dialog.setLayout(layout);
        dialog.exec();
    }
}

void AdminPanell::on_SaveToButton_clicked() {
    try {
        QSortFilterProxyModel* proxyModel = qobject_cast<QSortFilterProxyModel*>(ui->tableView->model());
        if (!proxyModel) {
            throw GeneralException("Proxy model not found.");
        }

        DataSaver::saveProxyModelToFile(proxyModel, "filteredResults.json");
        QMessageBox::information(this, "Success", "Data has been successfully saved to filteredResults.json.");
    } catch (const GeneralException& e) {
        e.showErrorMessage();
    }
}

void AdminPanell::on_logoutButton_clicked() {
    logoutRequested();
}


void AdminPanell::on_tableView_doubleClicked(const QModelIndex& index) {
    if (index.column() == tableModel->fieldIndex("flightNumber")) {
        int flightNumber = tableModel->data(index).toInt();
        QString filter = QString("flightNumber = %1").arg(flightNumber);
        showFilteredTable(filter, "Passenger List");
    }
}

void AdminPanell::showFilteredTable(const QString& filter, const QString& title) {
    QDialog dialog(this);
    dialog.setWindowTitle(title);
    dialog.resize(700, 400);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    QTableView* tableView = new QTableView(&dialog);
    setupTableBooked(tableView);
    QSqlTableModel* model = new QSqlTableModel(&dialog, database->getDatabase());
    model->setTable("BookedFlights");
    model->setFilter(filter);
    model->select();

    QStringList columnNames = {"ID", "Passenger Name", "Flight Number", "Seat Type", "Seat Number"};
    for (int i = 0; i < columnNames.size(); ++i) {
        model->setHeaderData(i, Qt::Horizontal, columnNames[i]);
    }

    tableView->setModel(model);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
    tableView->resizeColumnsToContents();
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    layout->addWidget(tableView);

    QPushButton* saveButton = new QPushButton("Save Changes", &dialog);
    layout->addWidget(saveButton);
    QObject::connect(saveButton, &QPushButton::clicked, [&]() {
        if (!model->submitAll()) {
            QMessageBox::warning(&dialog, "Error", "Failed to save changes.");
        } else {
            QMessageBox::information(&dialog, "Success", "Changes saved successfully.");
        }
    });

    QPushButton* deleteButton = new QPushButton("Delete Selected Row", &dialog);
    layout->addWidget(deleteButton);
    QObject::connect(deleteButton, &QPushButton::clicked, [&]() {
        QModelIndex currentIndex = tableView->currentIndex();
        if (!currentIndex.isValid()) {
            QMessageBox::warning(&dialog, "Selection Error", "No row selected.");
            return;
        }

        int row = currentIndex.row();
        if (QMessageBox::question(&dialog, "Confirm Deletion",
                                  "Are you sure you want to delete the selected row?") == QMessageBox::Yes) {
            model->removeRow(row);
            if (!model->submitAll()) {
                QMessageBox::warning(&dialog, "Error", "Failed to delete the row.");
            } else {
                QMessageBox::information(&dialog, "Success", "Row deleted successfully.");
                model->select();
            }
        }
    });

    dialog.setLayout(layout);
    dialog.exec();
}




void AdminPanell::on_ListOfUsers_clicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("List of Users");
    dialog.resize(400, 700);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    QTableView* usersTableView = new QTableView(&dialog);
    QSqlTableModel* usersModel = new QSqlTableModel(&dialog, database->getDatabase());
    usersModel->setTable("Users");
    usersModel->select();
    QStringList columnNames = {"ID", "Username", "Password", "Role"};
    for (int i = 0; i < columnNames.size(); ++i) {
        usersModel->setHeaderData(i, Qt::Horizontal, columnNames[i]);
        usersTableView->setColumnWidth(i, 200);
    }

    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel(&dialog);
    proxyModel->setSourceModel(usersModel);

    FilterTable* filterHeader = new FilterTable(Qt::Horizontal, usersTableView);
    filterHeader->setTableView(usersTableView);
    filterHeader->setFilterModel(proxyModel);
    usersTableView->setHorizontalHeader(filterHeader);

    usersTableView->setModel(proxyModel);
    usersTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    usersTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    usersTableView->setEditTriggers(QAbstractItemView::DoubleClicked);
    usersTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    usersTableView->resizeColumnsToContents();
    filterHeader->updateFilters();

    layout->addWidget(usersTableView, 1);
    QPushButton* addButton = new QPushButton("Add User", &dialog);
    layout->addWidget(addButton);
    QObject::connect(addButton, &QPushButton::clicked, [&]() {
        QDialog addUserDialog(&dialog);
        addUserDialog.setWindowTitle("Add New User");
        addUserDialog.resize(400, 250);

        QVBoxLayout* addUserLayout = new QVBoxLayout(&addUserDialog);
        QLineEdit* usernameInput = new QLineEdit(&addUserDialog);
        usernameInput->setPlaceholderText("Enter Username");
        QLineEdit* passwordInput = new QLineEdit(&addUserDialog);
        passwordInput->setPlaceholderText("Enter Password");
        passwordInput->setEchoMode(QLineEdit::Password);
        QLineEdit* roleInput = new QLineEdit(&addUserDialog);
        roleInput->setPlaceholderText("Enter Role");

        QPushButton* saveButton = new QPushButton("Save", &addUserDialog);
        addUserLayout->addWidget(new QLabel("Username:"));
        addUserLayout->addWidget(usernameInput);
        addUserLayout->addWidget(new QLabel("Password:"));
        addUserLayout->addWidget(passwordInput);
        addUserLayout->addWidget(new QLabel("Role:"));
        addUserLayout->addWidget(roleInput);
        addUserLayout->addWidget(saveButton);

        QObject::connect(saveButton, &QPushButton::clicked, [&]() {
            QString username = usernameInput->text();
            QString password = passwordInput->text();
            QString role = roleInput->text();

            if (username.isEmpty() || password.isEmpty() || role.isEmpty()) {
                QMessageBox::warning(&addUserDialog, "Input Error", "All fields are required.");
                return;
            }

            QSqlQuery query(database->getDatabase());
            query.prepare("INSERT INTO Users (username, password, role) VALUES (:username, :password, :role)");
            query.bindValue(":username", username);
            query.bindValue(":password", password);
            query.bindValue(":role", role);

            if (!query.exec()) {
                QMessageBox::warning(&addUserDialog, "Error", "Failed to add user.");
            } else {
                QMessageBox::information(&addUserDialog, "Success", "User added successfully.");
                usersModel->select();
                addUserDialog.accept();
            }
        });

        addUserDialog.exec();
    });
    QPushButton* deleteButton = new QPushButton("Delete Selected User", &dialog);
    layout->addWidget(deleteButton);
    QObject::connect(deleteButton, &QPushButton::clicked, [&]() {
        QModelIndex currentIndex = usersTableView->currentIndex();
        if (!currentIndex.isValid()) {
            QMessageBox::warning(&dialog, "Selection Error", "No user selected.");
            return;
        }

        if (QMessageBox::question(&dialog, "Confirm Deletion",
                                  "Are you sure you want to delete the selected user?") ==
            QMessageBox::Yes) {
            usersModel->removeRow(proxyModel->mapToSource(currentIndex).row());
            if (!usersModel->submitAll()) {
                QMessageBox::warning(&dialog, "Error", "Failed to delete user.");
            } else {
                QMessageBox::information(&dialog, "Success", "User deleted successfully.");
                usersModel->select();
            }
        }
    });
    QObject::connect(usersTableView, &QTableView::doubleClicked, [this, usersModel, proxyModel](const QModelIndex& index) {
        QModelIndex sourceIndex = proxyModel->mapToSource(index);
        int userId = usersModel->data(usersModel->index(sourceIndex.row(), usersModel->fieldIndex("userId"))).toInt();
        QString username = usersModel->data(usersModel->index(sourceIndex.row(), usersModel->fieldIndex("Username"))).toString();
        QString filter = QString("userID = %1").arg(userId);
        showFilteredTable(filter, "Passenger List");
    });
    dialog.setLayout(layout);
    dialog.exec();
}


void AdminPanell::applyTableStyling(QTableView* tableView, QSqlTableModel* model, const QStringList& columnNames) {
    if (!tableView || !model) return;
    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);

    FilterTable* filterHeader = new FilterTable(Qt::Horizontal, tableView);
    filterHeader->setTableView(tableView);
    filterHeader->setFilterModel(proxyModel);

    tableView->setHorizontalHeader(filterHeader);
    tableView->setModel(proxyModel);
    for (int i = 0; i < columnNames.size(); ++i) {
        model->setHeaderData(i, Qt::Horizontal, columnNames[i]);
    }
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->resizeColumnsToContents();
    filterHeader->updateFilters();
}
