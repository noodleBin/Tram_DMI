#ifndef TLEEVENTS_H
#define TLEEVENTS_H

#include <QPainter>
#include <QFrame>
#include <QEvent>
#include <QKeyEvent>
#include "DMISignal.h"

class TLEEvents : public QWidget
{
    Q_OBJECT
public:
    explicit TLEEvents(quint8 size, QWidget *parent = 0);
    ~TLEEvents();

    void setvalue(quint8 p_image, quint8 *p_xpos, quint8 *p_ypos, quint8 *p_statuspos, quint8 *p_rotationpos, quint8 count);
    bool display9cubic;

protected:
    void paintEvent(QPaintEvent *);
    QSize sizeHint() const;



private:
    quint8 image,signals_count;
    quint16 *xpos,*ypos;
    quint8 m_size,*status,*rotation;

    qint16 arrow_x,arrow_y;



    //    quint8 m_background;
    //    quint8 m_type;
    //    quint8 m_position;
    //    quint8 m_orivalue;

    //    quint8 m_restricpos;
    //    quint8 m_restricvalue;

    //

    //    MySignal *ms,*ms2,*ms3;


    //    void   drawEvent1(QPainter* painter);
    //    void   drawEvent2(QPainter* painter);
    //    void   drawEvent3(QPainter* painter);
    //    void   drawEvent4(QPainter* painter);
    //    void   drawEvent5(QPainter* painter);
    //    void   drawEvent6(QPainter* painter);

    //    void  drawSignal(QPainter *painter, quint8 type, quint8 pos, quint8 value, MySignal *ms);

    //    void drawCar(QPainter *painter, QRect r);
        void drawArrow(QPainter*painter, QPoint &end, QColor color);
    //    void drawArrow(QPainter*painter, QPoint &p1, QPoint &p2, QPoint &p3, QColor &color);

    void drawTSharpeRoad(QPainter *painter);
    void drawImage(QPainter* );
    void drawSignals(QPainter* );
    void drawOneSignal(QPainter*, quint16,
                       quint16,
                       quint8, quint8,bool isconflict=false);
};





#endif // TLEEVENTS_H


