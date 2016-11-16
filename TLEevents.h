#ifndef TLEEVENTS_H
#define TLEEVENTS_H

#include <QPainter>
#include <QFrame>
#include <QEvent>
#include <QKeyEvent>
#include "tlepublicvariables.h"

class TLEEvents : public QWidget
{
    Q_OBJECT
public:
    explicit TLEEvents(quint8 size, QWidget *parent = 0);
    ~TLEEvents();

    void setvalue(quint8 id, quint8 status, quint16 bitmaplength, quint8 *bitmap);
    bool display9cubic;



protected:
    void paintEvent(QPaintEvent *);
    QSize sizeHint() const;



private:
    quint8 image,signals_count;
    quint16 *xpos,*ypos;
    quint8 m_size,*status,*rotation;

    qint16 arrow_x,arrow_y;

    bool need_paint;

    quint8 sigid,sigstatus;
    quint16 m_bitmaplength;
    quint8* m_bitmap;

    float grid;




    void drawArrow(QPainter*painter, QPoint &end, QColor color);
    //    void drawArrow(QPainter*painter, QPoint &p1, QPoint &p2, QPoint &p3, QColor &color);




    QList<Shape>* tplete;
    View view;

    QList<SignalInfo> *siglist;
    QList<SignalInfo> *olclist;

    Entry entry;
    QString entryname;

    void drawSignal(quint8 x,quint8 y,quint8 rotate,quint8 status,QPainter *painter);

    QMap<quint8,QString> *map_sigid_name;
    QMap<quint8,QString> *map_olcid_name;
    QMap<QString,SignalBit> *map_Signal;
    QMap<QString,SignalBit> *map_OLC;
    QMap<quint8,QList<Shape>* > *map_Template;
    QMap<quint8,View> *map_ViewID;
    QMap<QString,Entry> *map_sigentry;
    /**********end***********/

public:

    void setMap(QMap<quint8, QString> *mapsigidname,
                QMap<quint8, QString> *mapolcidname,
                QMap<QString, SignalBit> *mapsignal,
                QMap<QString, SignalBit> *mapolc,
                QMap<quint8, QList<Shape> *> *mapTemplate,
                QMap<quint8, View> *mapViewID,
                QMap<QString, Entry> *mapsigentry);


};





#endif // TLEEVENTS_H


