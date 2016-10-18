#ifndef TLEEVENTS_H
#define TLEEVENTS_H

#include <QPainter>
#include <QFrame>
#include "DMISignal.h"

class TLEEvents : public QWidget
{
    Q_OBJECT
public:
    explicit TLEEvents(quint8 size, QWidget *parent = 0);

    void setvalue(quint8 background, quint8 type, quint8 position, quint8 orisignalstatus, quint8 restricpos, quint8 restricstatus);


protected:
    void paintEvent(QPaintEvent *);
    QSize sizeHint() const;


private:

    quint8 m_background;
    quint8 m_type;
    quint8 m_position;
    quint8 m_orivalue;

    quint8 m_restricpos;
    quint8 m_restricvalue;

    quint8 m_size;

    MySignal *ms,*ms2,*ms3;


    void   drawEvent1(QPainter* painter);
    void   drawEvent2(QPainter* painter);
    void   drawEvent3(QPainter* painter);
    void   drawEvent4(QPainter* painter);
    void   drawEvent5(QPainter* painter);
    void   drawEvent6(QPainter* painter);

    void  drawSignal(QPainter *painter, quint8 type, quint8 pos, quint8 value, MySignal *ms);

    void drawCar(QPainter *painter, QRect r);
    void drawArrow(QPainter*painter, QPoint &p1, QPoint &p2, QColor &color);
    void drawArrow(QPainter*painter, QPoint &p1, QPoint &p2, QPoint &p3, QColor &color);
};





#endif // TLEEVENTS_H


