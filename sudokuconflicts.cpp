#include "sudokuconflicts.h"
#include "boarddialog.h"
#include <map>

Conflicts::Conflicts(BoardDialog* board)
    : QObject(board), board(board)
{
    rowConflicts.reserve(board->boardLength());
    colConflicts.reserve(board->boardLength());
    boxConflicts.reserve(board->boardLength());

    rowConflicts.resize(board->boardLength());
    colConflicts.resize(board->boardLength());
    boxConflicts.resize(board->boardLength());
}

void Conflicts::updateConflict(int row, int col)
{
    int topLeftRow = row/board->boxLength()*board->boxLength();
    int topLeftCol = col/board->boxLength()*board->boxLength();
    int currentBox = topLeftRow + topLeftCol/board->boxLength();

    rowConflicts[row] = 0;
    colConflicts[col] = 0;
    boxConflicts[currentBox] = 0;

    std::map<int,SudokuMask> r;
    std::map<int,SudokuMask> c;
    std::map<int,SudokuMask> b;

    SudokuMask mask = 1;
    for (int i = 0; i < board->boardLength(); ++i) {
        if (board->value(row, i) != 0)
            r[board->value(row, i)] |= mask;

        if (board->value(i, col) != 0)
            c[board->value(i, col)] |= mask;

        int xBox = topLeftRow + i/board->boxLength();
        int yBox = topLeftCol + i%board->boxLength();
        if (board->value(xBox, yBox) != 0)
            b[board->value(xBox, yBox)] |= mask;

        mask <<= 1;
    }

    for (auto it = r.begin(); it != r.end(); ++it) {
        if ((it->second & (it->second-1)) != 0)
            rowConflicts[row] |= it->second;
    }

    for (auto it = c.begin(); it != c.end(); ++it) {
        if ((it->second & (it->second-1)) != 0)
            colConflicts[col] |= it->second;
    }

    for (auto it = b.begin(); it != b.end(); ++it) {
        if ((it->second & (it->second-1)) != 0)
            boxConflicts[currentBox] |= it->second;
    }
}

std::vector<QPoint> Conflicts::conflictList() const
{
    std::vector<QPoint> conf;
    SudokuMask temp;
    for (unsigned int row = 0; row < rowConflicts.size(); ++row) {
        temp = rowConflicts[row];
        for (int col = 0; temp != 0; ++col) {
            if (temp & 1) {
                conf.push_back(QPoint(row, col));
            }
            temp >>= 1;
        }
    }
    for (unsigned int col = 0; col < colConflicts.size(); ++col) {
        temp = colConflicts[col];
        for (int row = 0; temp != 0; ++row) {
            if (temp & 1) {
                conf.push_back(QPoint(row, col));
            }
            temp >>= 1;
        }
    }

    for (unsigned int box = 0; box < boxConflicts.size(); ++box) {
        temp = boxConflicts[box];
        int topLeftRow = box/board->boxLength()*board->boxLength();
        int topLeftCol = box%board->boxLength()*board->boxLength();
        for (int i = 0; temp != 0; ++i) {
            int xBox = topLeftRow + i/board->boxLength();
            int yBox = topLeftCol + i%board->boxLength();
            if (temp & 1) {
                conf.push_back(QPoint(xBox, yBox));
            }
            temp >>= 1;
        }
    }
    return conf;
}

void Conflicts::onCellChanged(int row, int col)
{
    updateConflict(row, col);
    emit conflictChanged(conflictList());
}

void Conflicts::onReset()
{
    rowConflicts.clear();
    colConflicts.clear();
    rowConflicts.resize(board->boardLength());
    colConflicts.resize(board->boardLength());
}
