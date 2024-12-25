#pragma once

#include <QDialog>
#include <QWidget>
#include <QTableView>
#include <QSqlTableModel>
#include <QVBoxLayout>


namespace Ui {
class ListAllFlightsPage;
}

class ListAllFlightsPage : public QWidget {
    Q_OBJECT

public:
    explicit ListAllFlightsPage(QWidget* parent = nullptr);
    void setDatabase(const QString& dbName);

public slots:
    void refreshData();

private:
    QTableView* tableView;
    QSqlTableModel* model;
};
