#ifndef STORAGE_H
#define STORAGE_H

#include <QThread>
#include <QMutex>

struct point;
template <typename T> class QVector;

class storage : public QThread
{
    Q_OBJECT
public:
    storage(QObject *parent = 0);
    const QVector<point>* data() const;
    double glowDuration() const;

protected:
    void run();
    QVector<point>* p_data;
    void timerEvent(QTimerEvent *);

private:
    QMutex mutex;
    double p_glowDuration;
    unsigned short timerinterval;
    int timerId;

public slots:
    void addPoint(unsigned short position);
    void beginAddingPoints();
    void endAddingPoints();
    void setGlowDuration(double duration);
    void setTimerInterval(int ti);

signals:
    void dataChanged();
};

#endif // STORAGE_H
