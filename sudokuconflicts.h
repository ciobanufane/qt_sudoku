#ifndef SUDOKUCONFLICTS_H
#define SUDOKUCONFLICTS_H

#include <QObject>
#include <QPoint>
#include <vector>

class BoardDialog;
using SudokuMask = unsigned short;

class Conflicts : public QObject
{
    Q_OBJECT
public:
    Conflicts(BoardDialog* board);
    std::vector<QPoint> conflictList() const;
signals:
    void conflictChanged(std::vector<QPoint> conf);
private slots:
    void onCellChanged(int row, int col);
    void onReset();
private:
    Q_DISABLE_COPY(Conflicts);
    BoardDialog* board;
    std::vector<unsigned short> rowConflicts;
    std::vector<unsigned short> colConflicts;
    std::vector<unsigned short> boxConflicts;

    void updateConflict(int row, int col);
};

#endif
