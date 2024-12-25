#ifndef FILTERTABLE_H
#define FILTERTABLE_H

#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QLineEdit>
#include <QVector>
#include <QTableView>
#include <QSqlTableModel>

class FilterTable : public QHeaderView
{
    Q_OBJECT

public:
    FilterTable(Qt::Orientation orientation, QWidget* parent = nullptr);
    void setFilterModel(QSortFilterProxyModel* proxy);
    void updateFilters();
    void clearFilters();
    void setTableView(QTableView* tableView);
    bool filtersCreated = false;

protected:
    void resizeEvent(QResizeEvent* event) override;
    void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const override;

private:
    QSortFilterProxyModel* filterModel;
    QVector<QLineEdit*> lineEdits;
    QTableView* tableView;

    QRect calculateSectionRect(int logicalIndex) const;
    void onFilterTextChanged(int columnIndex, const QString &text);
    QRegularExpression createCombinedFilterExpression(const QStringList &filters);
    void updateTableAndFilters(QSqlTableModel* model);
};

#endif // FILTERTABLE_H
