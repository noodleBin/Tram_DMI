#include "TLEevents.h"
#define MAX 65536
#include <QDebug>
#include <QPainterPath>
TLEEvents::TLEEvents(quint8 size, QWidget *parent): QWidget(parent)
{

    m_size=size;
    //    ms= new MySignal(m_size,this);
    //    ms2 = new MySignal(m_size,this);
    //    ms3= new MySignal(m_size,this);
    xpos = new quint16[50];
    ypos = new quint16[50];
    status = new quint8[50];
    rotation = new quint8[50];
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
//    qDebug()<<"sig count"<<signals_count;
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
            xpos[i]=p_xpos[i]*width()/9-width()/18;
            ypos[i]=p_ypos[i]*height()/9-height()/18;
            break;
        }


        status[i]=p_statuspos[i];

//        qDebug()<<"f"<<xpos[i]<<ypos[i]<<status[i];
    }

    //    signals_count=1;
    //    xpos[0]=5*width()/9+width()/18;
    //    ypos[0]=4*height()/9;
    //    status[0]=4;
    //    rotation[0]=1;
    update();

}

//void TLEEvents::setvalue(quint8 background, quint8 type, quint8 position, quint8 orisignalstatus
//                         ,quint8 restricpos,quint8 restricstatus)
//{
//    m_background=background;
//    m_type=type;
//    m_position=position;
//    m_orivalue=orisignalstatus;
//    m_restricpos=restricpos;
//    m_restricvalue=restricstatus;
//    update();

//}

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

    //    m_background=1;
    //    switch (m_background)
    //    {
    //    case 1:
    //        drawEvent1(&painter);
    //        break;
    //    case 2:
    //        drawEvent2(&painter);
    //        break;
    //    case 3:
    //        drawEvent3(&painter);
    //        break;
    //    case 4:
    //        drawEvent4(&painter);
    //        break;
    //    case 5:
    //        drawEvent5(&painter);
    //        break;
    //    case 6:
    //        drawEvent6(&painter);
    //        break;
    //    default:
    //        break;
    //    }
    //    m_type=1;
    //    m_position=5;
    //    m_value=2;
    //    qDebug()<<"mm"<<m_position<<m_restricpos;


    //    drawCar(&painter);
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

void TLEEvents::drawImage(QPainter *painter)
{
    painter->save();
    painter->setPen(QPen(QColor(220,220,220),4));
    QRect rect;
    switch(image)
    {
    case 1:
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

        rect=QRect(5*width()/9-8,8*height()/9,16,66);
        painter->drawPixmap(rect,QPixmap("res2/Tram_Tracklayout.png"));
        break;
    }
    painter->restore();
}

void TLEEvents::drawSignals(QPainter *p)
{
    for(int i=0;i<signals_count;i++)
    {
        drawOneSignal(p,xpos[i],ypos[i],status[i],rotation[i]);
//        qDebug()<<"ha"<<i<<xpos[i]<<ypos[i]<<status[i]<<rotation[i];
    }
}

void TLEEvents::drawOneSignal(QPainter *painter, quint16 x, quint16 y, quint8 s , quint8 ro)
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
        break;
    case 4:
        painter->rotate(180);
        break;
    }

    //    painter->rotate(90);
    painter->setPen(QPen(QColor(0,0,0),1));
    painter->setBrush(QBrush(Qt::black));
    //    painter->drawEllipse(x,y,width()/18,height()/18);
    painter->drawEllipse(0,0,width()/18,height()/18);
    painter->drawLine(0+width()/36,0+height()/18,0+width()/36,0+height()/18+height()/36);
    painter->drawLine(0,0+height()/18+height()/36,0+width()/18,0+height()/18+height()/36);



    switch (s)
    {
    case 1:
        painter->setPen(QPen(Qt::red,5));
        painter->drawLine(0+3,0+height()/36,0+width()/18-3,0+height()/36);
        break;
    case 2:
        painter->setPen(QPen(Qt::green,5));
        painter->drawLine(0+width()/36,0+3,0+width()/36,0+height()/18-3);




        painter->drawLine(arrow_x,0+height()/9,arrow_x,0-height()/9);
        painter->drawLine(arrow_x-10,-height()/9+10,arrow_x,-height()/9);
        painter->drawLine(arrow_x+10,-height()/9+10,arrow_x,-height()/9);
        break;
    case 3:
        painter->setPen(QPen(Qt::yellow,5));
        painter->drawLine(0+width()/72,0+height()/72,
                          0+width()/18-width()/72,0+height()/18-height()/72);
        //        painter->drawLine(arrow_x,0+height()/9,arrow_x,0-height()/9);

//        painter->drawArc(QRect(arrow_x,arrow_y,4*width()/9,4*height()/9),0,90*16);
//        painter->drawLine(arrow_x+2*width()/9,arrow_y,arrow_x+width()/9,arrow_y);
        //                painter->drawLine(arrow_x-10,-height()/9+10,arrow_x,-height()/9);
        //                painter->drawLine(arrow_x+10,-height()/9+10,arrow_x,-height()/9);
        break;
    case 4:
        painter->setPen(QPen(Qt::yellow,5));
        painter->drawLine(0+width()/18-width()/72,0+height()/72,
                          0+width()/72,0+height()/18-height()/72);

        break;
    }



    painter->restore();
}

//void TLEEvents::drawArrow(QPainter *painter, QPoint &p1, QPoint &p2, QColor &color)
//{
//    painter->save();
//    painter->setPen(QPen(color,6));
//    QPoint end(p2.x(),p2.y()+(p1.y()-p2.y())/10);
//    painter->drawLine(p1,end);
//    QPoint leftedge(p2.x()-width()/30,p2.y()+(p1.y()-p2.y())/10);
//    QPoint rightedge(p2.x()+width()/30,p2.y()+(p1.y()-p2.y())/10);


//    QPoint ps[3]={
//        leftedge,p2,rightedge,
//    };
//    painter->setPen(QPen(color,1));
//    painter->setBrush(color);
//    painter->drawPolygon(ps,3);


//    painter->restore();
//}

//void TLEEvents::drawArrow(QPainter *painter, QPoint &p1, QPoint &p2, QPoint &p3, QColor &color)
//{
//    painter->save();
//    painter->setPen(QPen(color,6));


//    QPainterPath ppath(p1);

//    QPointF c1(p1.x(),(p1.y()+p2.y())/2);
//    QPointF c2(p2.x(),(p1.y()+p2.y())/2);
//    ppath.cubicTo(c1,c2,p2);

//    painter->drawPath(ppath);


//    painter->drawLine(p3,p1);

//    QPoint end(p2.x(),p2.y()-(p1.y()-p2.y())/5);
//    painter->drawLine(p2,QPoint(p2.x(),p2.y()-(p1.y()-p2.y())/10));
//    QPoint leftedge(p2.x()-width()/30,p2.y()-(p1.y()-p2.y())/10);
//    QPoint rightedge(p2.x()+width()/30,p2.y()-(p1.y()-p2.y())/10);

//    QPoint ps[3]={
//        leftedge,end,rightedge
//    };
//    painter->setPen(QPen(color,1));
//    painter->setBrush(color);
//    painter->drawPolygon(ps,3);
//    painter->restore();
//}

//void TLEEvents::drawSignal(QPainter* painter,quint8 type,quint8 pos, quint8 value,MySignal* ms)
//{
//    QRect rect;
//    QPoint begin,end1,end2,end3;
//    QColor color;
//    switch (pos)
//    {
//    case 1:
//        //        rect=QRect(2*width()/9,3*height()/4,width()/15,height()/15);
//        //        ms->setValue(type,value,rect);

//        switch (value)
//        {
//        case 1:

//            rect=QRect(4*width()/6+8,4*height()/6+8,width()/12,height()/12);
//            ms->setValue(type,value,rect);
//            break;

//        case 2:
//            rect=QRect(4*width()/6+8,4*height()/6+8,width()/12,height()/12);
//            ms->setValue(type,value,rect);

//            begin=QPoint(4*width()/6,5*height()/6);
//            end1=QPoint(4*width()/6,1*height()/6);

//            color=Qt::yellow;
//            drawArrow(painter,begin,end1,color);
//            break;
//        case 4:
//            rect=QRect(3*width()/6+8,4*height()/6+8,width()/12,height()/12);
//            ms->setValue(type,2,rect);

//            begin=QPoint(3*width()/6,5*height()/6);
//            end1=QPoint(3*width()/6,1*height()/6);

//            color=Qt::yellow;
//            drawArrow(painter,begin,end1,color);
//            break;

//        case 3: //need reverse

//            painter->setPen(QPen(Qt::yellow,8));
//            QPainterPath ppath(QPointF(2*width()/3,1*height()/4));

//            QPointF c1(width()*2/3,height()/2);
//            QPointF c2(width()/3,height()/2);
//            ppath.cubicTo(c1,c2,QPointF(width()/3,3*height()/4));

//            painter->drawPath(ppath);

//            painter->drawLine(QPoint(2*width()/3,1*height()/4),QPoint(2*width()/3,3*height()/16));

//            painter->setPen(QPen(Qt::yellow,1));
//            QPoint parray[6]={
//                QPoint(2*width()/3,3*height()/16-8),
//                QPoint(2*width()/3-10,3*height()/16+8),
//                QPoint(2*width()/3-10,3*height()/16+24),
//                QPoint(2*width()/3,3*height()/16+8),
//                QPoint(2*width()/3+10,3*height()/16+24),
//                QPoint(2*width()/3+10,3*height()/16+8),
//            };
//            painter->setBrush(Qt::yellow);
//            painter->drawPolygon(parray,6);


//            break;



//        }

//        break;
//    case 2:
//        rect=QRect(2*width()/6,1*height()/6,width()/12,height()/12);
//        ms->setValue(type,value,rect);

//        break;
//    case 3:
//        rect=QRect(2*width()/6,4*height()/6+8,width()/12,height()/12);
//        ms->setValue(type,value,rect);

//        switch(value)
//        {
//        case 2:

//            //            qDebug()<<"rescric allow green arrow";
//            begin=QPoint(3*width()/9,1*height()/8);
//            end1=QPoint(3*width()/9,6*height()/8);
//            color=Qt::red;
//            drawArrow(painter,begin,end1,color);
//            break;
//        case 3:

//            painter->setPen(QPen(Qt::yellow,6));
//            QPoint p1(width()/3,height()/4);
//            QPoint p2(2*width()/3,height()*3/4);
//            QPoint p3(1*width()/3,height()*1/8);
//            color=Qt::red;
//            drawArrow(painter,p1,p2,p3,color);

//            break;
//        }

//        break;
//    case 4:

//        //        qDebug()<<"it is 4";
//        //        rect=QRect(2*width()/9,1*height()/4,width()/15,height()/15);
//        //        ms->setValue(type,value,rect,1);
//        //       rect=QRect(6*width()/9+20,3*height()/4,width()/15,height()/15);
//        rect=QRect(6*width()/9+20,1*height()/4,width()/15,height()/15);
//        ms->setValue(type,value,rect,1);
//        switch(value)
//        {
//        case 2:

//            //            qDebug()<<"rescric allow green arrow";
//            begin=QPoint(6*width()/9,1*height()/8);
//            end1=QPoint(6*width()/9,7*height()/8);
//            color=Qt::red;
//            drawArrow(painter,begin,end1,color);
//            break;
//        case 3:

//            painter->setPen(QPen(Qt::yellow,6));
//            QPoint p1(2*width()/3,height()/4);
//            QPoint p2(1*width()/3,height()*3/4);
//            QPoint p3(2*width()/3,height()*1/8);
//            color=Qt::red;
//            drawArrow(painter,p1,p2,p3,color);

//            break;
//        }

//        break;

//    case 5:


//        switch (value)
//        {
//        case 1:

//            rect=QRect(7*width()/9,2*height()/4,width()/9,height()/8);
//            ms->setValue(type,value,rect);
//            break;

//        case 2:
//            rect=QRect(7*width()/9,2*height()/4,width()/9,height()/8);
//            ms->setValue(type,value,rect);

//            begin=QPoint(6*width()/9,5*height()/8);
//            end1=QPoint(6*width()/9,1*height()/8);
//            //            drawArrow(painter,begin,end1);
//            break;

//        case 3: //need reverse
//            rect=QRect(2*width()/9,3*height()/4,width()/9,height()/8);
//            ms->setValue(type,value,rect);

//            begin=QPoint(3*width()/9,7*height()/8);
//            end1=QPoint(3*width()/9,3*height()/4);
//            end2=QPoint(2*width()/3,4*height()/8);
//            end3=QPoint(2*width()/3,3*height()/8);

//            //            drawArrow(painter,begin,end1,end2,end3);
//            break;
//        default:
//            break;
//        }

//        break;

//    case 6:


//        rect=QRect(6*width()/9+20,3*height()/4,width()/15,height()/15);
//        ms->setValue(type,value,rect);

//        color=Qt::green;

//        switch (value)
//        {
//        case 2:
//            begin=QPoint(6*width()/9,7*height()/8);
//            end1=QPoint(6*width()/9,1*height()/8);
//            drawArrow(painter,begin,end1,color);
//            break;

//        case 3:
//            begin=QPoint(6*width()/9,6*height()/8);
//            end1=QPoint(3*width()/9,2*height()/8);
//            end2=QPoint(6*width()/9,7*height()/8);
//            drawArrow(painter,begin,end1,end2,color);
//            break;
//        default:
//            break;
//        }


//        break;

//    case 7:
//        rect=QRect(3*width()/6+18,1*height()/6,width()/12,height()/12);
//        ms->setValue(type,1,rect);
//        break;
//    case 8:
//        rect=QRect(1*width()/6+18,1*height()/6,width()/12,height()/12);
//        ms->setValue(type,1,rect);
//        break;

//    case 11:
//        rect=QRect(4*width()/6+8,4*height()/6+8,width()/12,height()/12);
//        ms->setValue(type,1,rect);

//        begin=QPoint(4*width()/6,5*height()/6);
//        end1=QPoint(4*width()/6,1*height()/6);

//        color=Qt::yellow;
//        drawArrow(painter,begin,end1,color);
//    default:
//        break;
//    }

//}

//void TLEEvents::drawCar(QPainter *painter, QRect r)
//{
//    painter->save();
//    painter->drawPixmap(r,QPixmap("res/Tram_Tracklayout.png"));
//    painter->restore();
//}




//void TLEEvents::drawEvent1(QPainter *painter)
//{
//    painter->save();

//    //    painter->setPen(QPen(QColor(220,220,220),4));
//    painter->setPen(QPen(QColor(194,194,193),4));
//    painter->drawLine(width()/3,0,width()/3,height());
//    painter->drawLine(2*width()/3,0,2*width()/3,height());

//    //    painter->drawLine(width()/3,1*height()/4,2*width()/3,3*height()/4);
//    //    painter->drawLine(2*width()/3,1*height()/4,1*width()/3,3*height()/4);

//    QPainterPath ppath(QPointF(2*width()/3,1*height()/4));

//    QPointF c1(width()*2/3,height()/2);
//    QPointF c2(width()/3,height()/2);
//    ppath.cubicTo(c1,c2,QPointF(width()/3,3*height()/4));

//    painter->drawPath(ppath);


//    QPainterPath ppath2(QPointF(width()/3,height()/4));
//    ppath2.cubicTo(c2,c1,QPointF(2*width()/3,3*height()/4));
//    painter->drawPath(ppath2);

//    QRect rr(width()/3,3*height()/8,width()/3,height()/4);
//    //    painter->drawEllipse(rr);

//    //        painter->setPen(QPen(Qt::red,4));
//    //    painter->drawArc(rr,0*16,-180*16);

//    QRect rr1(width()/3,5*height()/8,width()/3,height()/4);
//    //    painter->drawArc(rr1,0*16,180*16);

//    painter->restore();
//}

//void TLEEvents::drawEvent3(QPainter *painter)
//{
//    painter->save();

//    painter->setPen(QPen(QColor(220,220,220),4));
//    painter->drawLine(width()/3,0,width()/3,height());
//    painter->drawLine(2*width()/3,0,2*width()/3,height());

//    //    painter->drawLine(width()/3,2*height()/4,2*width()/3,3*height()/4);
//    //    painter->drawLine(2*width()/3,2*height()/4,1*width()/3,3*height()/4);


//    QPainterPath ppath2(QPointF(width()/3,height()/4));

//    QPointF c1(width()*2/3,height()/2);
//    QPointF c2(width()/3,height()/2);
//    ppath2.cubicTo(c2,c1,QPointF(2*width()/3,3*height()/4));

//    painter->drawPath(ppath2);

//    painter->restore();
//}

//void TLEEvents::drawEvent4(QPainter *painter)
//{
//    painter->save();

//    painter->setPen(QPen(QColor(220,220,220),4));
//    painter->drawLine(0,2*height()/6,width(),2*height()/6);
//    painter->drawLine(0,3*height()/6,width(),3*height()/6);


//    QRect rec;
//    rec=QRect(1*width()/6,3*height()/6,2*width()/6,2*height()/6);

//    painter->drawArc(rec,0,90*16);
//    rec=QRect(0,2*height()/6,4*width()/6,4*height()/6);
//    painter->drawArc(rec,0,90*16);

//    rec=QRect(3*width()/6,2*height()/6,4*width()/6,4*height()/6);
//    //          painter->setPen(QPen(Qt::red,2));
//    //          painter->drawRect(rec);
//    painter->drawArc(rec,90*16,90*16);

//    rec=QRect(4*width()/6,3*height()/6,2*width()/6,2*height()/6);
//    painter->drawArc(rec,90*16,90*16);

//    painter->drawLine(3*width()/6,4*height()/6,3*width()/6,6*height()/6);
//    painter->drawLine(4*width()/6,4*height()/6,4*width()/6,6*height()/6);

//    QRect r(4*width()/6-7,5*height()/6,16,66);
//    drawCar(painter,r);

//    //    drawSignal(painter,1,0,1,ms);
//    //    drawSignal(painter,1,0,1,ms2);
//    //    drawSignal(painter,1,0,1,ms3);
//    ms->setValue(0,0,r);
//    ms3->setValue(0,0,r);
//    ms2->setValue(0,0,r);
//    painter->restore();
//}

//void TLEEvents::drawEvent5(QPainter *painter)
//{
//    painter->save();

//    painter->setPen(QPen(QColor(220,220,220),4));
//    painter->drawLine(2*width()/6,0,2*width()/6,6*height()/6);
//    painter->drawLine(3*width()/6,0,3*width()/6,6*height()/6);


//    QRect rec;
//    //    rec=QRect(2*width()/6,1*height()/6,2*width()/6,2*height()/6);

//    //    painter->drawArc(rec,0,90*16);
//    //    rec=QRect(0,2*height()/6,4*width()/6,4*height()/6);
//    //    painter->drawArc(rec,0,90*16);

//    rec=QRect(0,0,4*width()/6,4*height()/6);
//    rec.translate(2*width()/6,-height()/6);
//    painter->drawArc(rec,180*16,90*16);

//    rec=QRect(3*width()/6,0,2*width()/6,2*height()/6);
//    painter->drawArc(rec,180*16,90*16);

//    rec=QRect(2*width()/6,2*height()/6,4*width()/6,4*height()/6);
//    painter->drawArc(rec,90*16,90*16);

//    rec=QRect(3*width()/6,3*height()/6,2*width()/6,2*height()/6);
//    painter->drawArc(rec,90*16,90*16);

//    painter->drawLine(4*width()/6,2*height()/6,width(),2*height()/6);
//    painter->drawLine(4*width()/6,3*height()/6,width(),3*height()/6);

//    QRect r(3*width()/6-7,5*height()/6,16,66);
//    drawCar(painter,r);

//    drawSignal(painter,1,1,4,ms);
//    drawSignal(painter,1,7,1,ms2);
//    drawSignal(painter,1,8,1,ms3);

//    painter->restore();
//}

//void TLEEvents::drawEvent6(QPainter *painter)
//{
//    painter->save();

//    painter->setPen(QPen(QColor(220,220,220),4));
//    painter->drawLine(3*width()/6,0,3*width()/6,height());
//    painter->drawLine(4*width()/6,0,4*width()/6,height());


//    QRect rec;
//    rec=QRect(1*width()/6,1*height()/6,2*width()/6,2*height()/6);
//    painter->drawArc(rec,0,-90*16);

//    rec=QRect(0,0,4*width()/6,4*height()/6);
//    painter->drawArc(rec,0,-90*16);

//    rec=QRect(0,3*height()/6,4*width()/6,4*height()/6);
//    painter->drawArc(rec,90*16,-90*16);

//    rec=QRect(1*width()/6,4*height()/6,2*width()/6,2*height()/6);
//    painter->drawArc(rec,90*16,-90*16);

//    painter->drawLine(0,3*height()/6,2*width()/6,3*height()/6);
//    painter->drawLine(0,4*height()/6,2*width()/6,4*height()/6);
//    painter->restore();
//    QRect r(4*width()/6-7,5*height()/6,16,66);
//    drawCar(painter,r);

//    drawSignal(painter,1,1,2,ms);
//    drawSignal(painter,1,3,1,ms2);
//    drawSignal(painter,1,2,1,ms3);
//}


//void TLEEvents::drawEvent2(QPainter *painter)
//{
//    painter->save();

//    painter->setPen(QPen(QColor(220,220,220),4));
//    painter->drawLine(width()/3,0,width()/3,height());
//    painter->drawLine(2*width()/3,0,2*width()/3,height());

//    //    painter->drawLine(width()/3,2*height()/4,2*width()/3,3*height()/4);
//    //    painter->drawLine(2*width()/3,2*height()/4,1*width()/3,3*height()/4);


//    for(int i=0;i<20;i++)
//    {
//        int j=i+1;
//        //        qDebug()<<i<<j;
//        painter->drawLine((i)*width()/20,1*height()/8,(j)*width()/20,1*height()/8);
//        painter->drawLine((i)*width()/20,2*height()/8,(j)*width()/20,2*height()/8);
//        i++;
//    }
//    //    painter->drawLine(0,1*height()/8,width(),1*height()/8);


//    painter->restore();
//}



