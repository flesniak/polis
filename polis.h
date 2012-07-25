#ifndef POLIS_H
#define POLIS_H

#include <QtGui>

#include "communicator.h"
#include "gridwidget.h"
#include "storage.h"
#include "confdlg.h"

class polis : public QMainWindow
{
    Q_OBJECT

public:
    polis(QWidget *parent = 0);
    ~polis();
    void setPortConfiguration(portConfiguration pc);

private:
    storage* store;
    communicator* com;
    gridwidget* grid;
    QPushButton* button_connect;
    QPushButton* button_start;
    QPushButton* button_stop;
    QDoubleSpinBox* spinBox_glow;
    QLabel* label_debugBeams;
    QLabel* label_delay;
    QCheckBox* checkBox_debugBeams;
    QSpinBox *spinBox_refreshDelay;
    portConfiguration p_pc;

private slots:
    void stopCom();
    void startCom();
    void comStopped();
    void reconnect();
    void displayDebugBeams(QString debugstr);
    void setDelayLabel(int delay);
    void displayPortError(QString errorstr);
};

#endif // POLIS_H
