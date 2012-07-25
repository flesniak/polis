#include <QByteArray>
#include <QBitArray>

#include "communicator.h"

static const QByteArray answerConnect = QByteArray("\x30\x30\x0D\x0A");
static const QByteArray answerAuthenticate = QByteArray("\x30\x32\x0d\x0a");
static const QByteArray answerDisconnect = QByteArray("\x30\x30\x0D\x0A");

communicator::communicator(storage* store, QObject *parent)
    : QThread(parent), p_debugBeams(false), p_storage(store), delay(20000)
{
}

void communicator::setPortConfiguration(portConfiguration pc)
{
    p_devname[portx] = "/dev/"+pc.xPort;
    p_devname[porty] = "/dev/"+pc.yPort;
    p_baudrate[portx] = pc.xBaudRate;
    p_baudrate[porty] = pc.yBaudRate;
    p_serial[portx] = -1;
    p_serial[porty] = -1;
    p_mode = pc.mode;
}

int communicator::connect() {
    int rvx = doConnect(portx);
    int rvy = 0;
    if( rvx >= 0 )
        rvy = doConnect(porty);
    return rvx+2*rvy;
}

int communicator::doConnect(port p) {
    p_serial[p] = open(p_devname[p].toUtf8(), O_RDWR | O_NOCTTY | O_NDELAY);
    if(p_serial[p] < 0)
        return -1; //failed to open

    if(fcntl(p_serial[p], F_SETFL, 0) < 0) {
        disconnect();
        return -2; //failed to set options
    }

    if(fcntl(p_serial[p], F_SETFL, FNDELAY) < 0) {
        disconnect();
        return -2; //failed to set options
    }

    struct termios options;
    tcgetattr(p_serial[p], &options);                   //Get the current options for the port
    cfsetispeed(&options, p_baudrate[p]);                    //Set the baud rates to 9600
    cfsetospeed(&options, p_baudrate[p]);
    options.c_cflag |= (CLOCAL | CREAD);             //Enable the receiver and set local mode
    if(tcsetattr(p_serial[p], TCSANOW, &options) < 0) { //Set the new options for the port
        disconnect();
        return -2; //failed to set options
    }

    if( p_mode == rs232 ) { //special initalization for rs232
        int warncount = 0;
        QByteArray query;
        QByteArray answer;
        //Init connection "0x70"
        query.append('\x70');
        portsend(p, query.data(), query.size());
        usleep(delay);
        answer = portread(p);
        if( answer != answerConnect ) {
            emit portError("Antwort auf \"Verbinden\""+printhex(answerConnect)+"erwartet,"+printhex(answer)+"erhalten, setzte trotzdem fort!");
            warncount++;
        }
        usleep(delay);

        //Authenticate "MP55534552"
        query.clear();
        query.append("\x4d\x50\x35\x35\x35\x33\x34\x35\x35\x32");
        query.append("\x0D\x0A");
        portsend(p, query.data(), query.size());
        usleep(delay);
        answer = portread(p);
        if( answer != answerAuthenticate ) {
            emit portError("Antwort auf \"Authentifizieren\""+printhex(answerAuthenticate)+"erwartet,"+printhex(answer)+"erhalten, setzte trotzdem fort!");
            warncount++;
        }
        usleep(delay);
    }

    return 0;
}

void communicator::run() {
    p_stop = false;

    if( p_serial[portx] < 0 ) {
        emit portError("Port X nicht verbunden!");
        return;
    }
    if( p_serial[porty] < 0 ) {
        emit portError("Port Y nicht verbunden!");
        return;
    }

    QByteArray query, answer[portcount];
    query.append("\x67\x62"); //getBeamStatus
    query.append("\x0D\x0A");
    while( !p_stop ) {
        if( p_mode == rs232 ) {
            portsend(portx,query.data(), query.size());
            usleep(delay);
        }
        answer[portx].clear();
        answer[portx] = portread(portx);
        answer[portx].chop(2);
        usleep(delay);

        if( p_mode == rs232 ) {
            portsend(porty,query.data(), query.size());
            usleep(delay);
        }
        answer[porty].clear();
        answer[porty] = portread(porty);
        answer[porty].chop(2);
        usleep(delay);

        dataconv(answer[portx],answer[porty]);
    }
    emit stopped();
}

int communicator::disconnect() {
    int warncount = 0;
    warncount += doDisconnect(portx);
    warncount += doDisconnect(porty);
    return warncount;
}

int communicator::doDisconnect(port p) {
    QByteArray answer, query;
    int warncount = 0;

    if( p_mode == rs232 ) { //Close connection "ME"
        query.clear();
        query.append("\x6d\x65");
        query.append("\x0D\x0A");
        portsend(p, query.data(), query.size());
        usleep(delay);
        answer = portread(p);
        if( answer != answerDisconnect ) {
            emit portError("Antwort auf \"Trennen\""+printhex(answerDisconnect)+"erwartet,"+printhex(answer)+"erhalten, setzte trotzdem fort!");
            warncount++;
        }
    }

    if( p_serial[p] > 0 )
        close(p_serial[p]);
    p_serial[p] = -1;

    return warncount;
}

bool communicator::connected() {
    return p_serial[portx] > 0 && p_serial[porty] > 0;
}

void communicator::stop() {
    p_stop = true;
}

int communicator::portsend(port p, char* data, int length) {
    long int result = write(p_serial[p], data, length);
    if(result != length)
        return -1;
    return 0;
}

QByteArray communicator::portread(port p, unsigned int bytes) {
    char data[bytes];
    int bytesRead = 0;
    QByteArray bdata;
    for(int tries = 0; tries < 5; tries++) {
        bytesRead = read(p_serial[p], data, bytes);
        if( bytesRead > 0 ) {
            if( bytesRead != 12 && isRunning() && delay < 1000000 ) {
                delay += 1000;
                emit delayChanged(delay);
                emit portError(QString::fromUtf8("Empfangene Daten beschädigt. Delay erhöht."));
            }
            else
                bdata.append(data,bytesRead);
            break;
        }
        else
            emit portError("Keine Bytes gelesen, delay zu kurz?");
    }
    return bdata;
}

QString communicator::printhex(char* data, int len) {
    QString datastr;
    for(int pos=0; pos < len; pos++) {
        datastr.append(QString::number(data[pos],16).rightJustified(2,'0'));
        if( pos != len-1 )
            datastr.append(' ');
    }
    return datastr;
}

QString communicator::printhex(QByteArray data) {
    return printhex(data.data(),data.size());
}

void communicator::dataconv(QByteArray datax, QByteArray datay) {
    printhex(datax);
    printhex(datay);
    QBitArray bits[portcount];
    bits[portx].resize(38);
    bits[porty].resize(38);
    for(unsigned char index = 0; index < datax.size(); index++) {
        unsigned char byte = datax.at(index);
        if( byte < 0x40 )
            byte -= 0x30;
        else
            byte -= 0x41-10;
        bits[portx].setBit(index*4+0,byte & 8);
        bits[portx].setBit(index*4+1,byte & 4);
        if( index+1 != datax.size() ) {
            bits[portx].setBit(index*4+2,byte & 2);
            bits[portx].setBit(index*4+3,byte & 1);
        }
    }
    for(unsigned char index = 0; index < datay.size(); index++) {
        unsigned char byte = datay.at(index);
        if( byte < 0x40 )
            byte -= 0x30;
        else
            byte -= 0x41-10;
        bits[porty].setBit(index*4+0,byte & 8);
        bits[porty].setBit(index*4+1,byte & 4);
        if( index+1 != datay.size() ) {
            bits[porty].setBit(index*4+2,byte & 2);
            bits[porty].setBit(index*4+3,byte & 1);
        }
    }

    if( p_debugBeams ) {
        QString str;
        str.append("portx ");
            for(int i=0; i < bits[portx].size(); i++)
                str.append(bits[portx].at(i)?'1':'0');
            str.append(" porty ");
            for(int i=0; i < bits[porty].size(); i++)
                str.append(bits[porty].at(i)?'1':'0');
        emit(debugBeams(str));
    }

    p_storage->beginAddingPoints();
    for(int index1 = 0; index1 < bits[portx].size(); index1++)
        if( bits[portx].at(index1) )
            for(int index2 = 0; index2 < bits[porty].size(); index2++)
                if( bits[porty].at(index2) )
                    p_storage->addPoint(38*index1+index2);
    p_storage->endAddingPoints();
}

void communicator::setDebugBeams(bool on)
{
    p_debugBeams = on;
}
