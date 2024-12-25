#ifndef VALIDATEDSQLTABLEMODEL_H
#define VALIDATEDSQLTABLEMODEL_H

#include <QSqlTableModel>
#include "ManagerException.h"
#include <QSqlDatabase>

class ValidatedSqlTableModel : public QSqlTableModel {
    Q_OBJECT

public:
    explicit ValidatedSqlTableModel(QObject* parent = nullptr, QSqlDatabase db = QSqlDatabase());

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool submitAll();
Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role) override;
bool setData(const QModelIndex& index, const QVariant& value, int role) override;
private:
    bool validateData(const QModelIndex& index, const QVariant& value) const;
    QStringList columnHeaders;

};

#endif // VALIDATEDSQLTABLEMODEL_H
