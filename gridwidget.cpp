#include "gridwidget.h"

#include <QPainter>
#include <QVector>

#include "point.h"
#include "storage.h"

const unsigned char gridsize = 38;

gridwidget::gridwidget(storage* store, QWidget *parent) : QWidget(parent), p_storage(store)
{
    setMinimumSize(200,200);
}

void gridwidget::paintEvent(QPaintEvent *) {
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
    const QVector<point>* vector = p_storage->data();
    for(int index=0; index < vector->size(); index++) {
        painter.setBrush(Qt::SolidPattern);
        if( p_storage->glowDuration() > 0 && vector->at(index).age > 0)
            color.setAlphaF(1.0-vector->at(index).age/p_storage->glowDuration());
        else
            color.setAlphaF(1.0);
        painter.setPen(color);
        painter.setBrush(QBrush(color));
        painter.drawEllipse(QPointF(1.0*(vector->at(index).position/gridsize+0.5)*(width()-10)/gridsize+5,1.0*(vector->at(index).position%gridsize+0.5)*(height()-10)/gridsize+5-1),(width()-10)/gridsize/2-1,(height()-10)/gridsize/2-1);
    }
}
