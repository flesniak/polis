#include "storage.h"

#include <QVector>
#include "point.h"

const float startage = -0.2;

storage::storage(QObject *parent) : QThread(parent), p_glowDuration(0), timerinterval(100)
{
    p_data = new QVector<point>();
}

void storage::run()
{
    timerId = startTimer(timerinterval);
    mutex.unlock();
    exec();
}

void storage::addPoint(unsigned short position)
{
    bool newposition = true;
    for(int index = 0; index < p_data->size(); index++)
        if( p_data->at(index).position == position ) {
            (*p_data)[index].age = startage;
            newposition = false;
        }
    if( newposition ) {
        point pnt;
        pnt.position = position;
        pnt.age = startage;
        p_data->append(pnt);
    }
}

void storage::timerEvent(QTimerEvent *)
{
    if( p_glowDuration > 0 && mutex.tryLock() ) {
        for(int num=0;num<p_data->size();num++) {
            (*p_data)[num].age += 1.0*timerinterval/1000;
            if( p_data->at(num).age >= p_glowDuration ) {
                p_data->remove(num);
                num--;
            }
        }
        mutex.unlock();
        emit dataChanged();
    }
}

const QVector<point>* storage::data() const
{
    return p_data;
}

void storage::beginAddingPoints()
{
    if( p_glowDuration <= 0 )
        p_data->clear();
    mutex.lock();
}

void storage::endAddingPoints()
{
    mutex.unlock();
    emit dataChanged();
}

void storage::setGlowDuration(double duration)
{
    p_glowDuration = duration;
}

double storage::glowDuration() const
{
    return p_glowDuration;
}

void storage::setTimerInterval(int ti)
{
    killTimer(timerId);
    timerinterval = ti;
    startTimer(timerinterval);
}
