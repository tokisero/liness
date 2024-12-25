#ifndef BOOKEDFLIGHTSWINDOW_H
#define BOOKEDFLIGHTSWINDOW_H

#include <QDialog>
#include <QStandardItemModel>
#include <QModelIndex>
#include "Queue.h"
#include <QSqlDatabase>
#include "DataBase.h"
#include "BookedFlight.h"

namespace Ui {
class BookedFlightsWindow;
}

class BookedFlightsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit BookedFlightsWindow(Queue<BookedFlight>* queue, DataBase* db,int userId, QWidget* parent = nullptr);
    ~BookedFlightsWindow();
    void loadBookedFlights();


private slots:
    void on_listView_clicked(const QModelIndex &index);
    void on_cancelButton_clicked();
    void on_listView_doubleClicked(const QModelIndex &index);

private:
    Ui::BookedFlightsWindow* ui;
    QStandardItemModel* listModel;
    DataBase* database;
    int userId;
    QModelIndex selectedIndex;
    Queue<BookedFlight>* bookedFlightsQueue;
};

#endif // BOOKEDFLIGHTSWINDOW_H
