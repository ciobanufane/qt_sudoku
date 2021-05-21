#include <QtWidgets>
#include "cell.h"

Cell::Cell()
{
}

QTableWidgetItem* Cell::clone() const
{
    return new Cell(*this);
}

void Cell::setData(int role, const QVariant& value)
{
    bool isNumeric;
    int result;
    switch (role) {
    case Qt::EditRole:
        result = value.toInt(&isNumeric);
        if (result >= 0 && result <= 9) {
            QTableWidgetItem::setData(role, result);
        } else {
            QTableWidgetItem::setData(role, 0);
        }
        break;
    case Qt::ForegroundRole:
    case Qt::BackgroundRole:
        QTableWidgetItem::setData(role, value);
        break;
    }
}

QVariant Cell::data(int role) const
{
    switch(role) {
    case Qt::DisplayRole:
        if (value() == 0) {
            return "";
        }
        return value();
    case Qt::TextAlignmentRole:
        return int(Qt::AlignCenter);
    default:
        return QTableWidgetItem::data(role);
    }
}

void Cell::setValue(int value)
{
    setData(Qt::EditRole, value);
}

int Cell::value() const
{
    return data(Qt::EditRole).toInt();
}
