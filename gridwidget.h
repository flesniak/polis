#ifndef GRIDWIDGET_H
#define GRIDWIDGET_H

#include <QtGui>

struct point {
    unsigned short position;
    float age;
};

class gridwidget : public QWidget
{
    Q_OBJECT
public:
    explicit gridwidget(QWidget *parent = 0);
    bool enableTimer;

protected:
    void paintEvent(QPaintEvent *);
    void timerEvent(QTimerEvent *);
    QVector<point> points;

public slots:
    void addPoint(unsigned short position);
    void setGlowDuration(double duration);
    void clearPoints();

private:
    double p_glowDuration;
};

#endif // GRIDWIDGET_H
