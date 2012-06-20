#ifndef GRIDWIDGET_H
#define GRIDWIDGET_H

#include <QtGui>

class storage;

class gridwidget : public QWidget
{
    Q_OBJECT
public:
    explicit gridwidget(storage* store, QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *);

private:
    storage* p_storage;
};

#endif // GRIDWIDGET_H
