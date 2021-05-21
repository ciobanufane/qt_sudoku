#ifndef BOARDDIALOG_H
#define BOARDDIALOG_H

#include <QTableWidget>
#include <QStyledItemDelegate>

class Cell;

class DrawGridDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    DrawGridDelegate(int boxLength, QObject* parent = nullptr);
    void paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
signals:
    void finishPaint() const;
private slots:
    void onHoverRowChanged(int row);
    void onHoverColChanged(int col);
private:
    int boxLength;
    int highlightRow = -1;
    int highlightCol = -1;
};

class BoardDialog : public QTableWidget
{
    Q_OBJECT
public:
    BoardDialog(QWidget* parent = nullptr);
    void solve();
    void setDisplaySolver(bool b);
    void setDisplayTime(int milliseconds);
signals:
    void hoverRowChanged(int row);
    void hoverColChanged(int col);
private slots:
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void onFinishPaint();
private:
    bool displaySolver = false;
    int milliseconds = 25;
    enum{ BoxLength = 3, MinClues = 17, EasyClues = 31 };
    const QColor colours[5] = {QColor(255,0,0,125), QColor(0,255,0,125), QColor(0,0,255,125),
                      QColor(255,255,255,125),
                      QColor(0,128,64,125)};

    Cell* cell(int row, int col) const;
    void setItemForeground(int row, int col, const QColor& color);
    void setItemBackground(int row, int col, const QColor& color);
    void setHighlight(int row, int col);
    void undoHighlight(int row, int col);
    void wait(int interval);

    int value(int row, int col) const;
    void setValue(int row, int col, int value);

    bool validInsert(int row, int col);
    bool validRow(int row);
    bool validCol(int col);
    bool validBox(int row, int col);

    bool solveBoard(int position);
    void removeClues();
    void setFixedClues();
};


#endif
