#pragma once

#include <QDialog>
#include "FlightManager.h"
#include "ui_DeleteFlightPage.h"

namespace Ui {
class DeleteFlightPage;
}

class DeleteFlightPage : public QDialog, public FlightManager {
    Q_OBJECT

public:
    explicit DeleteFlightPage(QWidget *parent = nullptr);
    ~DeleteFlightPage();

private slots:
    void onButtonClicked();

private:
    Ui::DeleteFlightPage *ui;
    QPushButton* DeleteButton;
};
