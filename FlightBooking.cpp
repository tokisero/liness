#include "FlightBooking.h"
#include "ui_FlightBooking.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QDebug>
#include "ManagerException.h"

FlightBooking::FlightBooking(Flight& flight, DataBase* db, QWidget* parent)
    : QDialog(parent),
    ui(new Ui::FlightBooking),
    selectedFlight(flight),
    database(db) {
    ui->setupUi(this);
    setWindowTitle("Book a Flight");

    QString flightInfo = QString("Flight #%1\nDestination: %2\nDate: %3\nTime: %4")
                             .arg(flight.getFlightNumber())
                             .arg(flight.getDestination())
                             .arg(flight.getDepartureTime().toString("dd.MM.yyyy"))
                             .arg(flight.getDepartureTime().toString("hh:mm"));
    ui->flightInfoLabel->setText(flightInfo);

    QStringList seatClasses = {"Economy", "Comfort", "Business"};
    for (const QString& seatClass : seatClasses) {
        ui->seatClassComboBox->addItem(seatClass);
    }

    updateSeatAvailability();

    for (int i = 1; i <= 10; ++i) {
        ui->setNumbers->addItem(QString::number(i));
    }

    QVBoxLayout* layout = new QVBoxLayout(ui->scrollAreaPassengerFields->widget());
    layout->setAlignment(Qt::AlignTop);

    connect(ui->bookButton, &QPushButton::clicked, this, &FlightBooking::onBookButtonClicked);
    connect(ui->setNumbers, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &FlightBooking::onPassengerCountChanged);
    connect(ui->seatClassComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &FlightBooking::onSeatClassChanged);

    onPassengerCountChanged(0);
}

FlightBooking::~FlightBooking() {
    delete ui;
}

QString FlightBooking::getSelectedClass() const {
    return selectedClass;
}

QVector<PassengerData> FlightBooking::getPassengerData() const {
    return passengers;
}

void FlightBooking::onSeatClassChanged(int index) {
    selectedClass = ui->seatClassComboBox->currentText();
}

void FlightBooking::addPassengerFields(int passengerIndex) {
    PassengerWidgets widgets;

    QLabel* titleLabel = new QLabel(QString("Passenger %1").arg(passengerIndex + 1));
    titleLabel->setFont(QFont("Arial", 12, QFont::Bold));

    widgets.fullNameField = new QLineEdit();
    QLabel* fullNameLabel = new QLabel("Full Name:");
    widgets.fullNameField->setFixedHeight(40); // Увеличение высоты поля

    widgets.birthDateField = new QDateEdit();
    widgets.birthDateField->setCalendarPopup(true);
    QLabel* birthDateLabel = new QLabel("Birth Date:");
    widgets.birthDateField->setFixedHeight(40); // Увеличение высоты поля

    widgets.passportNumberField = new QLineEdit();
    QLabel* passportNumberLabel = new QLabel("Passport Number:");
    widgets.passportNumberField->setFixedHeight(40); // Увеличение высоты поля

    widgets.maleButton = new QRadioButton("Male");
    widgets.femaleButton = new QRadioButton("Female");
    QLabel* genderLabel = new QLabel("Choose Gender:");

    QHBoxLayout* genderLayout = new QHBoxLayout();
    genderLayout->addWidget(widgets.maleButton);
    genderLayout->addWidget(widgets.femaleButton);

    QVBoxLayout* passengerLayout = new QVBoxLayout();
    passengerLayout->addWidget(titleLabel);
    passengerLayout->addWidget(fullNameLabel);
    passengerLayout->addWidget(widgets.fullNameField);
    passengerLayout->addWidget(birthDateLabel);
    passengerLayout->addWidget(widgets.birthDateField);
    passengerLayout->addWidget(passportNumberLabel);
    passengerLayout->addWidget(widgets.passportNumberField);
    passengerLayout->addWidget(genderLabel);
    passengerLayout->addLayout(genderLayout);

    QWidget* passengerWidget = new QWidget();
    passengerWidget->setLayout(passengerLayout);
    passengerWidget->setFixedHeight(300); // Увеличение высоты всего виджета пассажира

    ui->scrollAreaPassengerFields->widget()->layout()->addWidget(passengerWidget);

    passengerWidgets.push_back(widgets);

    connect(widgets.fullNameField, &QLineEdit::editingFinished, [this, &widgets]() {
        try {
            validateFullName();
        } catch (const InputException& e) {
            QMessageBox::critical(this, "Validation Error", e.what());
            widgets.fullNameField->setFocus();
        }
    });

    connect(widgets.birthDateField, &QDateEdit::editingFinished, [this, &widgets]() {
        try {
            validateBirthDate();
        } catch (const InputException& e) {
            QMessageBox::critical(this, "Validation Error", e.what());
            widgets.birthDateField->setFocus();
        }
    });

    connect(widgets.passportNumberField, &QLineEdit::editingFinished, [this, &widgets]() {
        try {
            validatePassportNumber();
        } catch (const InputException& e) {
            QMessageBox::critical(this, "Validation Error", e.what());
            widgets.passportNumberField->setFocus();
        }
    });

    connect(widgets.maleButton, &QRadioButton::toggled, [this, &widgets]() {
        try {
            validateGender();
        } catch (const InputException& e) {
            QMessageBox::critical(this, "Validation Error", e.what());
        }
    });

    connect(widgets.femaleButton, &QRadioButton::toggled, [this, &widgets]() {
        try {
            validateGender();
        } catch (const InputException& e) {
            QMessageBox::critical(this, "Validation Error", e.what());
        }
    });
}


void FlightBooking::onPassengerCountChanged(int count) {
    count = ui->setNumbers->currentIndex() + 1;
    QLayout* layout = ui->scrollAreaPassengerFields->widget()->layout();
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    passengerWidgets.clear();

    for (int i = 0; i < count; ++i) {
        addPassengerFields(i);
    }
}

void FlightBooking::onBookButtonClicked() {
    try {
        passengers.clear();
        selectedClass = ui->seatClassComboBox->currentText();

        bool allFieldsValid = true;

        for (const auto& widgets : passengerWidgets) {
            try {
                validateFullName();
                validateBirthDate();
                validatePassportNumber();
                validateGender();
            } catch (const InputException& e) {
                QMessageBox::critical(this, "Validation Error", e.what());
                allFieldsValid = false;
                break;
            }
            if (allFieldsValid) {
                QString gender = widgets.maleButton->isChecked() ? "Male" : "Female";
                passengers.emplace_back(
                    widgets.fullNameField->text(),
                    widgets.birthDateField->date(),
                    widgets.passportNumberField->text(),
                    gender
                    );
            }
        }
        if (!allFieldsValid || passengers.isEmpty()) {
            throw InputException("Please fill in all fields correctly.");
        }
        database->updateAvailableSeats(selectedFlight.getFlightNumber(), selectedClass, passengers.size());
        accept();
    } catch (const ManagerException& e) {
        e.showErrorMessage();
    }
}





void FlightBooking::updateSeatAvailability() {
    int economySeats = selectedFlight.getAircraft().getEconomySeats();
    int comfortSeats = selectedFlight.getAircraft().getComfortSeats();
    int businessSeats = selectedFlight.getAircraft().getBusinessSeats();

    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->seatClassComboBox->model());

    if (model) {
        QStandardItem* item = model->item(0);
        if (item) {
            item->setFlags(economySeats > 0 ? Qt::ItemIsEnabled | Qt::ItemIsSelectable : Qt::NoItemFlags);
        }
        item = model->item(1);
        if (item) {
            item->setFlags(comfortSeats > 0 ? Qt::ItemIsEnabled | Qt::ItemIsSelectable : Qt::NoItemFlags);
        }
        item = model->item(2);
        if (item) {
            item->setFlags(businessSeats > 0 ? Qt::ItemIsEnabled | Qt::ItemIsSelectable : Qt::NoItemFlags);
        }
    }
}

void FlightBooking::validateFullName() {
    for (const auto& widgets : passengerWidgets) {
        try {
            bool valid = true;
            QRegularExpression nameRegex("^[A-Za-z\\s]+$");
            if (widgets.fullNameField->text().isEmpty() || !nameRegex.match(widgets.fullNameField->text()).hasMatch()) {
                valid = false;

                highlightField(widgets.fullNameField, valid);
                widgets.fullNameField->blockSignals(true);
                widgets.fullNameField->clear();
                widgets.fullNameField->blockSignals(false);

                throw InputException("Full Name is missing, contains invalid characters.");
            }
            highlightField(widgets.fullNameField, valid);
        } catch (const InputException& e) {
            e.showErrorMessage();
        }
    }
}

void FlightBooking::highlightField(QWidget* field, bool valid) {
    if (!valid) {
        field->setStyleSheet("border: 0.5px solid red;");
    } else {
        field->setStyleSheet("");
    }
}

void FlightBooking::validateBirthDate() {
    try {
        for (const auto& widgets : passengerWidgets) {
            if (!widgets.birthDateField->date().isValid()) {
                throw InputException("Invalid Birth Date.");
            }
        }
    } catch (const InputException& e) {
        e.showErrorMessage();
    }
}

void FlightBooking::validatePassportNumber() {
    for (const auto& widgets : passengerWidgets) {
        try {
            bool valid = true;
            QRegularExpression passportRegex("^[A-Z0-9]+$");

            if (widgets.passportNumberField->text().isEmpty() ||
                !passportRegex.match(widgets.passportNumberField->text()).hasMatch()) {
                valid = false;
                highlightField(widgets.passportNumberField, valid);

                widgets.passportNumberField->blockSignals(true);
                widgets.passportNumberField->clear();
                widgets.passportNumberField->blockSignals(false);

                throw InputException("Passport Number is missing, contains invalid characters, or is not in uppercase.");
            }
            highlightField(widgets.passportNumberField, valid);
        } catch (const InputException& e) {
            e.showErrorMessage();
        }
    }
}


void FlightBooking::validateGender() {
    try {
        for (const auto& widgets : passengerWidgets) {
            if (!widgets.maleButton->isChecked() && !widgets.femaleButton->isChecked()) {
                throw InputException("Gender not selected.");
            }
            break;
        }
    } catch (const InputException& e) {
        e.showErrorMessage();
    }
}


void FlightBooking::setupStyles() {
    // Общий стиль окна
    setStyleSheet("background-color: #f5f5f5; font-family: 'Segoe UI'; font-size: 12px;");

    // Заголовок информации о рейсе
    ui->flightInfoLabel->setStyleSheet(
        "color: #333; font-size: 18px; font-weight: bold; padding: 10px;"
        );

    // Выпадающий список класса
    ui->seatClassComboBox->setStyleSheet(
        "QComboBox {"
        "  border: 1px solid #ccc; border-radius: 5px; padding: 5px;"
        "  background: #ffffff;"
        "}"
        "QComboBox::drop-down {"
        "  border: 0px; padding-right: 10px;"
        "}"
        );

    // Кнопки
    ui->bookButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #4CAF50; color: white; border: none;"
        "  border-radius: 5px; padding: 8px 16px; font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: #45a049;"
        "}"
        );

    ui->cancelButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #f44336; color: white; border: none;"
        "  border-radius: 5px; padding: 8px 16px; font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: #d32f2f;"
        "}"
        );

    // Поля ввода для пассажиров
    ui->scrollAreaPassengerFields->setStyleSheet(
        "QScrollArea {"
        "  border: 1px solid #ccc; border-radius: 5px; background: #ffffff;"
        "}"
        );
}

void FlightBooking::on_cancelButton_clicked()
{
    this->close();
}

