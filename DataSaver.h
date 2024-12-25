#ifndef DATASAVER_H
#define DATASAVER_H

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include "ManagerException.h"

class DataSaver {
public:
    static void saveToFile(const QJsonArray& data, const QString& filename) {
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly)) {
            return;
        }

        QJsonDocument doc(data);
        file.write(doc.toJson());
        file.close();
    }

    static void saveProxyModelToFile(QSortFilterProxyModel* proxyModel, const QString& filename) {
        try {
            if (!proxyModel) {
                throw GeneralException("Proxy model not found.");
            }

            QJsonArray jsonArray;
            for (int row = 0; row < proxyModel->rowCount(); ++row) {
                QJsonObject jsonObject;
                for (int col = 0; col < proxyModel->columnCount(); ++col) {
                    QString header = proxyModel->headerData(col, Qt::Horizontal).toString();
                    QModelIndex index = proxyModel->index(row, col);
                    QVariant data = proxyModel->data(index);
                    jsonObject[header] = data.toString();
                }
                jsonArray.append(jsonObject);
            }

            if (jsonArray.isEmpty()) {
                throw GeneralException("Error: No data to save.");
            }

            saveToFile(jsonArray, filename);
        } catch (const GeneralException& e) {
            e.showErrorMessage();
        }
    }
};

#endif // DATASAVER_H
