#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <stdio.h>

#include <QThread>

#include "storage.h"
#include "confdlg.h"

class QByteArray;

class communicator : public QThread
{
    Q_OBJECT
    enum port { portx, porty, portcount };

public:
    explicit communicator(storage* store, QObject *parent = 0);
    int connect();
    int disconnect();
    bool connected();
    void setPortConfiguration(portConfiguration pc);

protected:
    void run();
    int portsend(port p, char* data, int length);
    void dataconv(QByteArray datax, QByteArray datay);
    QByteArray portread(port p, unsigned int bytes = 16);
    QString printhex(char* data, int len);
    QString printhex(QByteArray data);
    bool p_debugBeams;

private:
    int doConnect(port p);
    int doDisconnect(port p);
    bool p_stop;
    QString p_devname[portcount];
    unsigned int p_baudrate[portcount];
    portmode p_mode;
    int p_serial[portcount];
    storage* p_storage;
    unsigned int delay;

public slots:
    void stop();
    void setDebugBeams(bool on);

signals:
    void stopped();
    void debugBeams(QString debugstr);
    void delayChanged(int);
    void portError(QString errorstr);
};

#endif // COMMUNICATOR_H
