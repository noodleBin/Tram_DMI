#include "TLEevents.h"
#define MAX 65536
#include <QDebug>
#include <QPainterPath>
TLEEvents::TLEEvents(quint8 size, QWidget *parent): QWidget(parent)
{

    m_size=size;

    xpos = new quint16[50];
    ypos = new quint16[50];
    status = new quint8[50];
    rotation = new quint8[50];
    display9cubic=false;
    this->installEventFilter(this);
    this->show();
}

TLEEvents::~TLEEvents()
{
    delete[]   xpos;
    delete[] ypos;
    delete[] status;
    delete[] rotation;
}

void TLEEvents::setvalue(quint8 p_image, quint8 *p_xpos, quint8 *p_ypos,
                         quint8* p_statuspos, quint8* p_rotationpos,
                         quint8 count)
{
    image=p_image;
    signals_count=count;

    for(int i=0;i<signals_count;i++)
    {
        rotation[i]=p_rotationpos[i];

        switch(rotation[i])
        {
        case 1:
            ypos[i]=p_ypos[i]*height()/9;
            xpos[i]=p_xpos[i]*width()/9+width()/18;
            break;
        case 2:
            xpos[i]=p_xpos[i]*width()/9;
            ypos[i]=p_ypos[i]*height()/9+height()/18;
            break;
        case 3:
            xpos[i]=p_xpos[i]*width()/9;
            ypos[i]=p_ypos[i]*height()/9-height()/18;

            break;
        case 4:
            xpos[i]=p_xpos[i]*width()/9+width()/18;
            ypos[i]=p_ypos[i]*height()/9;
            break;
        }


        status[i]=p_statuspos[i];

    }

    update();
}



void TLEEvents::paintEvent(QPaintEvent *e)
{

    Q_UNUSED(e)

    QPainter painter(this);

    painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform|QPainter::HighQualityAntialiasing);

    //    painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform);
    painter.setBrush(QColor(73,86,119));
    painter.drawRect(0,0,this->width(),this->height());

    drawImage(&painter);
    drawSignals(&painter);



    if(display9cubic)
    {
        painter.setPen(QPen( QColor(220,220,220),1,Qt::DotLine));

        for(int i=0;i<10;i++)
            painter.drawLine(0,i*height()/9,width(),i*height()/9);
        for(int i=0;i<10;i++)
            painter.drawLine(i*width()/9,0,i*width()/9,height());
    }
    else
    {
        painter.setPen(QPen( QColor(220,220,220),1,Qt::NoPen));

        for(int i=0;i<10;i++)
            painter.drawLine(0,i*height()/9,width(),i*height()/9);
        for(int i=0;i<10;i++)
            painter.drawLine(0*width()/9,0,i*width()/9,height());
    }

    painter.end();

}

QSize TLEEvents::sizeHint() const
{
    if(m_size==1)
    {
        return QSize(413,407);
    }
    else if(m_size==2)
    {
        return QSize(300,300);
    }
    else if(m_size==3)
    {
        return QSize(500,500);
    }
    return QSize(100,100);
}

void TLEEvents::drawArrow(QPainter *painter, QPoint &end,QColor color)
{
    QPoint p1,p2,p3,p4,p5,p6;
    QPoint ps[6];
    p1= QPoint(end.x()-10,end.y()-10);
    p2=        QPoint(end.x(),end.y()-25);
    p3=       QPoint(end.x()+10,end.y()-10);


    p4= QPoint(end.x()-10,end.y()+15);
    p5=        QPoint(end.x(),end.y());
    p6=       QPoint(end.x()+10,end.y()+15);

    ps[0]=p1;
    ps[1]=p2;
    ps[2]=p3;
    ps[3]=p6;
    ps[4]=p5;
    ps[5]=p4;
    painter->setPen(QPen(color,1));
    painter->setBrush(color);
    painter->drawPolygon(ps,6);
}

void TLEEvents::drawParkinglog(QPainter *painter)
{
    painter->drawLine(4*width()/9,0,4*width()/9,height());
    painter->drawLine(6*width()/9,0,6*width()/9,height());
    painter->drawLine(4*width()/9,5*height()/9,6*width()/9,7*height()/9);
    painter->drawLine(2*width()/9,2*height()/9,4*width()/9,4*height()/9);
    painter->drawLine(0,2*height()/9,2*width()/9,2*height()/9);
}



void TLEEvents::drawTSharpeRoad(QPainter *painter)
{
    QRect rect;
    painter->save();


    painter->translate(0,height()/9);
    painter->save();
    painter->setPen(QPen(QColor(64,64,64),1));
    painter->setBrush(QColor(64,64,64));
    painter->drawRect(0,1*height()/9,9*width()/9,3*height()/9);
    painter->drawRect(3*width()/9,1*height()/9,3*width()/9,7*height()/9);
    painter->restore();
    painter->drawLine(0,2*height()/9,width(),2*height()/9);
    painter->drawLine(0,3*height()/9,width(),3*height()/9);

    rect=QRect(2*width()/9,3*height()/9,2*width()/9,2*height()/9);
    painter->drawArc(rect,0,90*16);
    rect=QRect(1*width()/9,2*height()/9,4*width()/9,4*height()/9);
    painter->drawArc(rect,0,90*16);
    rect=QRect(5*width()/9,3*height()/9,2*width()/9,2*height()/9);
    painter->drawArc(rect,90*16,90*16);
    rect=QRect(4*width()/9,2*height()/9,4*width()/9,4*height()/9);
    painter->drawArc(rect,90*16,90*16);

    painter->drawLine(4*width()/9,4*height()/9,4*width()/9,height());
    painter->drawLine(5*width()/9,4*height()/9,5*width()/9,height());


    painter->restore();
}

void TLEEvents::drawImage(QPainter *painter)
{
    QPoint p1,p2,c1,c2;
    QPainterPath ppath;
    painter->save();
    painter->setPen(QPen(QColor(220,220,220),4));
    QRect rect;
    switch(image)
    {
    case 1:

        drawTSharpeRoad(painter);
        rect=QRect(5*width()/9-8,8*height()/9,16,66);
        painter->drawPixmap(rect,QPixmap("res/Tram_Tracklayout.png"));
        break;
    case 2:

        painter->save();
        painter->rotate(90);
        painter->translate(0,-500);
        drawTSharpeRoad(painter);

        painter->restore();
        rect=QRect(6*width()/9-8,8*height()/9,16,66);
        painter->drawPixmap(rect,QPixmap("res/Tram_Tracklayout.png"));



        break;
    case 3:
        painter->save();
        painter->rotate(-90);
        painter->translate(-500,0);
        drawTSharpeRoad(painter);

        painter->restore();
        rect=QRect(4*width()/9-8,8*height()/9,16,66);
        painter->drawPixmap(rect,QPixmap("res/Tram_Tracklayout.png"));
        break;
    case 4:
        painter->drawLine(3*width()/9,0,3*width()/9,height());
        painter->drawLine(6*width()/9,0,6*width()/9,height());
        //        painter->drawLine(3*width()/9,2*height()/9,6*width()/9,5*height()/9);
        p1=  QPoint (3*width()/9,2*height()/9);
        p2=  QPoint (6*width()/9,5*height()/9);

        ppath= QPainterPath (p1);

        c1 =QPoint(p1.x(),(p1.y()+p2.y())/2);
        c2= QPoint (p2.x(),(p1.y()+p2.y())/2);
        ppath.cubicTo(c1,c2,p2);

        painter->drawPath(ppath);

        rect=QRect(6*width()/9-8,8*height()/9,16,66);
        painter->drawPixmap(rect,QPixmap("res/Tram_Tracklayout.png"));
        break;
    case 5:
        painter->drawLine(3*width()/9,0,3*width()/9,height());
        painter->drawLine(6*width()/9,0,6*width()/9,height());
        //        painter->drawLine(3*width()/9,5*height()/9,6*width()/9,2*height()/9);


        p1=  QPoint (3*width()/9,5*height()/9);
        p2=  QPoint (6*width()/9,2*height()/9);

        ppath= QPainterPath (p1);

        c1 =QPoint(p1.x(),(p1.y()+p2.y())/2);
        c2= QPoint (p2.x(),(p1.y()+p2.y())/2);
        ppath.cubicTo(c1,c2,p2);

        painter->drawPath(ppath);

        rect=QRect(6*width()/9-8,8*height()/9,16,66);
        painter->drawPixmap(rect,QPixmap("res/Tram_Tracklayout.png"));
        break;
    case 6:
        painter->drawLine(3*width()/9,0,3*width()/9,height());
        painter->drawLine(6*width()/9,0,6*width()/9,height());

        p1=  QPoint (3*width()/9,5*height()/9);
        p2=  QPoint (6*width()/9,2*height()/9);

        ppath= QPainterPath (p1);

        c1 =QPoint(p1.x(),(p1.y()+p2.y())/2);
        c2= QPoint (p2.x(),(p1.y()+p2.y())/2);
        ppath.cubicTo(c1,c2,p2);

        painter->drawPath(ppath);

        p1=  QPoint (3*width()/9,2*height()/9);
        p2=  QPoint (6*width()/9,5*height()/9);

        ppath= QPainterPath (p1);

        c1 =QPoint(p1.x(),(p1.y()+p2.y())/2);
        c2= QPoint (p2.x(),(p1.y()+p2.y())/2);
        ppath.cubicTo(c1,c2,p2);

        painter->drawPath(ppath);


        rect=QRect(6*width()/9-8,8*height()/9,16,66);
        painter->drawPixmap(rect,QPixmap("res/Tram_Tracklayout.png"));
        break;
    case 7:
        painter->save();
        painter->setPen(QPen(QColor(64,64,64),1));
        painter->setBrush(QColor(64,64,64));
        painter->drawRect(0,2*height()/9,width(),2*height()/9);
        painter->setPen(QPen(Qt::yellow,2,Qt::DotLine));
        painter->drawLine(0,3*height()/9,width(),3*height()/9);
        painter->restore();

        painter->drawLine(3*width()/9,0,3*width()/9,height());
        painter->drawLine(6*width()/9,0,6*width()/9,height());
        //        painter->drawLine(0,2*height()/9,width(),2*height()/9);
        //        painter->drawLine(0,4*height()/9,width(),4*height()/9);

        rect=QRect(6*width()/9-8,8*height()/9,16,66);
        painter->drawPixmap(rect,QPixmap("res/Tram_Tracklayout.png"));
        break;
    case 8:
        drawParkinglog(painter);
        break;
    case 9:
        painter->save();
        painter->rotate(-90);
        painter->translate(-width(),0);
        drawParkinglog(painter);
        painter->restore();
        break;
    case 10:
        painter->save();
        painter->rotate(-180);
        painter->translate(-width(),-height());
        drawParkinglog(painter);
        painter->restore();
        break;
    }
    painter->restore();
}

void TLEEvents::drawSignals(QPainter *p)
{
    for(int i=0;i<signals_count-1;i++)
    {
        drawOneSignal(p,xpos[i],ypos[i],status[i],rotation[i],true);
        //        qDebug()<<"ha"<<i<<xpos[i]<<ypos[i]<<status[i]<<rotation[i];
    }
    drawOneSignal(p,xpos[signals_count-1],
            ypos[signals_count-1],
            status[signals_count-1],
            rotation[signals_count-1]);
}



void TLEEvents::drawOneSignal(QPainter *painter, quint16 x, quint16 y, quint8 s , quint8 ro, bool isconflict)
{
    painter->save();
    painter->translate(x,y);
    arrow_x=arrow_y=0;
    switch(ro)
    {
    case 1:
        arrow_x=0-height()/18;
        //        arrow_y=0+2*width()/9-width()/18;
        break;
    case 2:
        painter->rotate(90);
        arrow_x=0-height()/18;
        break;
    case 3:
        painter->rotate(-90);
        arrow_x=0-height()/18;
        //        arrow_y=0+height()/18;
        break;
    case 4:
        painter->rotate(180);
        arrow_x=0-height()/18;
        break;
    }


    painter->setPen(QPen(QColor(0,0,0),1));
    painter->setBrush(QBrush(Qt::black));

    painter->drawEllipse(0,0,width()/18,height()/18);
    painter->drawLine(0+width()/36,0+height()/18,0+width()/36,0+height()/18+height()/36);
    painter->drawLine(0,0+height()/18+height()/36,0+width()/18,0+height()/18+height()/36);

    QPoint p1,p2,p3,p4,p5,p6,c1,c2,end;
    QPoint ps[6];
    QPainterPath ppath;
    QColor arrow_color;

    switch (s)
    {

    case 1:
        painter->setPen(QPen(Qt::red,5));
        painter->drawLine(0+3,0+height()/36,0+width()/18-3,0+height()/36);
        break;
    case 2:

        painter->setPen(QPen(Qt::green,5));
        painter->drawLine(0+width()/36,0+3,0+width()/36,0+height()/18-3);



        painter->save();

        if(isconflict)
            arrow_color=Qt::red;
        else\
            arrow_color=Qt::green;

        painter->setPen(QPen(arrow_color,8));
        painter->drawLine(arrow_x,0+height()/9,arrow_x,0-4*height()/9);
        end=QPoint(arrow_x,0-4*height()/9);
        drawArrow(painter,end,arrow_color);

        painter->restore();
        break;
    case 3:
        painter->setPen(QPen(Qt::yellow,5));
        painter->drawLine(0+width()/72,0+height()/72,
                          0+width()/18-width()/72,0+height()/18-height()/72);

        painter->save();

        if(isconflict)
            arrow_color=Qt::red;
        else
            arrow_color=Qt::yellow;
        painter->setPen(QPen(arrow_color,8));


        if(image==1||image==2||image==3)
        {
            painter->drawArc(QRect(arrow_x-4*width()/9,arrow_y-2*height()/9
                                   ,4*width()/9,4*height()/9),0,90*16);

            painter->drawLine(arrow_x,arrow_y,
                              arrow_x,arrow_y+1*height()/9);
            painter->drawLine(arrow_x-2*width()/9,arrow_y-2*height()/9,
                              arrow_x-3*width()/9,arrow_y-2*height()/9);


            end=QPoint(arrow_x-3*width()/9,arrow_y-2*height()/9);

            painter->save();
            painter->translate(-2*width()/9-arrow_x,-5*height()/9-27);
            painter->rotate(-90);

            drawArrow(painter,end,arrow_color);
            painter->restore();


        }
        else
        {
            painter->translate(-x,-y);
            p1=  QPoint (6*width()/9,5*height()/9);
            p2=  QPoint (3*width()/9,2*height()/9);
            painter->setPen(QPen(arrow_color,8));
            painter->setBrush(Qt::NoBrush);
            ppath= QPainterPath (p1);

            c1 =QPoint(p1.x(),(p1.y()+p2.y())/2);
            c2= QPoint (p2.x(),(p1.y()+p2.y())/2);
            ppath.cubicTo(c1,c2,p2);
            painter->drawPath(ppath);
            painter->drawLine(p1.x(),p1.y(),p1.x(),p1.y()+height()/9);
            painter->drawLine(p2.x(),p2.y(),p2.x(),p2.y()-height()/9);
            end=QPoint(p2.x(),p2.y()-height()/9);
            drawArrow(painter,end,arrow_color);
        }
        painter->restore();
        break;
    case 4:
        painter->setPen(QPen(Qt::yellow,5));
        painter->drawLine(0+width()/18-width()/72,0+height()/72,
                          0+width()/72,0+height()/18-height()/72);



        painter->save();
        if(isconflict)
            arrow_color=Qt::red;
        else
            arrow_color=Qt::yellow;
        if(image==1||image==2||image==3)
        {
            painter->setPen(QPen(arrow_color,8));
            painter->drawArc(QRect(arrow_x,arrow_y-1*height()/9
                                   ,2*width()/9,2*height()/9),90*16,90*16);



            painter->drawLine(arrow_x,arrow_y,
                              arrow_x,arrow_y+1*height()/9);
            painter->drawLine(arrow_x+1*width()/9,arrow_y-1*height()/9,
                              arrow_x+2*width()/9,arrow_y-1*height()/9);


            end=QPoint(arrow_x+2*width()/9,arrow_y-1*height()/9);

            painter->save();
            painter->translate(1*width()/9+arrow_x,-3*height()/9+27);
            painter->rotate(90);

            drawArrow(painter,end,arrow_color);
            painter->restore();

        }
        else
        {
            painter->translate(-x,-y);
            p1=  QPoint (3*width()/9,5*height()/9);
            p2=  QPoint (6*width()/9,2*height()/9);
            painter->setPen(QPen(arrow_color,8));
            painter->setBrush(Qt::NoBrush);
            ppath= QPainterPath (p1);

            c1 =QPoint(p1.x(),(p1.y()+p2.y())/2);
            c2= QPoint (p2.x(),(p1.y()+p2.y())/2);
            ppath.cubicTo(c1,c2,p2);
            painter->drawPath(ppath);
            painter->drawLine(p1.x(),p1.y(),p1.x(),p1.y()+height()/9);
            painter->drawLine(p2.x(),p2.y(),p2.x(),p2.y()-height()/9);
            end=QPoint(p2.x(),p2.y()-height()/9);
            drawArrow(painter,end,arrow_color);
        }

        painter->restore();
        break;
    }

    painter->restore();
}



