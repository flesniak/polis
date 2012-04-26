#include <QDebug>
#include <QBitArray>

#include "communicator.h"

const unsigned int delay = 40000;

communicator::communicator(QString devname, QObject *parent) : QThread(parent), p_devname(devname)
{
    hserial1 = -1;
    bits.resize(38);
}

void communicator::run()
{
    p_stop = false;
    if(hserial1 < 0) {
        qDebug() << "[com] hserial1 not connected!";
        return;
    }
    QByteArray query;

    //Init connection "0x70"
    query.append('\x70');
    portsend(query.data(), query.size());
    usleep(delay);
    portread();
    usleep(delay);

    //Authenticate "MP55534552"
    query.clear();
    query.append("\x4d\x50\x35\x35\x35\x33\x34\x35\x35\x32");
    query.append("\x0D\x0A");
    portsend(query.data(), query.size());
    usleep(delay);
    portread();
    usleep(delay);

    while( !p_stop ) {
        query.clear();
        query.append("\x67\x62"); //getBeamStatus
        query.append("\x0D\x0A");
        portsend(query.data(), query.size());
        usleep(delay);
        QByteArray data = portread();
        data.chop(2);
        dataconv(data);
    }

    //Close connection "ME"
    query.clear();
    query.append("\x6d\x65");
    query.append("\x0D\x0A");
    portsend(query.data(), query.size());
    usleep(delay);
    portread();
    usleep(delay);
    emit stopped();
}

void communicator::stop()
{
    p_stop = true;
}

int communicator::portsend(char* data, int length) {
    long int result = write(hserial1, data, length);
    //qDebug() << "portsend" << "result" << result << "data" << printhex(data,length);
    if(result != length)
        return -1;
    return 0;
}

QString communicator::printhex(char* data, int len) {
    QString datastr;
    for(int pos=0; pos < len; pos++)
        datastr.append(QString::number(data[pos],16).rightJustified(2,'0')+' ');
    return datastr;
}

QByteArray communicator::portread(unsigned int bytes, unsigned int tries) {
    char data[bytes];
    int bytesRead = 0;
    QByteArray bdata;
    for(unsigned int i=0; i<tries; i++) {
        bytesRead = read(hserial1, data, bytes);
        //qDebug() << "portread" << "bytesRead" << bytesRead << "data" << printhex(data,bytesRead);
        if( bytesRead > 0 ) {
            bdata.append(data,bytesRead);
            break;
        }
    }
    return bdata;
}

void communicator::dataconv(QByteArray data)
{
    mutex.lock();
    bits.fill(false);
    for(int index = 0; index < data.size(); index++) {
        /*unsigned char byte = QString::number((int)data.at(index),10);
        qDebug() << "dataconv" << "data" << QString::number(byte,2) << "index" << index;
        bits.setBit(index*4+0,byte & 8);
        bits.setBit(index*4+1,byte & 4);
        //if( index+1 != data.size() ) {
            bits.setBit(index*4+2,byte & 2);
            bits.setBit(index*4+3,byte & 1);
        //}*/

        switch(data.at(index)) {
        case 0x31 : bits.setBit(index*4+0,false);
                    bits.setBit(index*4+1,false);
                    bits.setBit(index*4+2,false);
                    bits.setBit(index*4+3,true);
                    break;
        case 0x32 : bits.setBit(index*4+0,false);
                    bits.setBit(index*4+1,false);
                    bits.setBit(index*4+2,true);
                    bits.setBit(index*4+3,false);
                    break;
        case 0x33 : bits.setBit(index*4+0,false);
                    bits.setBit(index*4+1,false);
                    bits.setBit(index*4+2,true);
                    bits.setBit(index*4+3,true);
                    break;
        case 0x34 : bits.setBit(index*4+0,false);
                    bits.setBit(index*4+1,true);
                    bits.setBit(index*4+2,false);
                    bits.setBit(index*4+3,false);
                    break;
        case 0x35 : bits.setBit(index*4+0,false);
                    bits.setBit(index*4+1,true);
                    bits.setBit(index*4+2,false);
                    bits.setBit(index*4+3,true);
                    break;
        case 0x36 : bits.setBit(index*4+0,false);
                    bits.setBit(index*4+1,true);
                    bits.setBit(index*4+2,true);
                    bits.setBit(index*4+3,false);
                    break;
        case 0x37 : bits.setBit(index*4+0,false);
                    bits.setBit(index*4+1,true);
                    bits.setBit(index*4+2,true);
                    bits.setBit(index*4+3,true);
                    break;
        case 0x38 : bits.setBit(index*4+0,true);
                    bits.setBit(index*4+1,false);
                    bits.setBit(index*4+2,false);
                    bits.setBit(index*4+3,false);
                    break;
        case 0x39 : bits.setBit(index*4+0,true);
                    bits.setBit(index*4+1,false);
                    bits.setBit(index*4+2,false);
                    bits.setBit(index*4+3,true);
                    break;
        case 0x41 : bits.setBit(index*4+0,true);
                    bits.setBit(index*4+1,false);
                    bits.setBit(index*4+2,true);
                    bits.setBit(index*4+3,false);
                    break;
        case 0x42 : bits.setBit(index*4+0,true);
                    bits.setBit(index*4+1,false);
                    bits.setBit(index*4+2,true);
                    bits.setBit(index*4+3,true);
                    break;
        case 0x43 : bits.setBit(index*4+0,true);
                    bits.setBit(index*4+1,true);
                    bits.setBit(index*4+2,false);
                    bits.setBit(index*4+3,false);
                    break;
        case 0x44 : bits.setBit(index*4+0,true);
                    bits.setBit(index*4+1,true);
                    bits.setBit(index*4+2,false);
                    bits.setBit(index*4+3,true);
                    break;
        case 0x45 : bits.setBit(index*4+0,true);
                    bits.setBit(index*4+1,true);
                    bits.setBit(index*4+2,true);
                    bits.setBit(index*4+3,false);
                    break;
        case 0x46 : bits.setBit(index*4+0,true);
                    bits.setBit(index*4+1,true);
                    bits.setBit(index*4+2,true);
                    bits.setBit(index*4+3,true);
                    break;
        }
    }

    /*QString str;
    for(int i=0; i < bits.size(); i++)
        str.append(bits.at(i)?'1':'0');
    qDebug() << "bitarray" << str;*/
        //ls2
    mutex.unlock();
    emit bitsComplete();
}

int communicator::openport() {
    hserial1 = open(p_devname.toUtf8(), O_RDWR | O_NOCTTY | O_NDELAY);
    if(hserial1 < 0)
        return -1;

    if(fcntl(hserial1, F_SETFL, 0) < 0) {
        closeport();
        return -1;
    }

    if(fcntl(hserial1, F_SETFL, FNDELAY) < 0) {
        closeport();
        return -1;
    }

    struct termios options;
    tcgetattr(hserial1, &options);   //Get the current options for the port
    cfsetispeed(&options, B9600);     //Set the baud rates to 115200
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD);    //Enable the receiver and set local mode
    if(tcsetattr(hserial1, TCSANOW, &options) < 0){   //Set the new options for the port
        closeport();
        return -1;
    }

    return 0;
}

void communicator::closeport() {
    if(hserial1 >= 0)
        close(hserial1);
    hserial1 = -1;
}

char communicator::connected() {
    return hserial1 > 0;
}
