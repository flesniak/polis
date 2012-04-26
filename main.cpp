#include <QtGui/QApplication>

#include "polis.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    polis w;
    w.show();

    return a.exec();
}
