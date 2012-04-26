#ifndef POLIS_H
#define POLIS_H

#include <QtGui>

#include "communicator.h"
#include "gridwidget.h"

class polis : public QMainWindow
{
    Q_OBJECT

public:
    polis(QWidget *parent = 0);
    ~polis();

private:
    communicator *com1, *com2;
    gridwidget* grid;
    QPushButton* button_connect;
    QPushButton* button_start;
    QPushButton* button_stop;
    QDoubleSpinBox* spinBox_glow;

private slots:
    void stopCom();
    void startCom();
    void comStopped();
    void toggleConnect();
    void calculate();
};

#endif // POLIS_H
