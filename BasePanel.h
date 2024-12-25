#ifndef BASEPANEL_H
#define BASEPANEL_H

#include <QDialog>
#include <QSqlTableModel>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QMessageBox>
#include "DataBase.h"
#include "PanelInterface.h"
#include "ValidatedSqlTableModel.h"

class BasePanel : public PanelInterface {
    Q_OBJECT

protected:
    DataBase* database;
    QSqlTableModel* tableModel;
    QSortFilterProxyModel* proxyModel;

    void setupTableView(QTableView* tableView);
    void loadData(const QString& tableName);

public:
    explicit BasePanel(DataBase* db, QWidget* parent = nullptr);
    ~BasePanel();
private slots:
    virtual void on_UpdateButton_clicked() = 0;
};

#endif
