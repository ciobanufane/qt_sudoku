#include <QApplication>
#include "boarddialog.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    BoardDialog * bd = new BoardDialog;
    bd->show();
    return app.exec();
}
