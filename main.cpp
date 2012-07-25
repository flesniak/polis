#include <QtGui/QApplication>

#include "polis.h"
#include "confdlg.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    confdlg cd;
    if( cd.exec() != QDialog::Accepted )
        return 1;

    polis w;
    w.setPortConfiguration(cd.getPortConfiguration());
    w.show();

    return a.exec();
}
