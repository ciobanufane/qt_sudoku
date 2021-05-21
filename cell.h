#ifndef CELL_H
#define CELL_H

#include <QTableWidgetItem>

class Cell : public QTableWidgetItem
{
public:
    Cell();
    QTableWidgetItem* clone() const;

    void setData(int role, const QVariant& value);
    QVariant data(int role) const;

    void setValue(int value);
    int value() const;

};

#endif
