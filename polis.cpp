#include "polis.h"

polis::polis(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("PoLis - Positionserkennung mit Lichtschrankengitter");
    setCentralWidget(new QWidget(this));

    grid = new gridwidget(centralWidget());
    com1 = new communicator("/dev/ttyUSB0",this);
    com2 = new communicator("/dev/ttyUSB1",this);

    button_connect = new QPushButton("Verbinden",centralWidget());
    button_start = new QPushButton("Start",centralWidget());
    button_start->setEnabled(false);
    button_stop = new QPushButton("Stop",centralWidget());
    button_stop->setEnabled(false);
    spinBox_glow = new QDoubleSpinBox(centralWidget());
    spinBox_glow->setRange(0,10);

    QGridLayout *toolbox_layout = new QGridLayout;
    toolbox_layout->addWidget(button_start,0,0);
    toolbox_layout->addWidget(button_stop,0,1);
    toolbox_layout->addWidget(button_connect,1,0);
    toolbox_layout->addWidget(spinBox_glow,1,1);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget());
    layout->addWidget(grid);
    layout->addLayout(toolbox_layout);

    connect(button_connect,SIGNAL(clicked()),SLOT(toggleConnect()));
    connect(button_start,SIGNAL(clicked()),SLOT(startCom()));
    connect(button_stop,SIGNAL(clicked()),SLOT(stopCom()));
    connect(com1,SIGNAL(stopped()),SLOT(comStopped()));
    connect(com2,SIGNAL(stopped()),SLOT(comStopped()));
    connect(spinBox_glow,SIGNAL(valueChanged(double)),grid,SLOT(setGlowDuration(double)));
    connect(com1,SIGNAL(bitsComplete()),SLOT(calculate()));
    connect(com2,SIGNAL(bitsComplete()),SLOT(calculate()));
}

polis::~polis()
{
    stopCom();
    if( com1->isRunning() ) {
        com1->stop();
        com1->wait(1000);
    }
    delete com1;
    if( com2->isRunning() ) {
        com2->stop();
        com2->wait(1000);
    }
    delete com2;
}

void polis::startCom()
{
    com1->start();
    com2->start();
    button_start->setEnabled(false);
    button_stop->setEnabled(true);
    button_connect->setEnabled(false);
}

void polis::stopCom()
{
    com1->stop();
    com2->stop();
}

void polis::calculate()
{
    com1->mutex.lock();
    com2->mutex.lock();

    QString str;
    str.append("com1 ");
        for(int i=0; i < com1->bits.size(); i++)
            str.append(com1->bits.at(i)?'1':'0');
        str.append(" com2 ");
        for(int i=0; i < com2->bits.size(); i++)
            str.append(com2->bits.at(i)?'1':'0');
    qDebug() << "calculate" << str;

    grid->enableTimer = false;
    grid->clearPoints();
    for(int index1 = 0; index1 < com1->bits.size(); index1++)
        if( com1->bits.at(index1) )
            for(int index2 = 0; index2 < com2->bits.size(); index2++)
                if( com2->bits.at(index2) )
                    grid->addPoint(38*index1+index2);
    grid->update();
    grid->enableTimer = true;

    com1->mutex.unlock();
    com2->mutex.unlock();
}

void polis::toggleConnect()
{
    if( com1->connected() > 0 ) {
        com1->closeport();
        button_connect->setText("Verbinden");
        button_start->setEnabled(false);
        button_stop->setEnabled(false);
    }
    else {
        if( com1->openport() == 0 ) { //successful
            button_connect->setText("Trennen");
            button_start->setEnabled(true);
        }
        else
            QMessageBox::critical(centralWidget(),"Verbindungsproblem","Die serielle Schnittstelle 1 konnte nicht verbunden werden!");
    }
    if( com2->connected() > 0 ) {
        com2->closeport();
        button_connect->setText("Verbinden");
        button_start->setEnabled(false);
        button_stop->setEnabled(false);
    }
    else {
        if( com2->openport() == 0 ) { //successful
            button_connect->setText("Trennen");
            button_start->setEnabled(true);
        }
        else
            QMessageBox::critical(centralWidget(),"Verbindungsproblem","Die serielle Schnittstelle 2 konnte nicht verbunden werden!");
    }
}

void polis::comStopped()
{
    button_start->setEnabled(true);
    button_stop->setEnabled(false);
    button_connect->setEnabled(true);
}
