#include "ListAllFlightsPage.h"
#include <QSqlError>


ListAllFlightsPage::ListAllFlightsPage(QWidget* parent)
    : QWidget(parent), tableView(new QTableView(this)), model(new QSqlTableModel(this)) {

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(tableView);

    tableView->setModel(model);
    tableView->resizeColumnsToContents();
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    setLayout(layout);
}

void ListAllFlightsPage::setDatabase(const QString& dbName) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);

    if (!db.open()) {
        qDebug() << "Ошибка подключения к базе данных:" << db.lastError().text();
        return;
    }

    // Установить модель для отображения таблицы "Flights"
    model->setTable("Flights");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    model->setHeaderData(0, Qt::Horizontal, "Flight number");
    model->setHeaderData(1, Qt::Horizontal, "Destination");
    model->setHeaderData(2, Qt::Horizontal, "Day");
    model->setHeaderData(3, Qt::Horizontal, "Month");
    model->setHeaderData(4, Qt::Horizontal, "Time");
    model->setHeaderData(5, Qt::Horizontal, "Seats");

    tableView->resizeColumnsToContents();
}

void ListAllFlightsPage::refreshData() {
    model->select(); // Обновление данных
    tableView->resizeColumnsToContents();
}
