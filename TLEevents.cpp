#include "TLEevents.h"
#define MAX 65536
#include <QDebug>
#include <QPainterPath>
TLEEvents::TLEEvents(quint8 size, QWidget *parent): QWidget(parent)
{

    m_size=size;
    grid=27.7;
    xpos = new quint16[50];
    ypos = new quint16[50];
    status = new quint8[50];
    rotation = new quint8[50];
    m_bitmap=new quint8[200];
    display9cubic=false;
    need_paint=false;
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

void TLEEvents::setvalue(quint8 id,quint8 status,
                         quint16 bitmaplength,
                         quint8* bitmap)
{

    sigid =id;
    sigstatus=status;
    m_bitmaplength=bitmaplength;
    for(int i=0;i<m_bitmaplength;i++)
    {
        m_bitmap[i]=bitmap[i];
    }

    entryname=map_sigid_name->value(id);
    //    qDebug()<<"signame"<<name;
    if(!map_sigentry->contains(entryname))
    {
        need_paint=false;
        //        qDebug()<<"paint entry"<<need_paint;
        update();
        return;
    }
    entry= map_sigentry->value(entryname);

    if(!map_ViewID->contains(entry.viewid))
    {
        need_paint=false;
        //        qDebug()<<"paint view"<<need_paint;
        update();
        return;
    }
    view= map_ViewID->value( entry.viewid);

    if(!map_Template->contains(view.templateid))
    {
        need_paint=false;
        //        qDebug()<<"paint tepl"<<need_paint;
        update();
        return;
    }
    tplete= map_Template->value( view.templateid);
    siglist=view.signallist;
    olclist=view.LCSlist;
    need_paint=true;

    //    qDebug()<<"templatid"<<view.templateid;

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



    if(display9cubic)
    {
        painter.setPen(QPen( QColor(220,220,220),1,Qt::DotLine));

        for(int i=0;i<19;i++)
            painter.drawLine(0,i*height()/18,width(),i*height()/18);
        for(int i=0;i<19;i++)
            painter.drawLine(i*width()/18,0,i*width()/18,height());
    }
    else
    {
        painter.setPen(QPen( QColor(220,220,220),1,Qt::NoPen));

        for(int i=0;i<19;i++)
            painter.drawLine(0,i*height()/18,width(),i*height()/18);
        for(int i=0;i<19;i++)
            painter.drawLine(0*width()/18,0,i*width()/18,height());
    }
    if(need_paint)
    {
        painter.save();
        if(entry.rotate>0)
        {
            painter.rotate(entry.rotate);
            painter.translate(-width(),-height());
        }

        if(tplete->size()>0)
        {

            for(int i=0;i<tplete->size();i++)
            {
                Shape sh = tplete->at(i);
                QString name=sh.name;



                if(name== "Line")
                {

                    //                painter.save();
                    painter.setPen(QPen( QColor(sh.list_Attr->value("Color")),
                                         sh.list_Attr->value("LineStyle").toUInt()));

                    painter.drawLine(sh.list_Attr->value("StartX").toUInt()*grid,
                                     sh.list_Attr->value("StartY").toUInt()*grid,
                                     sh.list_Attr->value("EndX").toUInt()*grid,
                                     sh.list_Attr->value("EndY").toUInt()*grid);
                    //                painter.restore();
                }
                else if(name== "Bezier")
                {
                    //                painter.save();
                    painter.setPen(QPen( QColor(sh.list_Attr->value("Color")),
                                         sh.list_Attr->value("LineStyle").toUInt()));

                    QPoint begin(sh.list_Attr->value("StartX").toUInt()*grid,
                                 sh.list_Attr->value("StartY").toUInt()*grid);
                    QPoint end( sh.list_Attr->value("EndX").toUInt()*grid,
                                sh.list_Attr->value("EndY").toUInt()*grid);
                    QPoint c1(sh.list_Attr->value("StartX").toUInt()*grid,
                             (sh.list_Attr->value("StartY").toUInt()+
                              sh.list_Attr->value("EndY").toUInt())*grid/2 );
                    QPoint c2(sh.list_Attr->value("EndX").toUInt()*grid,
                              (sh.list_Attr->value("StartY").toUInt()+
                               sh.list_Attr->value("EndY").toUInt())*grid/2 );
                    QPainterPath pp(begin);
                    pp.cubicTo(c1,c2,end);
                    painter.drawPath(pp);
//                    painter.drawLine(sh.list_Attr->value("StartX").toUInt()*grid,
//                                     sh.list_Attr->value("StartY").toUInt()*grid,
//                                     sh.list_Attr->value("EndX").toUInt()*grid,
//                                     sh.list_Attr->value("EndY").toUInt()*grid);
                    //                painter.restore();
                }
                else if(name== "Rect")
                {
//                    qDebug()<<"Rect";
                    painter.setBrush(QColor(sh.list_Attr->value("BrushColor")));
                    painter.setPen(QPen( QColor(sh.list_Attr->value("PenColor")),
                                         sh.list_Attr->value("LineStyle").toUInt()));
                    painter.drawRect(sh.list_Attr->value("LeftTopX").toUInt()*grid,
                                     sh.list_Attr->value("LeftTopY").toUInt()*grid,
                                     sh.list_Attr->value("RightBottomX").toUInt()*grid,
                                     sh.list_Attr->value("RightBottomY").toUInt()*grid);
                    //                for(int j=0;j<sh.list_Attr->size();j++)
                    //                    qDebug()<<j<<sh.list_Attr->at(j);
                }
                else if(name== "ARC")
                {
                    painter.setPen(QPen( QColor(sh.list_Attr->value("Color")),
                                         sh.list_Attr->value("LineStyle").toUInt()));
                    painter.drawArc(QRect(QPoint(sh.list_Attr->value("StartX").toUInt()*grid,
                                          sh.list_Attr->value("StartY").toUInt()*grid),
                                          QPoint(sh.list_Attr->value("EndX").toUInt()*grid,
                                          sh.list_Attr->value("EndY").toUInt()*grid)),
                                    0,sh.list_Attr->value("Angle").toInt()*16);
//painter.drawRect(0,0,5*grid,5*grid);
//                    painter.drawArc(QRect(QPoint(1*grid,7*grid),QPoint(7*grid,13*grid)),0,180*16);
                }
            }
        }

        if(siglist->size()>0)

        {
            QByteArray bytes((char*)m_bitmap,m_bitmaplength);

            //        for(int i=0;i<m_bitmaplength;i++)
            //        {
            //            qDebug()<<"bytes["<<i<<"]"<<(quint8)bytes.at(i);
            //        }

            for(int i=0;i<siglist->size();i++)
            {
                SignalInfo s=siglist->at(i);
                painter.save();
                if(s.name==entryname)
                    drawSignal(s.x,
                               s.y,
                               s.rotate,sigstatus,
                               &painter);
                else
                {
                    SignalBit b= map_Signal->value(s.name);


                    quint8 status;
                    //                qDebug()<<"signal"<<s.name<< b.left
                    //                       <<b.right
                    //                      <<b.permiss
                    //                     <<b.restric;
                    quint8 left = (m_bitmap[b.left/8]);
                    left&=1<<b.left%8;
                    quint8 right = (m_bitmap[b.right/8]);
                    right&=1<<b.right%8;
                    quint8 permiss = (m_bitmap[b.permiss/8]);
                    permiss&=1<<b.permiss%8;
                    quint8 restric = (m_bitmap[b.restric/8]);
                    restric&=1<<b.restric%8;
                    //                qDebug()<<"left"<<left;

                    //                quint8 red=(m_bitmap[b.restric/8]&=(1<<b.restric%8))>>b.restric%8;
                    //                quint8 green=(m_bitmap[b.permiss/8]&=(1<<b.permiss%8))>>b.permiss%8;
                    //                quint8 left=(m_bitmap[b.left/8]&=(1<<b.left%8))>>b.left%8;
                    //                quint8 right=(m_bitmap[b.right/8]&=(1<<b.right%8))>>b.right%8;
                    //                qDebug()<<"red"<<red<<green<<left<<right;
                    //                qDebug()<<m_bitmap[b.left/8]
                    //                        <<m_bitmap[b.right/8]
                    //                        <<m_bitmap[b.permiss/8]
                    //                        <<m_bitmap[b.restric/8];

                    //                qDebug()<<"bit"<<b.left   <<"="<<(m_bitmap[b.left/8]&=(1<<b.left%8));
                    //                qDebug()<<"bit"<<b.right  <<"="<<(m_bitmap[b.right/8]&=(1<<b.right%8));
                    //                qDebug()<<"bit"<<b.permiss<<"="<<(m_bitmap[b.permiss/8]&=(1<<b.permiss%8));
                    //                qDebug()<<"bit"<<b.restric<<"="<<(m_bitmap[b.restric/8]&=(1<<b.restric%8));
                    if(b.restric!=0&&restric)
                    {
                        status=1;
                    }
                    else if(b.permiss!=0&&permiss)
                    {
                        status=2;
                    }
                    else   if(b.left!=0&&left)
                    {
                        status=3;
                    }
                    else   if(b.right!=0&&right)
                    {
                        status=4;
                    }
                    else
                    {
                        status=1;
                    }

                    drawSignal(s.x,
                               s.y,
                               s.rotate,status,
                               &painter);
                }
                painter.setPen(QPen(QColor(Qt::black),1));
                painter.drawText(QRect(-2*grid-2,0,grid*2,grid),Qt::AlignLeft,s.name);
                //            painter.drawRect(0,0,grid,grid);
                painter.restore();

            }
        }
        painter.restore();
        //    if(olclist->size()>0)
        //    {
        //        for(int i=0;i<olclist->size();i++)
        //        {
        //            SignalInfo s=olclist->at(i);
        //            painter.save();
        //            drawSignal(s.x,
        //                       s.y,
        //                       s.rotate,sigstatus,
        //                       &painter);
        //            painter.restore();

        //        }
        //    }
        QRect rect=QRect(entry.tramx*grid-8,entry.tramy*grid,16,66);
        painter.drawPixmap(rect,QPixmap("res/Tram_Tracklayout.png"));
        painter.setPen(QPen(Qt::black,5));
        painter.drawText(QRect(0,0,10*grid,4*grid),Qt::AlignLeft,view.name);

        //        painter.drawText(rect,Qt::AlignLeft,view.name);
    }
    else
    {

        //        qDebug()<<"need painter paint false";
    }
    painter.end();

}

void TLEEvents:: drawSignal(quint8 x,quint8 y,quint8 rotate,quint8 status,QPainter *painter)
{
    //    qDebug()<<"x"<<x<<y<<grid<<width()/18;
    //    painter->save();
    painter->translate(x*grid,y*grid);
    painter->rotate(rotate);
    painter->setBrush(Qt::black);
    painter->setPen(QPen(Qt::black,2));
    painter->drawEllipse(0-grid/2,0,grid,grid);
    painter->drawLine(0,grid,0,grid+grid/2);
    painter->drawLine(0-grid/2,grid+grid/2,grid/2,grid+grid/2);
    switch(status)
    {
    case 1:
        painter->setPen(QPen(QColor(Qt::red),4));
        painter->drawLine(0-grid/2+3,grid/2,grid/2-2,grid/2);
        break;
    case 2:
        painter->setPen(QPen(QColor(Qt::green),4));
        painter->drawLine(0,0+3,0,grid-3);
        break;
    case 3:
        painter->setPen(QPen(QColor(Qt::yellow),4));
        painter->drawLine(0-grid/4,grid/4,grid/4,3*grid/4);
        break;
    case 4 :
        painter->setPen(QPen(QColor(Qt::yellow),4));
        painter->drawLine(grid/4,grid/4,0-grid/4,3*grid/4);
        break;
    }
    //    painter->restore();
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



void TLEEvents::setMap(QMap<quint8, QString> *mapsigidname, QMap<quint8, QString> *mapolcidname, QMap<QString, SignalBit> *mapsignal, QMap<QString, SignalBit> *mapolc, QMap<quint8, QList<Shape> *> *mapTemplate, QMap<quint8, View> *mapViewID, QMap<QString, Entry> *mapsigentry)
{
    map_sigid_name=mapsigidname;
    map_olcid_name=mapolcidname;
    map_Signal=mapsignal;
    map_OLC=mapolc;
    map_Template=mapTemplate;
    map_ViewID=mapViewID;
    map_sigentry=mapsigentry;
}



