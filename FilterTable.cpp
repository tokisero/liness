#include "FilterTable.h"
#include <QPainter>
#include <QRegularExpression>

FilterTable::FilterTable(Qt::Orientation orientation, QWidget* parent)
    : QHeaderView(orientation, parent), tableView(nullptr)
{
    filterModel = new QSortFilterProxyModel(this);
    if (orientation == Qt::Horizontal) {
        lineEdits.clear();
    }
}

void FilterTable::setTableView(QTableView* tableView) {
    this->tableView = tableView;
}

void FilterTable::updateFilters()
{
    if (filtersCreated) {
        return;
    }
    clearFilters();
    int columnCount = filterModel->sourceModel()->columnCount();

    for (int i = 0; i < columnCount; ++i) {
        if (tableView && tableView->isColumnHidden(i)) {
            continue;
        }

        QLineEdit *lineEdit = new QLineEdit(this);
        //lineEdit->setPlaceholderText(filterModel->sourceModel()->headerData(i, Qt::Horizontal).toString());
        lineEdit->setProperty("columnIndex", i);
        connect(lineEdit, &QLineEdit::textChanged, [this, lineEdit](const QString &text) {
            int columnIndex = lineEdit->property("columnIndex").toInt();
            onFilterTextChanged(columnIndex, text);
        });

        lineEdits.append(lineEdit);
    }

    filtersCreated = true;
}

void FilterTable::clearFilters()
{
    for (auto lineEdit : lineEdits) {
        lineEdit->clear();
        lineEdit->hide();
    }
    lineEdits.clear();

    filtersCreated = false;
}

void FilterTable::resizeEvent(QResizeEvent* event)
{
    QHeaderView::resizeEvent(event);
    if (!filterModel || !filterModel->sourceModel()) {
        return;
    }

    int columnCount = filterModel->sourceModel()->columnCount();
    int visibleColumnIndex = 0;
    int totalWidth = 0;
    for (int i = 0; i < columnCount; i++) {
        if (this->isSectionHidden(i)) {
            continue;
        }

        totalWidth += sectionSize(i);
    }

    for (int i = 0; i < columnCount; i++) {
        if (this->isSectionHidden(i)) {
            continue;
        }

        QRect sectionRect = calculateSectionRect(visibleColumnIndex);
        sectionRect.setWidth(sectionSize(i) * (width() / totalWidth));
        if (sectionRect.isValid() && visibleColumnIndex < lineEdits.size()) {
            int margin = 2;
            lineEdits[visibleColumnIndex]->setGeometry(sectionRect.x() + margin, sectionRect.y() + margin,
                                                       sectionRect.width() - 2 * margin, sectionRect.height() - 2 * margin);
            lineEdits[visibleColumnIndex]->show();
        }
        visibleColumnIndex++;
    }
}

void FilterTable::setFilterModel(QSortFilterProxyModel* proxy)
{
    filterModel = proxy;

    if (filterModel && filterModel->sourceModel()) {
        int columnCount = filterModel->sourceModel()->columnCount();

        for (int i = 0; i < columnCount; ++i) {
            if (tableView && tableView->isColumnHidden(i)) {
                continue;
            }

            QLineEdit* lineEdit = new QLineEdit(this);
            //lineEdit->setPlaceholderText(filterModel->sourceModel()->headerData(i, Qt::Horizontal).toString());
            lineEdit->setProperty("columnIndex", i);
            connect(lineEdit, &QLineEdit::textChanged, [this, lineEdit](const QString &text) {
                onFilterTextChanged(lineEdit->property("columnIndex").toInt(), text);
            });

            lineEdits.append(lineEdit);
        }
        for (int i = 0; i < columnCount; ++i) {
            if (tableView && tableView->isColumnHidden(i)) {
                continue;
            }

            if (i < lineEdits.size()) {
                lineEdits[i]->setPlaceholderText(filterModel->sourceModel()->headerData(i, Qt::Horizontal).toString());
            }
        }
    }
}

QRect FilterTable::calculateSectionRect(int logicalIndex) const
{
    int pos = sectionViewportPosition(logicalIndex);
    int width = sectionSize(logicalIndex);
    QRect rect(pos, 0, width, height());
    return rect;
}

void FilterTable::paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const
{
    QHeaderView::paintSection(painter, rect, logicalIndex);
}

void FilterTable::onFilterTextChanged(int columnIndex, const QString &text)
{
    QRegularExpression regex(text, QRegularExpression::CaseInsensitiveOption);
    filterModel->setFilterKeyColumn(columnIndex);
    filterModel->setFilterRegularExpression(regex);
}

QRegularExpression FilterTable::createCombinedFilterExpression(const QStringList &filters)
{
    QStringList regexParts;
    for (const QString &filter : filters) {
        regexParts.append(QRegularExpression::escape(filter));
    }
    QString combinedPattern = regexParts.join(".*");

    return QRegularExpression(combinedPattern, QRegularExpression::CaseInsensitiveOption);
}

void FilterTable::updateTableAndFilters(QSqlTableModel* model)
{
    clearFilters();
    if (model) {
        filterModel->setSourceModel(model);
        model->select();
    }
    updateFilters();
}
