#include "BasePanel.h"
#include "FilterTable.h"
#include "ValidatedSqlTableModel.h"

BasePanel::BasePanel(DataBase* db, QWidget* parent)
    : PanelInterface(parent), database(db), tableModel(nullptr), proxyModel(nullptr) {}

BasePanel::~BasePanel() {
    delete tableModel;
    delete proxyModel;
}

void BasePanel::setupTableView(QTableView* tableView) {
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(tableModel);

    FilterTable* header = new FilterTable(Qt::Horizontal, tableView);
    header->setFilterModel(proxyModel);
    header->setTableView(tableView);

    tableView->setModel(proxyModel);
    tableView->setHorizontalHeader(header);

    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    tableView->setColumnHidden(tableModel->fieldIndex("economySeats"), true);
    tableView->setColumnHidden(tableModel->fieldIndex("comfortSeats"), true);
    tableView->setColumnHidden(tableModel->fieldIndex("businessSeats"), true);

    if (!header->filtersCreated) {
        header->updateFilters();
    }
}

void BasePanel::loadData(const QString& tableName) {
    try {
        tableModel = new ValidatedSqlTableModel(this, database->getDatabase());
        tableModel->setTable(tableName);
        if (!tableModel->select()) {
            throw DatabaseException("Failed to load data from the database.");
        }
    } catch (const DatabaseException& e) {
        e.showErrorMessage();
    }
}

