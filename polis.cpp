#include "polis.h"

#include <QDebug>

polis::polis(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("PoLis - Positionserkennung mit Lichtschrankengitter");
    setCentralWidget(new QWidget(this));

    store = new storage(this);
    grid = new gridwidget(store,centralWidget());
    grid->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    com = new communicator("/dev/ttyUSB0","/dev/ttyUSB1",store,this);

    QDockWidget* dw_toolbox = new QDockWidget("Toolbox",this);
    QWidget* dw_toolbox_widget = new QWidget(dw_toolbox);
    dw_toolbox->setWidget(dw_toolbox_widget);
    button_connect = new QPushButton("Verbinden",dw_toolbox_widget);
    button_start = new QPushButton("Start",dw_toolbox_widget);
    button_start->setEnabled(false);
    button_stop = new QPushButton("Stop",dw_toolbox_widget);
    button_stop->setEnabled(false);
    spinBox_glow = new QDoubleSpinBox(dw_toolbox_widget);
    spinBox_glow->setRange(0,10);
    spinBox_glow->setSuffix("s");
    spinBox_refreshDelay = new QSpinBox(dw_toolbox_widget);
    spinBox_refreshDelay->setSuffix("ms");
    spinBox_refreshDelay->setRange(50,500);
    spinBox_refreshDelay->setSingleStep(10);
    spinBox_refreshDelay->setValue(100);
    checkBox_debugBeams = new QCheckBox(dw_toolbox_widget);
    checkBox_debugBeams->setText("Beam-Status anzeigen");
    label_delay = new QLabel(dw_toolbox_widget);
    addDockWidget(Qt::LeftDockWidgetArea,dw_toolbox);
    QLabel* label_glowDuration = new QLabel("Nachleuchtzeit",dw_toolbox_widget);
    QLabel* label_refreshDelay = new QLabel("Anzeigeaktualisierung",dw_toolbox_widget);

    QVBoxLayout *toolbox_layout = new QVBoxLayout(dw_toolbox_widget);
    toolbox_layout->addWidget(button_start);
    toolbox_layout->addWidget(button_stop);
    toolbox_layout->addWidget(button_connect);
    toolbox_layout->addWidget(label_glowDuration);
    toolbox_layout->addWidget(spinBox_glow);
    toolbox_layout->addWidget(checkBox_debugBeams);
    toolbox_layout->addWidget(label_refreshDelay);
    toolbox_layout->addWidget(spinBox_refreshDelay);
    toolbox_layout->addWidget(label_delay);
    toolbox_layout->addStretch();

    label_debugBeams = new QLabel(centralWidget());
    label_debugBeams->setVisible(false);
    label_debugBeams->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Minimum);

    QVBoxLayout *view_layout = new QVBoxLayout(centralWidget());
    view_layout->addWidget(grid);
    view_layout->addWidget(label_debugBeams);

    statusBar();

    connect(button_connect,SIGNAL(clicked()),SLOT(toggleConnect()));
    connect(button_start,SIGNAL(clicked()),SLOT(startCom()));
    connect(button_stop,SIGNAL(clicked()),SLOT(stopCom()));
    connect(com,SIGNAL(stopped()),SLOT(comStopped()));
    connect(spinBox_glow,SIGNAL(valueChanged(double)),store,SLOT(setGlowDuration(double)));
    connect(store,SIGNAL(dataChanged()),grid,SLOT(update()));
    connect(com,SIGNAL(debugBeams(QString)),SLOT(displayDebugBeams(QString)));
    connect(checkBox_debugBeams,SIGNAL(toggled(bool)),com,SLOT(setDebugBeams(bool)));
    connect(checkBox_debugBeams,SIGNAL(toggled(bool)),label_debugBeams,SLOT(setVisible(bool)));
    connect(com,SIGNAL(delayChanged(int)),SLOT(setDelayLabel(int)));
    connect(com,SIGNAL(portError(QString)),SLOT(displayPortError(QString)));
    connect(spinBox_refreshDelay,SIGNAL(valueChanged(int)),store,SLOT(setTimerInterval(int)));

    store->start();
}

polis::~polis()
{
    stopCom();
    store->quit();
    store->wait();
    delete store;
    if( com->isRunning() ) {
        com->stop();
        com->wait(2000);
    }
    delete com;
}

void polis::startCom()
{
    com->start();
    button_start->setEnabled(false);
    button_stop->setEnabled(true);
    button_connect->setEnabled(false);
}

void polis::stopCom()
{
    com->stop();
}

void polis::toggleConnect()
{
    if( com->connected() > 0 ) {
        com->disconnect();
        button_connect->setText("Verbinden");
        button_start->setEnabled(false);
        button_stop->setEnabled(false);
    }
    else {
        if( com->connect() == 0 ) { //successful
            button_connect->setText("Trennen");
            button_start->setEnabled(true);
        }
        else
            QMessageBox::critical(centralWidget(),"Verbindungsproblem","Die serielle Schnittstelle konnte nicht verbunden werden!");
    }
}

void polis::comStopped()
{
    button_start->setEnabled(true);
    button_stop->setEnabled(false);
    button_connect->setEnabled(true);
}

void polis::displayDebugBeams(QString debugstr)
{
    label_debugBeams->setText(debugstr);
}

void polis::setDelayLabel(int delay)
{
    label_delay->setText("Delay: "+QString::number(delay/1000)+"ms");
}

void polis::displayPortError(QString errorstr)
{
    statusBar()->showMessage(errorstr,3000);
    qDebug() << "[Port-Fehler]" << errorstr;
}
