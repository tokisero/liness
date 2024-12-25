#ifndef ADMINPANEL_H
#define ADMINPANEL_H

#include "BasePanel.h"
#include <QTableView>

namespace Ui {
class AdminPanell;
}

class AdminPanell : public BasePanel {
    Q_OBJECT

public:
    explicit AdminPanell(DataBase* db, QWidget* parent = nullptr);
    ~AdminPanell();

private slots:
    void on_AddButton_clicked();
    void on_DeleteButton_clicked();
    void on_UpdateButton_clicked() override;
    void on_SaveToButton_clicked();
    void on_tableView_clicked(const QModelIndex& index);
    void on_logoutButton_clicked();
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_ListOfUsers_clicked();

private:
    void showFilteredTable(const QString& filter, const QString& title);
    void setupTableBooked(QTableView* tableView);
    Ui::AdminPanell* ui;
    int currentRow;
    bool isDialogOpen;
    QTableView* dialogPassengerTableView;
    QSqlTableModel* dialogPassengerModel;
    void applyTableStyling(QTableView* tableView, QSqlTableModel* model, const QStringList& columnNames);

signals:
    void logoutRequested();
};

#endif // ADMINPANEL_H
