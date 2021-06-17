#ifndef BOARDDIALOG_H
#define BOARDDIALOG_H

#include <QTableWidget>
#include <QStyledItemDelegate>
#include "sudokuconflicts.h"

class Cell;
class Conflicts;

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
    void onConflictChanged(std::vector<QPoint> conf);
    void onReset();
private:
    int boxLength;
    int highlightRow = -1;
    int highlightCol = -1;
    std::vector<QPoint> conflicts;
};

class BoardDialog : public QTableWidget
{
    Q_OBJECT
public:
    BoardDialog(QWidget* parent = nullptr);
    void solve();
    int boxLength() const;
    int boardLength() const;
    int displayTime() const;
    bool displaySolver() const;
    void setDisplayTime(int ms);
    void setDisplaySolver(bool b);
    int value(int row, int col) const;
    void setValue(int row, int col, int value);
signals:
    void hoverRowChanged(int row);
    void hoverColChanged(int col);
    void resetBoard();
private slots:
    void onFinishPaint();
protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
private:
    bool editing;
    bool solver;
    int milliseconds;
    Conflicts* conflict;

    enum{ BoxLength = 3, MinClues = 17, EasyClues = 31 };
    const QColor colours[5] = {QColor(255,0,0,125), QColor(0,255,0,125), QColor(0,0,255,125),
                      QColor(255,255,255,125),
                      QColor(0,128,64,125)};

    Cell* cell(int row, int col) const;
    void setItemForeground(int row, int col, const QColor& color);
    void setItemBackground(int row, int col, const QColor& color);
    void setZeroBoard();
    void setHighlight(int row, int col);
    void undoHighlight(int row, int col);
    void wait(int interval);

    bool validInsert(int row, int col) const;
    bool validRow(int row) const;
    bool validCol(int col) const;
    bool validBox(int row, int col) const;

    bool solveBoard(int position);
    void removeClues();
    void setFixedClues();
};


#endif
