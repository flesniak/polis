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
#include <QByteArray>
#include <QMutex>
#include <QBitArray>

class communicator : public QThread
{
    Q_OBJECT
public:
    explicit communicator(QString devname, QObject *parent = 0);
    int openport();
    void closeport();
    QBitArray bits;
    QMutex mutex;

protected:
    void run();
    int portsend(char* data, int length);
    QByteArray portread(unsigned int bytes = 16, unsigned int tries = 2);
    QString printhex(char* data, int len);
    void dataconv(QByteArray data);

private:
    bool p_stop;
    QString p_devname;
    int hserial1;

public slots:
    void stop();
    char connected();

signals:
    void stopped();
    void bitsComplete();
};

#endif // COMMUNICATOR_H
