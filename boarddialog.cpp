#include <QtWidgets>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include "boarddialog.h"
#include "cell.h"

DrawGridDelegate::DrawGridDelegate(int boxLength, QObject* parent) : QStyledItemDelegate(parent)
{
    this->boxLength = boxLength;
}

void DrawGridDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    const QRect rect(option.rect);
    painter->setPen(QPen(Qt::black, 3));

    if (index.row() == highlightRow || index.column() == highlightCol) {
        painter->fillRect(rect, QColor(255,0,0,50));
    }

    if (index.column()%boxLength == boxLength-1 && index.column() != index.model()->columnCount()-1) {
        painter->drawLine(rect.topRight(), rect.bottomRight());
    }
    if (index.row()%boxLength == boxLength-1 && index.row() != index.model()->rowCount()-1) {
        painter->drawLine(rect.bottomLeft(), rect.bottomRight());
    }

    emit finishPaint();

    QStyledItemDelegate::paint(painter, option, index);
}

void DrawGridDelegate::onHoverRowChanged(int row)
{
    highlightRow = row;
}

void DrawGridDelegate::onHoverColChanged(int col)
{
    highlightCol = col;
}

BoardDialog::BoardDialog(QWidget* parent)
    : QTableWidget(parent)
{
    setItemPrototype(new Cell);
    setItemDelegate(new DrawGridDelegate(BoxLength, this));
    setSelectionMode(SingleSelection);
    setMouseTracking(true);
    viewport()->setAttribute(Qt::WA_Hover);

    setRowCount(0);
    setColumnCount(0);
    setRowCount(BoxLength*BoxLength);
    setColumnCount(BoxLength*BoxLength);

    verticalHeader()->setDefaultSectionSize(30);
    verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    verticalHeader()->hide();

    horizontalHeader()->setDefaultSectionSize(30);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->hide();

    setFont(QFont("Times", 20, QFont::Bold));
    setSizeAdjustPolicy(AdjustToContents);

    connect(this, SIGNAL(hoverRowChanged(int)), itemDelegate(), SLOT(onHoverRowChanged(int)));
    connect(this, SIGNAL(hoverColChanged(int)), itemDelegate(), SLOT(onHoverColChanged(int)));
    connect(itemDelegate(), SIGNAL(finishPaint()), this, SLOT(onFinishPaint()));

    std::srand(std::time(nullptr));
}

Cell* BoardDialog::cell(int row, int col) const
{
    return static_cast<Cell*>(item(row,col));
}

void BoardDialog::setItemForeground(int row, int col, const QColor &color)
{
    Cell* c = cell(row, col);
    if (!c) {
        c = new Cell;
        setItem(row, col, c);
    }
    c->setData(Qt::ForegroundRole, color);
}

void BoardDialog::setItemBackground(int row, int col, const QColor &color)
{
    Cell* c = cell(row, col);
    if (!c) {
        c = new Cell;
        setItem(row, col, c);
    }
    c->setData(Qt::BackgroundRole, color);
}

void BoardDialog::setHighlight(int row, int col)
{
    for(int i = 0; i < BoxLength*BoxLength; ++i) {
        setItemBackground(row, i, QColor(255,0,0,100));
    }
    for(int i = 0; i < BoxLength*BoxLength; ++i) {
        setItemBackground(i, col, QColor(255,0,0,100));
    }
}

void BoardDialog::undoHighlight(int row, int col)
{
    for(int i = 0; i < BoxLength*BoxLength; ++i) {
        setItemBackground(row, i, QColor(255,255,255));
    }
    for(int i = 0; i < BoxLength*BoxLength; ++i) {
        setItemBackground(i, col, QColor(255,255,255));
    }
}

int BoardDialog::value(int row, int col) const
{
    Cell* c = cell(row, col);
    if (c) {
        return c->value();
    } else {
        return 0;
    }
}

void BoardDialog::setValue(int row, int col, int value)
{
    Cell* c = cell(row, col);
    if (!c) {
        c = new Cell;
        setItem(row, col, c);
    }
    c->setValue(value);
}

void BoardDialog::wait(int interval)
{
    QElapsedTimer* timer = new QElapsedTimer;
    timer->restart();

    while(timer->elapsed() < interval) {
        QApplication::processEvents();
    }

    delete timer;
}

bool BoardDialog::validInsert(int row, int col) {
    return validBox(row, col)
            && validRow(row)
            && validCol(col);
}

bool BoardDialog::validRow(int row){
    unsigned short int match{1};
    unsigned short int mask;

    for (int col = 0; col < BoxLength*BoxLength; ++col) {
        mask = (1 << (value(row, col)));
        if (mask != 1 && match&mask) {
            return false;
        }
        match |= mask;
    }
    return true;
}

bool BoardDialog::validCol(int col){
    unsigned short int match{1};
    unsigned short int mask;

    for (int row = 0; row < BoxLength*BoxLength; ++row) {
        mask = (1 << (value(row, col)));
        if (mask != 1 && match&mask) {
            return false;
        }
        match |= mask;
    }
    return true;
}

bool BoardDialog::validBox(int row, int col) {
    int topRow = (row / BoxLength) * BoxLength;
    int topCol = (col / BoxLength) * BoxLength;

    unsigned short int match{1};
    unsigned short int mask;

    for (int i = 0; i < BoxLength; ++i) {
        for (int j = 0; j < BoxLength; ++j) {
            mask = (1 << (value(topRow+i, topCol+j)));
            if (mask != 1 && match&mask) {
                return false;
            }
            match |= mask;
        }
    }
    return true;
}


void BoardDialog::solve()
{
    solveBoard(0);
}

void BoardDialog::mousePressEvent(QMouseEvent* e)
{
    if(e->button() == Qt::RightButton) {
        solve();
        removeClues();
        setFixedClues();
    } else {
        QTableWidget::mousePressEvent(e);
    }
}

void BoardDialog::mouseMoveEvent(QMouseEvent* e)
{
    // row is treated as Y-value here and col is treated as X-value
    int row = BoxLength*BoxLength*e->pos().y() / height();
    int col = BoxLength*BoxLength*e->pos().x() / width();
    emit hoverRowChanged(row);
    emit hoverColChanged(col);
}

void BoardDialog::onFinishPaint()
{
    viewport()->update();
}

bool BoardDialog::solveBoard(int position)
{
    if (position < 0) {
        return false;
    }

    int boardLength = BoxLength * BoxLength;

    if (position == boardLength * boardLength) {
        return true;
    }

    int x = position / boardLength;
    int y = position % boardLength;

    std::vector<int> numList(boardLength);
    std::iota(numList.begin(), numList.end(), 1);

    if (displaySolver)
        wait(milliseconds);

    if (value(x,y) == 0) {
        std::random_shuffle(numList.begin(), numList.end());
        for (unsigned int i = 0; i < numList.size(); ++i) {
            setValue(x, y, numList[i]);
            if (validInsert(x, y)) {
                if (solveBoard(position+1)) {
                    return true;
                }
            }
            setValue(x, y, 0);
        }
    } else {
        if (validInsert(x, y)) {
            return solveBoard(position+1);
        }
    }
    return false;
}

void BoardDialog::removeClues()
{
    int boardLength = BoxLength * BoxLength;
    int cluesToRemove = boardLength * boardLength - EasyClues;
    int row, col;
    while (cluesToRemove > 0) {
        row = rand() % boardLength;
        col = rand() % boardLength;
        if (value(row, col) != 0) {
            setValue(row, col, 0);
            setValue(boardLength-row-1, boardLength-col-1, 0);
            cluesToRemove -= 2;
        }
    }
}

void BoardDialog::setFixedClues()
{
    for (int row = 0; row < BoxLength*BoxLength; ++row) {
        for (int col = 0; col < BoxLength*BoxLength; ++col) {
            if (value(row, col) != 0) {
                setItemForeground(row, col, QColor(Qt::red));
                cell(row, col)->setFlags(Qt::NoItemFlags | Qt::ItemIsSelectable);
            }
        }
    }
}
