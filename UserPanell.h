#pragma once

#include "BasePanel.h"
#include "Queue.h"
#include <QTableView>
#include <QModelIndex>
#include "BookedFlightsWindow.h"
#include "BookedFlight.h"

namespace Ui {
class UserPanell;
}

class UserPanell : public BasePanel {
    Q_OBJECT

public:
    explicit UserPanell(DataBase* db, int userId, QWidget* parent = nullptr);
    ~UserPanell();
    QString setUsername(const QString username);

private slots:
    void on_BookFlight_clicked();
    void on_ListABooked_clicked();
    void on_tableView_clicked(const QModelIndex& index);
    void on_UpdateButton_clicked() override;
    void on_logoutButton_clicked();

private:
    Ui::UserPanell* ui;
    int currentRow;
    int userId;
    Queue<BookedFlight>* bookedFlightsQueue;
    BookedFlightsWindow* bookedFlightsWindow;
    void loadQueue();

signals:
    void logoutRequested();
};
