#include "gridwidget.h"

#include <QPainter>

const unsigned char gridsize = 38;
const unsigned short timerinterval = 100;
const float startage = -0.2;

gridwidget::gridwidget(QWidget *parent) : QWidget(parent), enableTimer(true)
{
    setMinimumSize(200,200);
    p_glowDuration = 0;
    startTimer(timerinterval);
}

void gridwidget::timerEvent(QTimerEvent *)
{
    if( !enableTimer )
        return;
    if( p_glowDuration > 0 )
        for(int num=0;num<points.size();num++) {
            points[num].age += 1.0*timerinterval/1000;
            if( points.at(num).age >= p_glowDuration ) {
                points.remove(num);
                num--;
            }
        }
    update();
}

void gridwidget::clearPoints()
{
    if( p_glowDuration <= 0 )
        points.clear();
}

void gridwidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.fillRect(painter.window(),QColor(Qt::white));
    float x = 5;
    while( round(x) <= height()-5 ) {
        painter.drawLine(5,x,width()-5,x);
        x+=1.0*(height()-10)/gridsize;
    }
    x = 5;
    while( round(x) <= width()-5 ) {
        painter.drawLine(x,5,x,height()-5);
        x+=1.0*(width()-10)/gridsize;
    }
    QColor color = QColor(Qt::black);
    foreach(point pnt, points) {
        painter.setBrush(Qt::SolidPattern);
        if( p_glowDuration > 0 && pnt.age > 0)
            color.setAlphaF(1.0-pnt.age/p_glowDuration);
        else
            color.setAlphaF(1.0);
        painter.setPen(color);
        painter.setBrush(QBrush(color));
        painter.drawEllipse(QPointF(1.0*(pnt.position/gridsize+0.5)*(width()-10)/gridsize+5,1.0*(pnt.position%gridsize+0.5)*(height()-10)/gridsize+5-1),(width()-10)/gridsize/2-1,(height()-10)/gridsize/2-1);
    }
}

void gridwidget::addPoint(unsigned short position)
{
    bool newposition = true;
    for(int index = 0; index < points.size(); index++)
        if( points.at(index).position == position ) {
            points[index].age = startage;
            newposition = false;
        }
    if( newposition ) {
        point pnt;
        pnt.position = position;
        pnt.age = startage;
        points.append(pnt);
    }
    //update();
}

void gridwidget::setGlowDuration(double duration)
{
    p_glowDuration = duration;
}
