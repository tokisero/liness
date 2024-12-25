#include "DeleteFlightPage.h"
#include "ui_DeleteFlightPage.h"
#include <QMessageBox>
#include <string>

DeleteFlightPage::DeleteFlightPage(QWidget *parent)
    : QDialog(parent), ui(new Ui::DeleteFlightPage) {
    ui->setupUi(this);
    DeleteButton = new QPushButton("Delete flight", this);
    connect(DeleteButton, &QPushButton::clicked, this, &DeleteFlightPage::onButtonClicked);
}

DeleteFlightPage::~DeleteFlightPage() {
    delete ui;
}

void DeleteFlightPage::onButtonClicked() {
    QString flightNumber = ui->flightNumberField->text();
    int flightNum = flightNumber.toInt();

    if (flightNumber.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter the flight number.");
        return;
    }
    std::cout << flightNum;
    deleteFlight(flightNum);
    QMessageBox::information(this, "Success", "Flight deleted successfully!");
    this->close();
}
