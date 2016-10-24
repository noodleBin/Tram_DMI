/******************************
** Date: 2016/06/23
** Author: noodlebin
** Brief: Main process logic .cpp
** Version: 1.0
******************************/
#include "Casco_dmi.h"
#include <QApplication>
#include <QDir>

int OpenAndSetFile(QString filename,QFile* file,QDomDocument* dom);
QString ConvertSpeed(int src);


Casco_DMI::Casco_DMI()
{
    initialVariable();
    initDefaultValue();
    initialLogSMS();
    initialMapFromXml();



    translateAll();
    initSocket();

    initMainWindow("UI");

    initWorkTTS();






    refreshAlarmQue(100,"[软件] ");

    timer100ms=startTimer(100);
    timerchecktime=startTimer(1*60*1000); //定时1分钟
}

Casco_DMI::~Casco_DMI()
{
    refreshAlarmQue(101,"[软件] ");
    writeAllSMStoFile();
}

void Casco_DMI::translateAll()
{
    QString translatorFile;
    //    languageno=1;
    switch(languageno)
    {
    case 1:
        translatorFile = "app_zh.qm";
        break;
    case 2:
        translatorFile="app_en.qm";
        break;
    default:
        translatorFile="app_zh.qm";
        break;
    }

    translator.load(exePath+"/Translator/"+translatorFile);

    QApplication::installTranslator(&translator);

}

void Casco_DMI::initMainWindow(QString type)
{
    QFile f;
    QUiLoader uiloader;

    if(type=="UI")
    {
        //                screenmode=3;
        switch (screenmode)
        {
        case 1:
            f.setFileName(exePath+"/UI/Display_7_800_480.ui");
            break;
        case 2:
            f.setFileName(exePath+"/UI/Display_10_4_800_600.ui");

            break;
        case 3:
            f.setFileName(exePath+"/UI/Display_10_4_1024_768.ui");

            break;
        default:
            warning_msg="[软件] 无对应UI界面";
            alarmPromte(warning_msg);
            break;
        }

    }
    else if(type=="LoseConnection")
    {
        switch (screenmode)
        {
        case 1:
            f.setFileName(exePath+"/UI/Loseconnection_7_800_480.ui");
            break;
        case 2:
            f.setFileName(exePath+"/UI/Loseconnection_10_4_800_600.ui");
            break;
        case 3:
            f.setFileName(exePath+"/UI/Loseconnection_10_4_1024_768.ui");

            break;
        default:
            warning_msg="[软件] 无对应Loseconnection界面";
            alarmPromte(warning_msg);
            break;
        }

    }
    else if(type=="ShutdownScreen")
    {
        switch (screenmode)
        {
        case 1:
            f.setFileName(exePath+"/UI/ShutdownScreen_7_800_480.ui");
            break;
        case 2:
            f.setFileName(exePath+"/UI/ShutdownScreen_10_4_800_600.ui");
            break;
        case 3:
            f.setFileName(exePath+"/UI/ShutdownScreen_10_4_1024_768 .ui");

            break;
        default:
            warning_msg="[软件] 无对应Shutdown界面";
            alarmPromte(warning_msg);
            break;
        }

    }
    if(wid!=NULL)
    {
        delete wid; //delete parent,all the children will be deleted automatically
    }
    wid = uiloader.load(&f);
    wid->setAttribute(Qt::WA_DeleteOnClose);

    if(displayno==1)
    {

        initialControl();  //only in normal type
        initialNormalControl();

    }
    else if(displayno==2)
    {
        initialDisconnectControl();
    }
    wid->installEventFilter(this);
    f.close();
    QDesktopWidget *desktop=QApplication::desktop();
    int width=desktop->width();
    int height=desktop->height();

    xpos = (width-wid->width())/2;
    ypos =(height-wid->height())/2;



    //    qDebug()<<"xpos"<<xpos<<"ypos"<<ypos;
    wid->move(xpos,ypos);
    wid->setWindowFlags(Qt::FramelessWindowHint);

    if(type=="UI")
    {
        initDialog();
    }

    wid->show();


}

void Casco_DMI::initialDisconnectControl()
{
    wid->findChild<QLabel*>("lbl_disconnect")->setText(tr("disconnect"));
}

void Casco_DMI::initialNormalControl()
{
    wid->findChild<QLabel*>("lbl_namedriverid")->setText(tr("driverid"));
    wid->findChild<QLabel*>("lbl_nameserviceid")->setText(tr("serviceid"));
    wid->findChild<QLabel*>("lbl_namenextstaid")->setText(tr("nextstaid"));
    //    wid->findChild<QLabel*>("lbl_namearrtime")->setText(tr("arrtime"));
    wid->findChild<QLabel*>("lbl_namedelay")->setText(tr("delay"));
    //    wid->findChild<QLabel*>("lbl_delayunit")->setText(tr("delayunit"));

    switch (screenmode)
    {
    case 1:
        wid->findChild<QLabel*>("lbl_namespeed")->setText(tr("speed"));
        wid->findChild<QLabel*>("lbl_namelimitspeed")->setText(tr("limitspeed"));
        wid->findChild<QLabel*>("lbl_namemeter")->setText(tr("km/h"));

        lblNameServiceMode= wid->findChild<QLabel*>("lbl_nameservicemode");
        lblNameServiceMode->setText(tr("servicemode"));
        lblNameServiceMode->installEventFilter(this);

        break;
    default:
        break;
    }


    wid->findChild<QLabel*>("lbl_CPArea")->setText(tr("cparea"));
    wid->findChild<QLabel*>("lbl_CPTotal")->setText(tr("cptotal"));
    wid->findChild<QLabel*>("lbl_CPUnit")->setText(tr("meter"));
    wid->findChild<QLabel*>("lbl_CPRemain")->setText(tr("cpremain"));
    wid->findChild<QLabel*>("lbl_CPRemainUnit")->setText(tr("meter"));

    //Tab mission
    //    if(screenmode==3)
    //    {
    //        //        wid->findChild<QTabWidget*>("tabWidget")->setTabText(0,tr("geoInfo"));
    //        wid->findChild<QTabWidget*>("tabWidget")->removeTab(0);
    //        wid->findChild<QTabWidget*>("tabWidget")->setTabText(1,tr("maintenceInfo"));
    //    }
    //    else
    {
        wid->findChild<QTabWidget*>("tabWidget")->setTabText(0,tr("missionInfo"));
        //        wid->findChild<QTabWidget*>("tabWidget")->setTabText(1,tr("geoInfo"));
        wid->findChild<QTabWidget*>("tabWidget")->removeTab(1);

        wid->findChild<QTabWidget*>("tabWidget")->setTabText(2,tr("maintenceInfo"));
    }
    wid->findChild<QLabel*>("lbl_namesch")->setText(tr("sch"));
    wid->findChild<QLabel*>("lbl_namedes")->setText(tr("des"));
    wid->findChild<QLabel*>("lbl_namedownstream")->setText(tr("downstream"));
    wid->findChild<QLabel*>("lbl_nameupstream")->setText(tr("upstream"));



    //    wid->findChild<QLabel*>("lbl_downdisunit")->setText(tr("meter"));
    //    wid->findChild<QLabel*>("lbl_downminunit")->setText(tr("sec"));
    //    wid->findChild<QLabel*>("lbl_updisunit")->setText(tr("meter"));
    //    wid->findChild<QLabel*>("lbl_upminunit")->setText(tr("sec"));

}

void Casco_DMI::initialControl()
{

    //top area
    lblCascoLogo = wid->findChild<QLabel*>("pic_namelogo");
    lblNameDriver=  wid->findChild<QLabel*>("pic_key");
    lblNameDriverId=wid->findChild<QLabel*>("lbl_namedriverid");
    lblDriverId=wid->findChild<QLabel*>("lbl_driverid");
    lblDriverId->setText(tr("unknown"));
    lblServiceId=wid->findChild<QLabel*>("lbl_serviceid");
    lblServiceId->setText(tr("unknown"));
    lblNextstaId =wid->findChild<QLabel*>("lbl_nextstaid");
    lblNextstaId->setText(tr("unknown"));
    lblNextstaId2 =wid->findChild<QLabel*>("lbl_nextstaid_2");
    lblNextstaId2->setText(tr("unknown"));
    lblArrTime=wid->findChild<QLabel*>("lbl_arrtime");
    lblArrTime->setText(QTime::currentTime().toString("hh:mm:ss"));
    lblArrTime2=wid->findChild<QLabel*>("lbl_arrtime_2");
    lblArrTime2->setText(QTime::currentTime().toString("hh:mm:ss"));
    lblNameDelay=wid->findChild<QLabel*>("lbl_namedelay");
    //    lblDelayunit= wid->findChild<QLabel*>("lbl_delayunit");
    lblDelay=wid->findChild<QLabel*>("lbl_delay");
    lblDelay->setText(tr("unknown"));


    lblDateTime=wid->findChild<QLabel*>("lbl_date");

    QString format = "yyyy"+tr("年")+"MM"+tr("月")+"dd"+tr("日")+"\n"
            +"hh:mm:ss";

    lblDateTime->setText((QDateTime::currentDateTime().toString(format)));

    lblCascoLogo->installEventFilter(this);

    lblNameDriver->installEventFilter(this);
    lblNameDriverId->installEventFilter(this);
    lblDriverId->installEventFilter(this);
    lblDelay ->installEventFilter(this);
    lblDateTime->installEventFilter(this);
    lblNameDelay->installEventFilter(this);
    //    lblDelayunit->installEventFilter(this);



    //atp area
    lblCurrentSpeed=  wid->findChild<QLabel*>("lbl_speed");
    lblAuthoritySpeed =  wid->findChild<QLabel*>("lbl_limitspeed");
    if(screenmode==2||screenmode==3)
    {
        widDashboard=wid->findChild<QWidget*>("widget_dashboard");
        //        if(mydashboard!=NULL)
        //        {
        //            delete mydashboard;
        //        }
        mydashboard=new MyDashboard(screenmode,widDashboard);
    }
    lblAtpProtection =  wid->findChild<QLabel*>("pic_atpprotection");
    lblAtpInhibited   =  wid->findChild<QLabel*>("pic_Atpinhibi");
    lblAtpWarning   =  wid->findChild<QLabel*>("pic_atpwarning");
    lblServiceMode =  wid->findChild<QLabel*>("lbl_servicemode");

    lblServiceMode->installEventFilter(this);

    //mission area



    lblObsStatus=wid->findChild<QLabel*>("pic_obsstatus");
    lblELSDMSStatus=wid->findChild<QLabel*>("pic_dmsels");
    lblLocStatus=wid->findChild<QLabel*>("pic_traloc");
    lblPassengerType=wid->findChild<QLabel*>("pic_passertype");
    lblRunningType=wid->findChild<QLabel*>("pic_runningtype");
    lblSystemOK=wid->findChild<QLabel*>("pic_pantostatus");

    lblShutdownStatus=wid->findChild<QLabel*>("pic_working");
    lblActiveEnd=wid->findChild<QLabel*>("pic_activeend");
    lblEb=wid->findChild<QLabel*>("pic_eb");
    lblSb=wid->findChild<QLabel*>("pic_sb");
    lblTurnback=wid->findChild<QLabel*>("pic_turnback");
    lblSkip=wid->findChild<QLabel*>("pic_skip");
    lblHold=wid->findChild<QLabel*>("pic_hold");

    lblObsStatus->installEventFilter(this);
    lblELSDMSStatus->installEventFilter(this);

    //RRCP area
    RRCPbar = wid->findChild<QProgressBar*>("probar_CP");
    lblBarName=wid->findChild<QLabel*>("lbl_CPArea");
    lblBarTotalValue   =wid->findChild<QLabel*>("lbl_RRCPTotalValue");
    lblBarRemainValue=wid->findChild<QLabel*>("lbl_RRCPRemainValue");


    //Operate area
    btnleft=wid->findChild<QLabel*>("btn_left");
    btnstra= wid->findChild<QLabel*>("btn_stra");
    btnright= wid->findChild<QLabel*>("btn_right");
    frameRR =wid->findChild<QFrame*>("frame_RR");
    btncp=wid->findChild<QLabel*>("btn_cp");
    btninput=wid->findChild<QLabel*>("btn_input");

    btninput->installEventFilter(this);
    btnleft->installEventFilter(this);
    btnstra->installEventFilter(this);
    btnright->installEventFilter(this);
    btncp->installEventFilter(this);

    //tab Mission
    lblNameDepTime=  wid->findChild<QLabel*>("lbl_namedeptime");
    //    lblNameArrTime= wid->findChild<QLabel*>("lbl_namearrtime");
    lblNameCurSta= wid->findChild<QLabel*>("lbl_namecursta");

    lblNamesch=wid->findChild<QLabel*>("lbl_namesch");
    lblNamedes=wid->findChild<QLabel*>("lbl_namedes");
    lblSchId=wid->findChild<QLabel*>("lbl_schid");
    lblDesId=wid->findChild<QLabel*>("lbl_desid");
    lblDownstreamDis=wid->findChild<QLabel*>("lbl_downstreamdis");
    lblDownstreamTime=wid->findChild<QLabel*>("lbl_downstreamtime");
    lblUpstreamDis=wid->findChild<QLabel*>("lbl_upstreamdis");
    lblUpstreamTime=wid->findChild<QLabel*>("lbl_upstreamtime");
    lblCurStaId=wid->findChild<QLabel*>("lbl_curstaid");
    lblDepTime=wid->findChild<QLabel*>("lbl_deptime");
    btnMute=wid->findChild<QLabel*>("btn_mute");
    btnSlider=wid->findChild<QSlider*>("volumeslider");

    lblNamesch->installEventFilter(this);
    lblNamedes->installEventFilter(this);
    lblSchId ->installEventFilter(this);
    lblDesId->installEventFilter(this);
    btnMute->installEventFilter(this);
    refreshMute();

    //tab geo
    widGeo=wid->findChild<QWidget*>("widget_geo");
    widGeo->setVisible(false);

    widDMS=wid->findChild<QWidget*>("widget_DMS");


    mygeoevents = new MyGeoEvents(screenmode,widGeo);
    mytleevents=new TLEEvents(screenmode,widDMS);
    widDMS->setHidden(true);
    //tab maintence
    listAlarm=wid->findChild<QTextEdit*>("tblwdg_fault");

    //sms
    listSMS=wid->findChild<QTextEdit*>("listSMS");
    //    listSMS->setText("jj");
}

void Casco_DMI::initDialog()
{
    dialversion = new DialogVersion(player,wid);
    dialversion->setGeometry(xpos+(wid->width()-dialversion->width())/2,
                             ypos+(wid->height()-dialversion->height())/2
                             ,dialversion->width(),dialversion->height());
    diallogin=new DialogLogin(wid);
    diallogin->setGeometry(xpos+(wid->width()-diallogin->width())/2,
                           ypos+(wid->height()-diallogin->height())/2
                           ,diallogin->width(),diallogin->height());
    diallogin->setInfo(driver_Password);
    dialtimeshift = new Dialogtimeshift(wid);
    dialtimeshift->setGeometry(xpos+(wid->width()-dialtimeshift->width())/2,
                               ypos+(wid->height()-dialtimeshift->height())/2
                               ,dialtimeshift->width(),dialtimeshift->height());

    msbox=new DialogWarningbox(wid);
    msbox->setFont(QFont("system",16));

    dialtimeupdate = new DialogTimeUpdate(wid);
    dialtimeupdate->setGeometry(xpos+(wid->width()-dialtimeupdate->width())/2,
                                ypos+(wid->height()-dialtimeupdate->height())/2
                                ,dialtimeupdate->width(),dialtimeupdate->height());
    dialobs = new DialogOBSStatus(wid);
    dialobs->setGeometry(xpos,ypos+wid->height()/2,dialobs->width(),dialobs->height());
    dialcommstatus = new DialogCommstatus(wid);
    dialcommstatus->setGeometry(xpos,ypos+wid->height()/2,dialcommstatus->width(),dialcommstatus->height());
    dialdes = new DialogDestination(wid);
    dialdes->setGeometry(xpos+(wid->width()-dialdes->width())/2,
                         ypos+(wid->height()-dialdes->height())/2
                         ,dialdes->width(),dialdes->height());
    dialdes->intialmap(stationIdMap);

    dialelsmode = new DialogELSModeSelection(wid);
    dialelsmode->setGeometry(xpos+(wid->width()-dialelsmode->width())/2,
                             ypos+(wid->height()-dialelsmode->height())/2
                             ,dialelsmode->width(),dialelsmode->height());
    dialschedule = new DialogSchedule(scheduleIdMap,wid);
    dialschedule->setGeometry(xpos+(wid->width()-dialschedule->width())/2,
                              ypos+(wid->height()-dialschedule->height())/2
                              ,dialschedule->width(),dialschedule->height());

    dialsms = new DialogSMSToDriver(log,sms,wid);
    dialsms->setGeometry(xpos+(wid->width()-dialsms->width())/2,
                         ypos+(wid->height()-dialsms->height())/2
                         ,dialsms->width(),dialsms->height());


    connect(dialversion,&DialogVersion::signalRefreshMute,this,&Casco_DMI::refreshMute);
    connect(dialschedule,&DialogSchedule::sendScheduleId,this,&Casco_DMI::setScheduleId);
    connect(dialschedule,&DialogSchedule::sendServiceId,this,&Casco_DMI::setServiceId);
    connect(dialschedule,&DialogSchedule::sendTripId,this,&Casco_DMI::setTripId);
}

void Casco_DMI::initWorkTTS()
{
    ttsthreadisstart=false;

    workerthread = new QThread;
    myttsworker=new TTSWorker;

    myttsworker->moveToThread(workerthread);

    connect(player,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus )),this,SLOT(playEnd(QMediaPlayer::MediaStatus)));
    connect(workerthread, &QThread::finished, myttsworker, &QObject::deleteLater);
    connect(this, &Casco_DMI::ttsConvertNow, myttsworker, &TTSWorker::TTSConvert);
    connect(myttsworker, &TTSWorker::resultReady, this, &Casco_DMI::playMediaNow);
    //    connect(btnSlider,SIGNAL(sliderMoved(int)),this,SLOT(on_volumeslider_sliderMoved(int)));


    workerthread->start();


}


void Casco_DMI::getAlarmOutQue()
{
    Alarm_Record alarm;

    if(!m_alarm_que->isEmpty())
    {
        quint8 playstatus=player->mediaStatus();
        if(playstatus==QMediaPlayer::EndOfMedia
                ||playstatus==QMediaPlayer::NoMedia)
        {
            alarm=m_alarm_que->dequeue();
            ttsthreadisstart=true;
            m_current_alarm=alarm;

            Alarm_Record modify_alarm = m_Alarm_Record_map->value(alarm.id);
            modify_alarm.couldplay=false;
            m_Alarm_Record_map->remove(alarm.id);
            m_Alarm_Record_map->insert(modify_alarm.id,modify_alarm);

            //            qDebug()<<"in end media"<<alarm.str<<alarm.couldplay<<alarm.current_playtick;
            emit ttsConvertNow(alarm.str,m_alarm_filename);
        }
        else if(!ttsthreadisstart) //不能打断tts，需要tts完成后才能再次tts
        {
            alarm=  m_alarm_que->first();
            if(alarm.priority<m_current_alarm.priority)
            {
                Alarm_Record modify_alarm;
                if(m_Alarm_Record_map->value(alarm.id).couldplay)
                {
                    alarm=m_alarm_que->dequeue();



                    modify_alarm= m_Alarm_Record_map->value(m_current_alarm.id);
                    modify_alarm.couldplay=true;
                    m_Alarm_Record_map->remove(m_current_alarm.id);
                    m_Alarm_Record_map->insert(modify_alarm.id,modify_alarm);
                    putAlarmInQue(m_current_alarm); //放回去
                    //                      qDebug()<<"now put current in que is"<<m_current_alarm.str;
                    //                      for(int i=0;i<m_alarm_que->size();i++)
                    //                      {
                    //                          qDebug()<<"alarm que "<<i<<m_alarm_que->at(i).str;
                    //                      }

                    m_current_alarm=alarm;  //打断

                    modify_alarm = m_Alarm_Record_map->value(m_current_alarm.id);
                    modify_alarm.couldplay=false;
                    m_Alarm_Record_map->remove(m_current_alarm.id);
                    m_Alarm_Record_map->insert(modify_alarm.id,modify_alarm);

                    //                    qDebug()<<"in playing media"<<alarm.str<<alarm.couldplay<<alarm.current_playtick;
                    emit ttsConvertNow(alarm.str,m_alarm_filename);
                }
                else
                {
                    ;
                }
            }
        }
    }

}

void Casco_DMI::playEnd(QMediaPlayer::MediaStatus status)
{
    switch(status)
    {
    case QMediaPlayer::EndOfMedia:
        //        ttsthreadisstart=false;
        break;
    default:
        break;
    }

}

void Casco_DMI::playMediaNow()
{
    ttsthreadisstart=false;
    player->stop();
    player->setMedia(QUrl::fromLocalFile(QDir::currentPath()+ "/"+"tmp"));
    player->play();

}

//void Casco_DMI::on_volumeslider_sliderMoved(int position)
//{
//    player->setVolume(position);
//}

void Casco_DMI::recvMySMS(QString promotetext)
{
    if(displayno==1)
    {
        smsPromte("[软件] "+promotetext);
    } //when ==3,there is no listsms,so will crash
}
void Casco_DMI::initialVariable()
{
    que_els_dmi = new QQueue<QByteArray>();
    que_dms_dmi=new QQueue<QByteArray>();
    m_alarm_que = new QQueue<Alarm_Record>();

    list_sms  = new QQueue<QString>();
    list_alarm  = new QQueue<QString>();

    stationIdMap = new QMap<int,QString>;
    scheduleIdMap = new QMap<int,QString>;
    SSAIdMap=new QMap<int,QString>;
    DMI_Description=new QMap<int,Description>;
    map_CBI_Signal=new QMap<quint8,DMS_Signal_Info>;
    map_OLC_Signal=new QMap<quint8,DMS_Signal_Info>;
    map_DMI_CBI_Bitmap=new QMap<quint8,DMI_CBI_Info>;
    driver_Password=new QMap<quint16,quint16>;
    m_Alarm_Record_map=new QMap<quint8,Alarm_Record>;
    m_alarm_filename="tmp";

    m_lastsms="";

    timerdisconnect=0;
    exePath=QDir::currentPath();
    resPath = exePath+"/res/";

    timer1s=0;
    smsCount=0;

    timerflash=0;
    needspeedflash=false;
    needatpwarning=false;

    isdialobspop=false;
    isdialcommstatuspop=false;

    needleftflash=false;
    needstraflash=false;
    needrightflash=false;
    isatpwarningflash=false;
    isspeedflash=false;
    isleftflash=false;
    isrightflash=false;
    isstraflash=false;
    displayno=1;
    isDebug=false; //not debug mode
    stepOneMsg=false;

    els_dmi_data=new ELS_DMI_Protocol();
    dms_dmi_data=new DMS_DMI_Protocol();

    dmi_els_data = new DMI_ELS_Protocol();
    dmi_els_data->Message_Format_Version=1;

    movieshutdown=new QMovie(resPath+"ELS_RR_CP_Shutdown1.gif");




    player = new QMediaPlayer();


    wid=NULL;
    mydashboard=NULL;
    mygeoevents=NULL;


    operateType="";

    one_minute_increase=0;

    smsindex=0;

    timeforhidepnl=0;

    isLogin=false;
    isquit=false;
    isDiscon=false;
    time_checked=false;

    timediff= QDateTime(QDate(1970,1,1)).secsTo(QDateTime(QDate(2000,1,1)));

}

void Casco_DMI::initDefaultValue()
{
    screenmode=1;
    languageno=1;
    remoteip="127.0.0.1";
    remoteip2="127.0.0.1";
    remoteport=12345;
    remoteport2=12346;
    port=12345;
    port2=12346;

    m_diffflash=5;
    m_discontime = 5;
    m_appversion="0.0.1";
    m_dataversion="0.0.1";
    m_smsdisplaycount=5;

    m_promotesizeMB=500;
    m_clearsizeMB =500;
    m_writecount =1000;

    m_checktime=15;
    m_sendTime =255;
    m_cp_sendtime=m_elsmode_sendtime=m_driverid_sendtime=m_timeshift_sendtime
            =m_scheduleid_sendtime=m_serviceid_sendtime
            =m_tripid_sendtime=m_pathid_sendtime
            =m_desid_sendtime=m_sendTime;
    is_cp_send=is_elsmode_send=is_driverid_send=is_timeshift_send=
            is_scheduleid_send=is_serviceid_send
            =is_tripid_send=is_pathid_send
            =is_desid_send=false;
}
/**************/
/* 时间转换函数*/
/* 输入：64位utc格式的时间，使用ntp协议，从2000-1-1开始的秒数*/
/* 输出：64位的utc秒数*/
/*************/
quint64 Casco_DMI::timeConvertDisplay(quint64 ntptime)
{

    quint32 utctimefrom2000_1_1=  ntptime>>32;
    time_t utctimefrom1970_1_1;
    if(utctimefrom2000_1_1==0) //如果为0，返回当前时间
    {
        alarmPromte("[车载] ntp时间错误");
        time(&utctimefrom1970_1_1);
        //        tmpdatetime.setTime_t(tmptime);
    }
    else
    {
        utctimefrom1970_1_1=(time_t)(utctimefrom2000_1_1+timediff);
        currentSecondsFromELS=utctimefrom1970_1_1;
    }

    return utctimefrom1970_1_1;
}

bool Casco_DMI::eventFilter(QObject *obj, QEvent *event)
{
    if(obj==lblCascoLogo)
    {
        if(event->type()==QEvent::MouseButtonRelease)
        {
            if(isLogin)
            {
                dialversion->setValue(m_appversion,m_dataversion);
                dialversion->exec();
            }
            else
            {
                msbox->setValue("请登陆后操作");
                msbox->exec();
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    else if((obj==lblDriverId)|(obj==lblNameDriver)|(obj==lblNameDriverId))
    {
        if((event->type()==QEvent::MouseButtonRelease))
        {
            quint16 tripid=els_dmi_data->Trip_Id;
            quint16 pathid=els_dmi_data->Path_Id;
            quint16 lineid=els_dmi_data->Line_Id;
            quint16 physicaltrainid=els_dmi_data->Physical_Train_Id;

            //             qDebug()<<"logic"<<QString::number(logictrainid);

            if(tripid>32767)
            {
                tripid=0;
                warning_msg="[ELS] tripid>32767";
                alarmPromte(warning_msg);
            }

            if(pathid>32767)
            {
                pathid=0;
                warning_msg="[ELS] pathid>32767";
                alarmPromte(warning_msg);
            }
            //            diallogin=new DialogLogin(wid);
            //            diallogin->setGeometry(xpos,ypos,diallogin->width(),diallogin->height());
            diallogin->setTripAndPath(tripid,pathid,lineid,physicaltrainid);
            //        diallogin->move(xpos,ypos);
            if(diallogin->exec()==QDialog::Accepted)
            {
                qint16 i=  diallogin->driverID.toInt();

                dmi_els_data->Driver_Id=i;
                is_driverid_send=true;
                if(i!=0)
                    isLogin=true;
                else
                    isLogin=false;

            }
            operateType="modify DriverID "+QString::number(dmi_els_data->Driver_Id);
            //            delete diallogin;
            return true;
        }
        else
        {
            return false;
        }
    }
    else if((obj==lblDelay)||(obj==lblNameDelay))
    {
        if((event->type()==QEvent::MouseButtonRelease))
        {
            if(isLogin)
            {
                operateType="TimeShift";
                //            dialtimeshift = new Dialogtimeshift(wid);
                //            dialtimeshift->setGeometry(xpos,ypos,dialtimeshift->width(),dialtimeshift->height());
                dialtimeshift->setvalue(els_dmi_data->Service_Id,
                                        stationIdMap->value(els_dmi_data->Destination_Id),els_dmi_data->Advance_Delay_Time);
                //        dialtimeshift->move(xpos,ypos);
                if(dialtimeshift->exec()==QDialog::Accepted)
                {
                    dmi_els_data->Time_Shift_Request=1;
                    is_timeshift_send=true;
                }
                else
                {
                    dmi_els_data->Time_Shift_Request=0;
                }
            }
            else
            {
                msbox->setValue("请登陆后操作");
                msbox->exec();

            }
            return true;
        }
        else
        {
            return false;
        }
    }
    else if(obj==lblDateTime)
    {
        if(event->type()==QEvent::MouseButtonRelease)
        {
            if(isLogin)
            {
                operateType="TimeUpdate";
                //            dialtimeupdate = new DialogTimeUpdate(wid);
                //            dialtimeupdate->setGeometry(xpos,ypos,dialtimeupdate->width(),dialtimeupdate->height());

                dialtimeupdate->exec();
            }
            else
            {
                //                qDebug()<<"timeupdate";
                msbox->setValue("请登陆后操作");
                msbox->exec();
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    else if(obj==lblObsStatus)
    {
        if(event->type()==QEvent::MouseButtonRelease)
        {
            //            dialobs = new DialogOBSStatus(wid);
            //            dialobs->setGeometry(xpos,ypos+wid->height()/2,dialobs->width(),dialobs->height());
            isdialobspop=true;
            dialobs->exec();

            isdialobspop=false;

            return true;
        }
        else
        {
            return false;
        }
    }
    else if(obj==lblELSDMSStatus)
    {
        if(event->type()==QEvent::MouseButtonRelease)
        {
            dialcommstatus = new DialogCommstatus(wid);
            dialcommstatus->setGeometry(xpos,ypos+wid->height()/2,dialcommstatus->width(),dialcommstatus->height());
            isdialcommstatuspop=true;
            dialcommstatus->exec();
            isdialcommstatuspop=false;

            return true;
        }
        else
        {
            return false;
        }
    }
    else if((obj==lblDesId)|(obj==lblNamedes))
    {
        if((event->type()==QEvent::MouseButtonRelease))
        {
            if(isLogin)
            {
                quint8 elsmode=els_dmi_data->ELS_Service_Mode;
                if(elsmode==0||elsmode==1)
                {
                    msbox->setValue("请在车载独立或者手工控制模式下操作");
                    msbox->exec();
                    return true;
                }
                quint8 tdes=els_dmi_data->Number_of_Destinations;
                quint8 tpath = els_dmi_data->Number_of_Path;
                if(tdes<=50)
                {
                    if (tdes>0)
                    {
                        quint16* desid=new quint16[tdes];

                        for(int i=0;i<tdes;i++)
                        {
                            desid[i]=els_dmi_data->Id_of_Destinations[i];
                            //                            qDebug()<<"recv desid"<<hex<<desid[i];

                        }

                        dialdes->setDestinationValue(tdes,desid);
                        delete[] desid;
                    }
                    else
                        dialdes->setDestinationValue(0,NULL);
                }
                else
                {
                    warning_msg="[ELS] recv Number_of_Destinations error";
                    alarmPromte(warning_msg);
                }

                if(tpath<=50)
                {
                    if (tpath>0)
                    {
                        quint16* pathid=new quint16[tpath];

                        for(int i=0;i<tpath;i++)
                        {
                            pathid[i]=els_dmi_data->Id_of_Paths[i];

                        }
                        dialdes->setPathValue(tpath,pathid);
                        delete[] pathid;
                    }
                    else
                        dialdes->setPathValue(0,NULL);
                }
                else
                {
                    warning_msg="[ELS] recv Number_of_paths error";
                    alarmPromte(warning_msg);
                }


                if(dialdes->exec()==QDialog::Accepted)
                {
                    dmi_els_data->Schedule_Id=-1;
                    dmi_els_data->Service_Id=-1;
                    dmi_els_data->Trip_Id=-1;

                    quint8 desorpath=dialdes->m_desorpath;
                    qint8 chosen_value=dialdes->chosen_value;
                    if(desorpath==1)
                    {
                        dmi_els_data->Destination_Id=chosen_value;
                        dmi_els_data->Path_Id=-1;
                        is_desid_send=true;

                    }
                    else if(desorpath==2)
                    {
                        dmi_els_data->Path_Id=chosen_value;
                        is_pathid_send=true;
                        dmi_els_data->Destination_Id=-1;
                    }

                }

            }
            else
            {
                msbox->setValue("请登陆后操作");
                msbox->exec();
            }
            return true;
        }
        else
        {
            return false;
        }

    }
    else if((obj==lblServiceMode)|(obj==lblNameServiceMode))
    {
        if((event->type()==QEvent::MouseButtonRelease))
        {
            if(isLogin)
            {
                //                quint8 elsmode=els_dmi_data->ELS_Service_Mode;
                //                if(elsmode==0||elsmode==1)
                //                {
                //                    msbox->setValue("请在车载独立或者手工控制模式下操作");
                //                    msbox->exec();
                //                    return true;
                //                }
                operateType="modify ServiceMode";
                if(dialelsmode->exec()==QDialog::Accepted)
                {
                    quint8 i= dialelsmode->modevalue;
                    dmi_els_data->ELS_Mode_Selection=i;
                    is_elsmode_send=true;
                    if(isDebug)
                    {
                        alarmPromte("ELS mode 选择为 "+QString::number(i));
                    }
                }
            }
            else
            {
                msbox->setValue("请登陆后操作");
                msbox->exec();
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    else if((obj==lblSchId)|(obj==lblNamesch))
    {
        if((event->type()==QEvent::MouseButtonRelease))
        {
            if(isLogin)
            {
                quint8 elsmode=els_dmi_data->ELS_Service_Mode;
                if(elsmode==0||elsmode==1)
                {
                    msbox->setValue("请在车载独立或者手工控制模式下操作");
                    msbox->exec();
                    return true;
                }
                operateType="modify ScheduleID";
                quint8 tsche=els_dmi_data->Number_of_Schedule;
                quint8 tservice=els_dmi_data->Number_of_Service;
                quint8 ttrip=els_dmi_data->Number_of_Trip;

                if(tsche<=20)
                {
                    //normal;
                }
                else
                {
                    tsche=20; //only demostrate 5 items;
                    warning_msg="[ELS] recv Number_of_schedule error";
                    alarmPromte(warning_msg);
                }

                //                if (tsche>0)
                //                {
                quint16* scheid=new quint16[tsche];

                for(int i=0;i<tsche;i++)
                {
                    scheid[i]=els_dmi_data->Id_of_schedules[i];

                }
                quint16* serviceid=new quint16[tservice];

                for(int i=0;i<tservice;i++)
                {
                    serviceid[i]=els_dmi_data->Id_of_services[i];

                }
                quint16* tripid=new quint16[ttrip];

                for(int i=0;i<ttrip;i++)
                {
                    tripid[i]=els_dmi_data->Id_of_Trips[i];

                }

                dialschedule->setvalue(tsche,scheid,
                                       tservice,serviceid,
                                       ttrip,tripid);
                delete[] scheid;
                delete[] serviceid;
                delete[] tripid;
                //                }
                //                else
                //                 dialschedule->setvalue(0,NULL);   //需要清除界面

                //        dialschedule->move(xpos,ypos);
                dialschedule->exec();

            }
            else
            {
                msbox->setValue("请登陆后操作");
                msbox->exec();
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    else if(obj==btninput)
    {
        if(event->type()==QEvent::MouseButtonRelease)
        {
            if(isLogin)
            {
                //                QStringList smsfiles=sms->findSMSInDirec();
                //                dialsms->setValue(smsfiles);
                dialsms->exec();
            }
            else
            {
                msbox->setValue("请登陆后操作");
                msbox->exec();
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    //media play
    else if(obj==btnMute)
    {

        if(event->type()==QEvent::MouseButtonRelease)
        {
            if(isLogin)
            {
                ToggleMute();

            }
            else
            {
                msbox->setValue("请登陆后操作");
                msbox->exec();
            }
            return true;
        }
        else
            return false;


    }
    //RRCP
    else if(obj==btnleft)
    {
        if(event->type()==QEvent::MouseButtonRelease)
        {
            if(isLogin)
            {
                operateType="RR Left";
//                dmi_els_data->RR_Left_Button=1;

            }
            else
            {
                msbox->setValue("请登陆后操作");
                msbox->exec();
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    else if(obj==btnstra)
    {
        if(event->type()==QEvent::MouseButtonRelease)
        {
            if(isLogin)
            {
                operateType="RR Straight";
//                dmi_els_data->RR_Straight_Button=1;
            }
            else
            {
                msbox->setValue("请登陆后操作");
                msbox->exec();
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    else if(obj==btnright)
    {
        if(event->type()==QEvent::MouseButtonRelease)
        {
            if(isLogin)
            {
                operateType="RR Right";
//                dmi_els_data->RR_Right_Button=1;
            }
            else
            {
                msbox->setValue("请登陆后操作");
                msbox->exec();
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    else if(obj==btncp)
    {
        if(event->type()==QEvent::MouseButtonRelease)
        {
            if(isLogin)
            {
                operateType="CP Request";
                dmi_els_data->Crossing_Priority_Request=1;
                is_cp_send=true;
            }
            else
            {
                msbox->setValue("请登陆后操作");
                msbox->exec();
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    else if( event->type()== QKeyEvent::KeyPress)
    {
        //        qDebug()<<"it is quit";
        QKeyEvent* key =(QKeyEvent*)(event);
        if(key->key()==Qt::Key_1&&key->modifiers()==Qt::CTRL)
        {
            QApplication::exit();
        }
        if(key->key()==Qt::Key_2&&key->modifiers()==Qt::CTRL)
        {
            lblShutdownStatus->setMovie(movieshutdown);
            movieshutdown->start();
        }

        if(key->key()==Qt::Key_3&&key->modifiers()==Qt::CTRL)
        {
            isDebug=!isDebug;  //按一次ctrl+k，切换一次运行模式 调试/正常
            return true;
        }
        if(key->key()==Qt::Key_4&&key->modifiers()==Qt::CTRL)
        {
            stepOneMsg=true;
            return true;
        }
        if(key->key()==Qt::Key_5&&key->modifiers()==Qt::CTRL)
        {
            ToggleMute();
            return true;
        }
        if(key->key()==Qt::Key_6&&key->modifiers()==Qt::CTRL)
        {
            isLogin=!isLogin;
            return true;
        }
        if(key->key()==Qt::Key_7&&key->modifiers()==Qt::CTRL)
        {
            isDiscon=!isDiscon;
            return true;
        }
        if(key->key()==Qt::Key_8&&key->modifiers()==Qt::CTRL)
        {
            smsPromte(QString::number(smsCount));
            return true;
        }
        if(key->key()==Qt::Key_9&&key->modifiers()==Qt::CTRL)
        {
            if(widDMS->isHidden())
            {
                widDMS->setHidden(false);
            }
            else
            {
                widDMS->setHidden(true);
            }
            return true;
        }
        return false;
    }
    return  QObject::eventFilter(obj,event);
}


int Casco_DMI::initSocket()
{
    //    port =12334;
    mySocket = new QUdpSocket(this);
    mySocket2=new QUdpSocket(this);
    if(!mySocket->bind(port,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint)) //需要读取配置文件获得port之后才能bind
        return 0;
    if(!mySocket2->bind(port2,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint)) //需要读取配置文件获得port之后才能bind
        return 0;

    connect(mySocket,SIGNAL(readyRead()),this,SLOT(recvMsg()));
    connect(mySocket2,SIGNAL(readyRead()),this,SLOT(recvMsg()));
    return 1;
}

void Casco_DMI::recvMsg()
{
    //    qDebug()<<"recv";
    //    qDebug()<<"obj"<<this->sender();
    while(mySocket->hasPendingDatagrams())
    {

        QByteArray datagram;
        datagram.resize(mySocket->pendingDatagramSize());
        mySocket->readDatagram(datagram.data(),datagram.size());
        que_els_dmi->enqueue(datagram);

    }

    while(mySocket2->hasPendingDatagrams())
    {

        QByteArray datagram;
        datagram.resize(mySocket2->pendingDatagramSize());
        mySocket2->readDatagram(datagram.data(),datagram.size());
        que_dms_dmi->enqueue(datagram);

    }

}
void Casco_DMI::timerEvent(QTimerEvent *e)
{
    if(e->timerId()==timerchecktime)
    {
        one_minute_increase++;
        if(one_minute_increase>=m_checktime)
        {
            time_checked=false;
            one_minute_increase=0;
        }
    }
    if(timer1s>=10)
    {
        timer1s=0;
        if(displayno==1)
        {
            lblDateTime->setText(QDateTime::currentDateTime().toString("yyyy年MM月dd日\nhh:mm:ss"));
        }
        getAlarmOutQue();
        refreshAlarmMap();
    }
    if(e->timerId()==timer100ms) //100ms只收一条，只发一条
    {
        timer1s++;
        if(isquit)
        {
            QApplication::exit();
        }
        //如果没收到消息不用刷新
        if(!que_els_dmi->isEmpty()) // if no new txt,then do nothing, it can save performance
        {

            timerdisconnect=0;

            while(!que_els_dmi->isEmpty())
            {
                last_els_dmi_data=que_els_dmi->dequeue();

            }

            setRecvDataFromSocket(last_els_dmi_data); //只用最新的
            //            if(m_isdebug)
            //            {
            //                QString recvmsg="now recv the els data is "+QString::number(last_els_dmi_data.size())+" ";
            //                char *c = last_els_dmi_data.data();
            //                for(int i=0;i<last_els_dmi_data.size();i++)
            //                {
            //                    recvmsg=recvmsg+"0x"+QString::number( (quint8)(c[i]),16)+" ";
            //                }
            //                recvmsg+='\n';
            //                log->writeLog(recvmsg);
            //            }
            if(els_dmi_data->DMI_Control==1)
            {
                if(displayno!=1)
                {
                    displayno=1;
                    wid->hide();
                    wid->close();
                    //                    qDebug()<<"widget close 1"<<tmp;
                    initMainWindow("UI");
                    refreshAlarmQue(8,"[车载] ");
                }
                //                qDebug()<<"before refresh UI"<<els_dmi_data->ELS_Service_Mode;
                refreshUI();

            }
            else
            {
                if(displayno!=3)
                {
                    displayno=3;
                    wid->hide();
                    wid->close();
                    //                    qDebug()<<"widget close 3"<<tmp;
                    initMainWindow("ShutdownScreen");
                }
            }

        }
        else
        {
            if(timerdisconnect<=(m_discontime*10))
            {
                timerdisconnect++;
            }
            else
            {
                if(displayno!=2&&!isDiscon)
                {
                    displayno=2;
                    wid->hide();
                    wid->close();
                    refreshAlarmQue(7,"[车载] ");
                    initMainWindow("LoseConnection");

                    //                    timer15s=0;

                }
            }

        }

        //     if(displayno==1) //输入不需要队列，司机可以随意修改，反正时间到了就发一条
        //   {
        if(!isDebug)
        {
            //            qDebug()<<"send qq";
            //            QApplication::exit();
            sendMsgToELS();
            //            dmi_els_data->clearAll();//清空当前发送状态
            //            operateType="";
        }
        else
        {
            if(stepOneMsg)
            {
                sendMsgToELS();
                //                dmi_els_data->clearAll();
                //            operateType="";
                stepOneMsg=false;
            }//如果不发送的话，是不会清空数据内容的，后面的操作会覆盖之前的操作
        }
        //    }



        writeSMSToFile();
        writeAlarmToFile();
        if(!que_dms_dmi->isEmpty())
        {
            //            qDebug()<<"que_dms_dmi count is"<<que_dms_dmi->count();

            while(!que_dms_dmi->isEmpty())
            {
                last_dms_dmi_data=que_dms_dmi->dequeue();

            }
            //            qDebug()<<"recv dms msg";
            setRecvDMSDataFromSocket(last_dms_dmi_data); //只用最新的

            refreshTLE();
        }
        else
        {

        }
    }

}




void Casco_DMI::setRecvDataFromSocket(QByteArray& bytes)//因为消息是变长的，因此无法用指针强转
{
    els_dmi_data->freePointer();
    els_dmi_data->getDataFromBytes(bytes);



}

void Casco_DMI::setRecvDMSDataFromSocket(QByteArray &bytes)
{
    //        qDebug()<<"it's 2";

    dms_dmi_data->getDataFromBytes(bytes);

    //    qDebug()<<"dms_dmi_data"<<dms_dmi_data->Signal_ID<<dms_dmi_data->Signal_Status;

    if(dms_dmi_data->Msg_ID==102)
    {
        qDebug()<<"now return DMS"<<dms_dmi_data->Signal_ID<<
                  dms_dmi_data->RTU_ID<<dms_dmi_data->RTU_Type;
        dms_dmi_data->Msg_ID=103;
        QByteArray b;

        quint16 length=sizeof(DMS_DMI_Protocol)-sizeof(dms_dmi_data->Bitmap)-sizeof(dms_dmi_data->Bitmap_Length)
                -sizeof(dms_dmi_data->Signal_Status);

        qDebug()<<"send length"<<length;
        b.resize(length);


        quint8  t=dms_dmi_data->Dest_Log_ID;
        dms_dmi_data->Dest_Log_ID= dms_dmi_data->Src_Log_ID;
        dms_dmi_data->Src_Log_ID=t;

        t=dms_dmi_data->Dest_SSID;
        dms_dmi_data->Dest_SSID= dms_dmi_data->Src_SSID;
        dms_dmi_data->Src_SSID=t;

        t=dms_dmi_data->Dest_SSTy;
        dms_dmi_data->Dest_SSTy= dms_dmi_data->Src_SSTy;
        dms_dmi_data->Src_SSTy=t;

        quint16 t2=dms_dmi_data->Receiver_Identity;
        dms_dmi_data->Receiver_Identity= dms_dmi_data->Transmitter_Identity;
        dms_dmi_data->Transmitter_Identity=t2;

        dms_dmi_data->setBytesFromData(b);

        mySocket2->writeDatagram(b,QHostAddress(remoteip2),remoteport2);


    }
    //    qDebug()<<"it is in recv parser"<<dms_dmi_data->Signal_ID<<dms_dmi_data->RTU_ID
    //           <<dms_dmi_data->RTU_Type<<dms_dmi_data->Signal_Status<<dms_dmi_data->Bitmap_Length;

}

void Casco_DMI::refreshUI()
{
    //if dmi_control == 1, should shutdown one dmi display
    refreshTop();
    refreshATP();
    refreshMission();
    refreshRRCP();
    refreshOperation();
    refreshTabMission();
    refreshGeoEvents();
    refreshMaintence();
    refreshSMS();
    flashUI();
}

void Casco_DMI::flashUI()
{
    if(timerflash>=1)
    {
        timerflash=0;
        //闪烁函数
        if(screenmode==1)
        {

            if(needspeedflash)
            {
                isspeedflash=!isspeedflash;
            }
            else
                isspeedflash=false;
            if(isspeedflash)
            {
                QPalette tmp;
                QBrush b1(QColor(255,255,0));
                QBrush b2(QColor(255,0,0));
                tmp.setBrush(QPalette::Window,b1);
                tmp.setBrush(QPalette::WindowText,b2);

                lblCurrentSpeed->setPalette(tmp);

            }
            else
            {
                QPalette tmp;
                QBrush b1(QColor(115,115,115));
                QBrush b2(QColor(0,255,0));
                tmp.setBrush(QPalette::Window,b1);
                tmp.setBrush(QPalette::WindowText,b2);
                lblCurrentSpeed->setPalette(tmp);

            }
        }
        if(needatpwarning)
        {
            isatpwarningflash=!isatpwarningflash;
            if(isatpwarningflash)
            {
                int tmp=els_dmi_data->ATP_Warning;
                if(tmp==1)
                {
                    lblAtpWarning->setPixmap(resPath+"ATP_Warning1.png");
                }
                else
                {
                    lblAtpWarning->setPixmap(resPath+"ATP_Warning2.png"); //default status
                }
            }
            else
            {
                lblAtpWarning->setPixmap(QPixmap());

            }
        }

        if(needleftflash)
        {
            isleftflash=!isleftflash;
            if(isleftflash)
            {

                btnleft->setPixmap(resPath+"Left_Route_Status1.png");
            }
            else
            {
                btnleft->setPixmap(QPixmap());
            }
        }


        if(needrightflash)
        {
            isrightflash=!isrightflash;
            if(isrightflash)
            {
                btnright->setPixmap(resPath+"Right_Route_Status1.png");

            }
            else
            {
                btnright->setPixmap(QPixmap());

            }
        }

        if(needstraflash)
        {
            isstraflash=!isstraflash;
            if(isstraflash)
            {
                btnstra->setPixmap(resPath+"Straight_Route_Status1.png");

            }
            else
            {
                btnstra->setPixmap(QPixmap());

            }
        }

    }
    else
    {
        timerflash++;
    }
}



void Casco_DMI::refreshTop()
{
    //Top
    if(els_dmi_data->Driver_Id<=32767&&els_dmi_data->Driver_Id>0)
    {
        lblDriverId->setText(QString::number(els_dmi_data->Driver_Id));
        lblNameDriver->setPixmap(resPath+"Logined.png");
    }
    else
    {
        lblDriverId->setText("未知");
        lblNameDriver->setPixmap(resPath+"key.png");
        if(els_dmi_data->Driver_Id>32767)
        {
            //            if(!log->writeLog("recv Driver_Id error value "+els_dmi_data->Driver_Id))
            //            {
            //                popFaultBox("fail to write log, now exit");
            //            }
        }
    }
    if(els_dmi_data->Service_Id<=32767&&els_dmi_data->Service_Id>0)
    {
        lblServiceId->setText(QString::number(els_dmi_data->Service_Id));
    }
    else
    {
        lblServiceId->setText("未知");
        if(els_dmi_data->Service_Id>32767)
        {
            //            if(!log->writeLog("recv Service_Id error value "+els_dmi_data->Service_Id))
            //            {
            //                popFaultBox("fail to write log, now exit");
            //            }
        }
    }

    if(SSAIdMap->contains(els_dmi_data->Next_SSA_Id))
    {
        lblNextstaId->setText(SSAIdMap->value(els_dmi_data->Next_SSA_Id));
        lblNextstaId2->setText(SSAIdMap->value(els_dmi_data->Next_SSA_Id));
    }
    else
    {
        lblNextstaId->setText(QString::number(els_dmi_data->Next_SSA_Id));
        lblNextstaId2->setText(QString::number(els_dmi_data->Next_SSA_Id));
        //        if(!log->writeLog("recv Next_SSA_Id error value "+els_dmi_data->Next_SSA_Id))
        //        {
        //            popFaultBox("fail to write log, now exit");
        //        }
    }

    QDateTime tmpnextarrtime;
    tmpnextarrtime.setTime_t(timeConvertDisplay(els_dmi_data->Next_SSA_Arrival_Time));
    lblArrTime->setText(tmpnextarrtime.toString("hh:mm:ss"));
    lblArrTime2->setText(tmpnextarrtime.toString("hh:mm:ss"));
    qint16 temp=els_dmi_data->Advance_Delay_Time;
    QPalette tmp;
    if(temp<0)
    {
        temp=-temp; //绝对值
        lblNameDelay->setText(tr("提前"));

        tmp.setBrush(QPalette::WindowText,Qt::yellow);
        lblDelay->setPalette(tmp);
    }
    else
    {
        lblNameDelay->setText(tr("晚点"));
        tmp.setBrush(QPalette::WindowText,Qt::red);
        lblDelay->setPalette(tmp);

    }
    QString delaytime;
    quint16 min,sec;
    if(temp>=60)
    {
        min=temp/60;
        delaytime=QString::number(min)+" "+tr("分");
    }
    sec=temp%60;
    delaytime+=" "+QString::number(sec)+" "+tr("秒");
    lblDelay->setText(delaytime);
    if(!time_checked)
    {
        time_checked=true;
        quint64 utctime = timeConvertDisplay(els_dmi_data->Current_Time);
        checkTimeWithELS(utctime);
    }


    //    QDateTime currentdate;
    //    currentdate.setTime_t(utctime);

}

void Casco_DMI::refreshATP()
{
    quint16 tmpvalue;
    //ATP
    quint8 tmpcurspeed=els_dmi_data->ELS_Current_Speed;
    quint8 tmplimitspeed=els_dmi_data->Authorized_Speed;

    if(tmpcurspeed>=tmplimitspeed&&tmpcurspeed!=0)
    {
        QString convertlimitspeed=  ConvertSpeed(tmplimitspeed);

        if(tmplimitspeed>0)
        {

            refreshAlarmQue(2,"[车载] ");
            refreshAlarmQue(5,"[车载] ",QString::number(tmplimitspeed),
                            convertlimitspeed);


        }
        else
        {
            refreshAlarmQue(1,"[车载] ");
        }
    }
    if(tmpcurspeed<=100)
    {
        //not record
    }
    else
    {
        //        QString content="recv Current_Speed error value "+QString::number(tmpcurspeed);
        //        if(!log->writeLog(content))
        //        {
        //            popFaultBox("fail to write log, now exit");
        //        }
    }
    if(tmplimitspeed<=100)
    {
        //not record
    }
    else
    {
        tmplimitspeed=100;
        //        QString content="recv Authorized_Speed error value "+QString::number(tmplimitspeed);
        //        if(!log->writeLog(content))
        //        {
        //            popFaultBox("fail to write log, now exit");
        //        }
    }
    switch (screenmode)
    {
    case 1:
        if(((els_dmi_data->In_Slowdown_Area==1)&&(tmplimitspeed-tmpcurspeed)<=m_diffflash)||(tmpcurspeed>=tmplimitspeed))
        {
            needspeedflash=true;

        }
        else
        {
            needspeedflash=false;

        }

        lblCurrentSpeed->setText(QString::number(tmpcurspeed));
        lblAuthoritySpeed->setText(QString::number(tmplimitspeed));
        break;
    case 2:
    case 3:
        if(tmpcurspeed>100)
        {
            mydashboard->setValue(100,tmplimitspeed,tmpcurspeed);
        }
        else
        {
            mydashboard->setValue(tmpcurspeed,tmplimitspeed,tmpcurspeed);
        }
        break;
    default:
        //impossible come here, because it had already quit in the file open.
        break;
    }


    switch(els_dmi_data->Train_Protection_Control_Status)
    {
    case 0:
        lblAtpProtection->setPixmap(resPath+"Train_Ptotect0.png");
        break;
    case 1:
        lblAtpProtection->setPixmap(resPath+"Train_Ptotect1.png");
        break;
    case 2:
        lblAtpProtection->setPixmap(resPath+"Train_Ptotect2.png");
        break;
    case 3:
        lblAtpProtection->setPixmap(resPath+"Train_Ptotect3.png");
        break;
    case 4:
        lblAtpProtection->setPixmap(resPath+"Train_Ptotect4.png");
        break;
    case 255:
        lblAtpProtection->setPixmap(QPixmap());
        break;
    default:
        lblAtpProtection->setPixmap(resPath+"Train_Ptotect0.png");
        if(!log->writeLog("recv Train_Protection_Control_Status error value "+els_dmi_data->Train_Protection_Control_Status))
        {
            popFaultBox("fail to write log, now exit");
        }
        break;
    }

    switch(els_dmi_data->ATP_Inhibition_Status)
    {
    case 0:
        lblAtpInhibited->setPixmap(resPath+"ATP_Inhi0.png");
        break;
    case 1:
        lblAtpInhibited->setPixmap(resPath+"ATP_Inhi1.png");
        break;
    case 2:
        lblAtpInhibited->setPixmap(QPixmap());
        break;
    default:
        lblAtpInhibited->setPixmap(resPath+"ATP_Inhi1.png");
        if(!log->writeLog("recv ATP_inhibition_Status error value "+els_dmi_data->ATP_Inhibition_Status))
        {
            popFaultBox("fail to write log, now exit");
        }
    }

    switch(els_dmi_data->ATP_Warning)
    {
    case 0:
    case 3:
        needatpwarning=false;
        lblAtpWarning->setPixmap(QPixmap());
        break;
    case 1:
    case 2:
        needatpwarning=true;
        break;
    default:
        needatpwarning=false;
        lblAtpWarning->setPixmap(QPixmap());
        if(!log->writeLog("recv ATP_Warning error value "+els_dmi_data->ATP_Warning))
        {
            popFaultBox("fail to write log, now exit");
        }
        break;
    }
    //    qDebug()<<"servicemode"<<els_dmi_data->ELS_Service_Mode;
    switch(els_dmi_data->ELS_Service_Mode)
    {


    case 0:
        lblServiceMode->setText(tr("timetable"));
        dmi_els_data->resetDefault();
        break;
    case 1:
    {
        tmpvalue=els_dmi_data->Time_Headway_Required;
        dmi_els_data->resetDefault();
        if(tmpvalue<=3600&&tmpvalue>0)
        {
            QString temp =QString::number(els_dmi_data->Time_Headway_Required);
            lblServiceMode->setText(tr("等间隔")+"\n"+temp+tr("秒"));
        }
        else if(tmpvalue==0)
        {
            lblServiceMode->setText(tr("等间隔")+"\n"+tr("未知秒"));
        }
        else
        {
            lblServiceMode->setText(tr("等间隔")+"\n"+tr(">3600秒"));
            if(!log->writeLog("recv Time_Headway_Required error value "+tmpvalue))
            {
                popFaultBox("fail to write log, now exit");
            }
        }
        break;
    }
    case 2:
        lblServiceMode->setText(tr("车载独立"));
        break;
    case 3:
        lblServiceMode->setText(tr("手工控制"));
        break;
    default:
        lblServiceMode->setText(tr("时刻表"));
        if(!log->writeLog("recv ELS_Service_Mode error value "+els_dmi_data->ELS_Service_Mode))
        {
            popFaultBox("fail to write log, now exit");
        }
        break;
    }

}

void Casco_DMI::refreshAlarmQue(quint8 index,QString filter)
{
    warning_index=index;
    warning_record=m_Alarm_Record_map->value(warning_index);
    smsPromte(filter+warning_record.str);
    putAlarmInQue(warning_record);
}
void Casco_DMI::refreshAlarmQue(quint8 index,QString filter,QString sms_arg1)
{
    warning_index=index;
    warning_record=m_Alarm_Record_map->value(warning_index);
    warning_record.str=warning_record.str.arg(sms_arg1);
    smsPromte(filter+warning_record.str);
    putAlarmInQue(warning_record);
}
void Casco_DMI::refreshAlarmQue(quint8 index,QString filter,QString sms_arg1,QString alarm_arg1)
{
    warning_index=index;
    warning_record=m_Alarm_Record_map->value(warning_index);
    smsPromte(filter+warning_record.str.arg(sms_arg1));
    warning_record.str=warning_record.str.arg(alarm_arg1);
    putAlarmInQue(warning_record);
}
void Casco_DMI::refreshAlarmText(quint8 index,QString filter,QString arg1)
{
    warning_index=index;
    warning_record=m_Alarm_Record_map->value(warning_index);
    alarmPromte(filter+warning_record.str.arg(arg1));

}

void Casco_DMI::refreshAlarmMap()
{
    //    qDebug()<<"in refreshalarm 1s";
    //    for(int i=1;i<m_Alarm_Record_map->size();i++)
    //    {
    QMap<quint8,Alarm_Record> *tmp=new QMap<quint8,Alarm_Record> ;
    QMap<quint8,Alarm_Record>::iterator it;
    for( it= m_Alarm_Record_map->begin();it!=m_Alarm_Record_map->end();it++)
    {
        //        qDebug()<<it.key()<<it.value().str;
        Alarm_Record rec;
        rec.id=it.value().id;
        rec.priority=it.value().priority;
        rec.playtick=it.value().playtick;
        rec.str=it.value().str;
        rec.couldplay=it.value().couldplay;
        rec.current_playtick=it.value().current_playtick;
        if(!rec.couldplay)
        {
            rec.current_playtick--;
            if(rec.current_playtick<=0)
            {
                rec.current_playtick=rec.playtick;
                rec.couldplay=true;
            }
            tmp->insert(it.key(),rec);

        }

    }

    for(it=tmp->begin();it!=tmp->end();it++)
    {
        m_Alarm_Record_map->remove(it.key());
        m_Alarm_Record_map->insert(it.key(),it.value());
    }

    delete tmp;
}

void Casco_DMI::refreshMission()
{
    //Mission Information

    if(els_dmi_data->ELS_Function_Detailed_Status<=0x0f)
    {
        if(els_dmi_data->ELS_Function_Detailed_Status==0x0f)
        {
            lblObsStatus->setPixmap(resPath+"OBS_OK.png");
        }
        else
        {
            quint8 v=els_dmi_data->ELS_Function_Detailed_Status;
            if((v&0x01)==0)
            {
                refreshAlarmQue(6,"[车载] ","E L S");
            }
            if((v&0x02)==0)
            {
                refreshAlarmQue(6,"[车载] ","R R");
            }
            if((v&0x04)==0)
            {
                refreshAlarmQue(6,"[车载] ","C P");
            }
            if((v&0x08)==0)
            {
                refreshAlarmQue(6,"[车载] ","A T P");
            }

            lblObsStatus->setPixmap(resPath+"OBS_Error.png");
        }
        if(isdialobspop)
        {
            dialobs->setPicStatus(els_dmi_data->ELS_Function_Detailed_Status);
        }
    }
    else
    {
        lblObsStatus->setPixmap(resPath+"OBS_Error.png");
    }


    if(els_dmi_data->Communication_ELS_DMS_Status!=1||els_dmi_data->Radio_Coverage_ELS_DMS!=1
            ||els_dmi_data->Radio_Status!=1)
    {
        lblELSDMSStatus->setPixmap(resPath+"Communication_fault.png");
        //        qDebug()<<"fault"<<els_dmi_data->Communication_ELS_DMS_Status<<els_dmi_data->Radio_Coverage_ELS_DMS<<els_dmi_data->Radio_Status;
    }
    else
    {
        lblELSDMSStatus->setPixmap(resPath+"Communication_good.png");
        //         qDebug()<<"good"<<els_dmi_data->Communication_ELS_DMS_Status<<els_dmi_data->Radio_Coverage_ELS_DMS<<els_dmi_data->Radio_Status;
    }

    if(isdialcommstatuspop)
    {
        dialcommstatus->setPicCommDMSELS(els_dmi_data->Communication_ELS_DMS_Status);
        dialcommstatus->setPicRadioDMSELS(els_dmi_data->Radio_Status);
        dialcommstatus->setPicRadioCoverDMSELS(els_dmi_data->Radio_Coverage_ELS_DMS);
    }
    switch(els_dmi_data->Running_Type)
    {

    case 0:
        lblRunningType->setPixmap(QPixmap());
        break;
    case 1:
        lblRunningType->setPixmap(resPath+"Running_Type1.png");
        break;
    case 2:
        lblRunningType->setPixmap(resPath+"Running_Type2.png");
        break;
    case 3:
        lblRunningType->setPixmap(resPath+"Running_Type3.png");
        break;
    default:
        lblRunningType->setPixmap(QPixmap());
        QString content=QString("recv Running_Type error value ")+QString::number(els_dmi_data->Running_Type);
        if(!log->writeLog(content))
        {
            popFaultBox("fail to write log, now exit");
        }
        break;
    }

    switch(els_dmi_data->Train_Location_Status)
    {
    case 0:
        lblLocStatus->setPixmap(resPath+"Train_Location_Status0.png");
        break;
    case 1:
        lblLocStatus->setPixmap(resPath+"Train_Location_Status1.png");
        break;
    case 2:
        lblLocStatus->setPixmap(resPath+"Train_Location_Status2.png");
        break;
    default:
        lblLocStatus->setPixmap(resPath+"Train_Location_Status0.png");
        //        QString content=QString("recv Train_Location_Status error value ")+QString::number(els_dmi_data->Train_Location_Status);
        //        if(!log->writeLog(content))
        //        {
        //            popFaultBox("fail to write log, now exit");
        //        }
        break;
    }

    switch(els_dmi_data->Passenger_Mission)
    {
    case 0:
        lblPassengerType->setPixmap(QPixmap());
        break;
    case 1:
        lblPassengerType->setPixmap(resPath+"Passenger_Mission1.png");
        break;
    case 2:
        lblPassengerType->setPixmap(resPath+"Passenger_Mission2.png");
        break;
    default:
        lblPassengerType->setPixmap(QPixmap());
        QString content=QString("recv Passenger_Mission error value ")+QString::number(els_dmi_data->Passenger_Mission);
        if(!log->writeLog(content))
        {
            popFaultBox("fail to write log, now exit");
        }
        break;
    }

    switch(els_dmi_data->System_Ok)
    {
    case 0:
        lblSystemOK->setPixmap(resPath+"SysOK_0.png");
        break;
    case 1:
        lblSystemOK->setPixmap(resPath+"SysOK_1.png");
        break;
    default:
        lblSystemOK->setPixmap(resPath+"SysOK_0.png");
        //        QString content=QString("recv Pantograph_Authorization_Status error value ")+QString::number(els_dmi_data->Pantograph_Authorization_Status);
        //        if(!log->writeLog(content))
        //        {
        //            popFaultBox("fail to write log, now exit");
        //        }
        break;
    }

    switch(els_dmi_data->ELS_RR_CP_Shutdown)
    {
    case 0:
        lblShutdownStatus->setPixmap(resPath+"ELS_RR_CP_Shutdown0.png");
        break;
    case 1:
        lblShutdownStatus->setMovie(movieshutdown);
        movieshutdown->start();
        break;
    case 2:
        lblShutdownStatus->setPixmap(resPath+"ELS_RR_CP_Shutdown2.png");
        break;
    default:
        lblShutdownStatus->setPixmap(resPath+"ELS_RR_CP_Shutdown0.png");
        QString content=QString("recv ELS_RR_CP_Shutdown error value ")+QString::number(els_dmi_data->ELS_RR_CP_Shutdown);
        if(! log->writeLog(content))
        {
            popFaultBox("fail to write log, now exit");
        }
        break;
    }

    switch(els_dmi_data->Emergency_Braking_Applied)
    {
    case 0:
        lblEb->setPixmap(QPixmap());
        break;
    case 1:
        lblEb->setPixmap(resPath+"Emergency_Braking_Applied1.png");
        refreshAlarmQue(3,"[车载] ");
        break;
    default:
        lblEb->setPixmap(QPixmap());
        QString content=QString("recv Emergency_Braking_Applied error value ")+QString::number(els_dmi_data->Emergency_Braking_Applied);
        if(!log->writeLog(content))
        {
            popFaultBox("fail to write log, now exit");
        }
        break;
    }

    switch(els_dmi_data->Service_Braking_Applied)
    {
    case 0:
        lblSb->setPixmap(QPixmap());
        break;
    case 1:
        lblSb->setPixmap(resPath+"Service_Braking_Applied1.png");
        //        refreshAlarmQue(4,"[车载] ");
        break;
    default:
        lblSb->setPixmap(QPixmap());
        QString content=QString("recv Service_Braking_Applied error value ")+QString::number(els_dmi_data->Service_Braking_Applied);
        if(!log->writeLog(content))
        {
            popFaultBox("fail to write log, now exit");
        }
        break;
    }

    switch(els_dmi_data->Turn_Back_Required)
    {
    case 1:
        lblTurnback->setPixmap(resPath+"Turn_Back_Required1.png");
        break;
    case 2:
        lblTurnback->setPixmap(resPath+"Turn_Back_Required2.png");
        break;
    case 0:
    case 3:
        lblTurnback->setPixmap(QPixmap());
        break;
    default:
        lblTurnback->setPixmap(QPixmap());
        QString content=QString("recv Turn_Back_Required error value ")+QString::number(els_dmi_data->Turn_Back_Required);
        if(!log->writeLog(content))
        {
            popFaultBox("fail to write log, now exit");
        }
        break;
    }

    switch(els_dmi_data->Skip_Next_SSA)
    {
    case 0:
        lblSkip->setPixmap(QPixmap());
        break;
    case 1:
        lblSkip->setPixmap(resPath+"Skip_Next_SSA1.png");
        break;
    default:
        lblSkip->setPixmap(QPixmap());
        //        QString content =QString("recv Skip_Next_SSA error value ")+QString::number(els_dmi_data->Skip_Next_SSA);
        //        if(!log->writeLog(content))
        //        {
        //            popFaultBox("fail to write log, now exit");
        //        }
        break;
    }

    switch(els_dmi_data->Train_Hold)
    {
    case 0:
        lblHold->setPixmap(QPixmap());
        break;
    case 1:
        lblHold->setPixmap(resPath+"Train_Hold1.png");
        break;
    default:
        lblHold->setPixmap(QPixmap());
        //        QString content=QString("recv Train_Hold error value ")+QString::number(els_dmi_data->Train_Hold);
        //        if(!log->writeLog(content))
        //        {
        //            popFaultBox("fail to write log, now exit");
        //        }
        break;
    }
}

void Casco_DMI::refreshRRCP()
{
    //RR&CP
    if(els_dmi_data->Route_Request_Area_Status==1)
    {
        quint8 RRtotal=els_dmi_data->Selection_Area_Length;
        qint16 RRremain=els_dmi_data->Distance_To_End_Selection_Area;
        RRCPbar->setMaximum(RRtotal);
        if(RRremain!=-1)
        {
            if(RRremain<RRtotal)
            {
                RRCPbar->setValue(RRremain);
            }
            else
            {
                RRCPbar->setValue(RRtotal);
            }
        }
        else
        {
            RRCPbar->setValue(RRtotal); //recv -1, indicates that not in rr zone.
        }
        lblBarTotalValue->setText(QString::number(RRtotal));
        lblBarRemainValue->setText(QString::number(RRremain));
        lblBarName->setText("进路区域");


        QPalette tmp;

        switch(els_dmi_data->Station_RR_Beacon_Link)
        {
        case 0:
            tmp.setBrush(QPalette::Window,Qt::gray);
            break;
        case 1:
            tmp.setBrush(QPalette::Window,Qt::red);
            break;
        case 2:
            tmp.setBrush(QPalette::Window,Qt::green);
            break;
        default:
            tmp.setBrush(QPalette::Window,Qt::gray);
            QString content=QString("recv Station_RR_Beacon_Link error value ")+QString::number(els_dmi_data->Station_RR_Beacon_Link);
            if(!log->writeLog(content))
            {
                popFaultBox("fail to write log, now exit");
            }
        }

        lblBarName->setPalette(tmp);

    }
    else if(els_dmi_data->CP_Area_Status==1)   //if previous statement is true, this statement will not be judged
    {
        quint8 cptotal=els_dmi_data->CP_Length_Preselection_Area;
        qint16 cpremain=els_dmi_data->CP_Distance_To_End_Preselection_Area;
        RRCPbar->setMaximum(cptotal);
        if(cpremain!=-1)
        {
            if(cpremain<cptotal)
            {
                RRCPbar->setValue(cpremain);
            }
            else
            {
                RRCPbar->setValue(cptotal);
            }
        }
        else
        {
            RRCPbar->setValue(cptotal);//recv -1, indicates that not in cp zone.
        }
        lblBarTotalValue->setText(QString::number(cptotal));
        lblBarRemainValue->setText(QString::number(cpremain));
        lblBarName->setText("路口区域");
    }
    else
    {
        //无操作
    }

    //RR
    switch(els_dmi_data->Right_Route_Status)
    {

    case 0:
    case 2:
        needrightflash=false;
        btnright->setPixmap(resPath+"Right_Route_Status0.png");
        break;
    case 1:
        needrightflash=true;
        break;
    case 3:
        needrightflash=false;
        btnright->setPixmap(resPath+"Right_Route_Status3.png");
        break;
    case 4:
        needrightflash=false;
        btnright->setPixmap(resPath+"Right_Route_Status4.png");
        break;
    default:
        needrightflash=false;
        btnright->setPixmap(resPath+"Right_Route_Status0.png");
        //        QString content=QString("recv Right_Route_Status error value ")+QString::number(els_dmi_data->Right_Route_Status);
        //        if(!log->writeLog(content))
        //        {
        //            popFaultBox("fail to write log, now exit");
        //        }
        break;
    }
    switch(els_dmi_data->Left_Route_Status)
    {

    case 0:
    case 2:
        needleftflash=false;
        btnleft->setPixmap(resPath+"Left_Route_Status0.png");
        break;
    case 1:needleftflash=true;
        break;
    case 3:
        needleftflash=false;
        btnleft->setPixmap(resPath+"Left_Route_Status3.png");
        break;
    case 4:
        needleftflash=false;
        btnleft->setPixmap(resPath+"Left_Route_Status4.png");
        break;
    default:
        needleftflash=false;
        btnleft->setPixmap(resPath+"Left_Route_Status0.png");
        //        QString content=QString("recv Left_Route_Status error value ")+QString::number(els_dmi_data->Left_Route_Status);
        //        if(!log->writeLog(content))
        //        {
        //            popFaultBox("fail to write log, now exit");
        //        }
        break;

    }

    switch(els_dmi_data->Straight_Route_Status)
    {

    case 0:
    case 2:
        needstraflash=false;
        btnstra->setPixmap(resPath+"Straight_Route_Status0.png");
        break;
    case 1:needstraflash=true;
        break;
    case 3:
        needstraflash=false;
        btnstra->setPixmap(resPath+"Straight_Route_Status3.png");
        break;
    case 4:
        needstraflash=false;
        btnstra->setPixmap(resPath+"Straight_Route_Status4.png");
        break;
    default:
        needstraflash=false;
        btnstra->setPixmap(resPath+"Straight_Route_Status0.png");
        //        QString content=QString("recv Straight_Route_Status error value ")+QString::number(els_dmi_data->Straight_Route_Status);
        //        if(!log->writeLog(content))
        //        {
        //            popFaultBox("fail to write log, now exit");
        //        }
        break;

    }

    switch(els_dmi_data->Route_Request_Area_Status)
    {

    case 0:
        frameRR->setPalette(QPalette(QPalette::Window,QColor(64,64,64)));
        break;
    case 1:
        frameRR->setPalette(QPalette(QPalette::Window,Qt::yellow));
        break;
    case 2:
        frameRR->setPalette(QPalette(QPalette::Window,Qt::green));
    default:
        frameRR->setPalette(QPalette(QPalette::Window,QColor(64,64,64)));
        QString content=QString("recv Route_Request_Area_Status error value ")+QString::number(els_dmi_data->Route_Request_Area_Status);
        if(!log->writeLog(content))
        {
            popFaultBox("fail to write log, now exit");
        }
        break;
    }
}

void Casco_DMI::refreshOperation()
{
    switch (els_dmi_data->CP_Area_Status)
    {
    case 0:
    case 4:
    case 5:
        btncp->setPixmap(resPath+"CP_Area_Status0.png");
        break;
    case 1:
    case 2:
    case 3:
        btncp->setPixmap(resPath+"CP_Area_Status1.png");
        break;

    default:
        btncp->setPixmap(resPath+"CP_Area_Status0.png");
        //        QString content=QString("recv CP_Area_Status error value ")+QString::number(els_dmi_data->CP_Area_Status);
        //        if(!log->writeLog(content))
        //        {
        //            popFaultBox("fail to write log, now exit");
        //        }
        break;
    }

}

void Casco_DMI::refreshTabMission()
{
    quint16 tmpvalue;
    quint8 schedule_key=els_dmi_data->Schedule_Id;
    if(scheduleIdMap->contains(schedule_key))
        lblSchId->setText(scheduleIdMap->value(schedule_key));
    else
    {
        lblSchId->setText(QString::number(els_dmi_data->Schedule_Id));
    }
    if(stationIdMap->contains(els_dmi_data->Destination_Id))
    {
        lblDesId->setText(stationIdMap->value(els_dmi_data->Destination_Id));
    }
    else
    {
        //        lblDesId->setText("未知");
        lblDesId->setText(QString::number(els_dmi_data->Destination_Id));
        //        QString content=QString("recv Destination_Id error value ")+QString::number(els_dmi_data->Destination_Id);
        //        if(!log->writeLog(content))
        //        {
        //            popFaultBox("fail to write log, now exit");
        //        }
    }
    if(els_dmi_data->Current_SSA_Id==0)
    {
        lblDepTime->setVisible(false);
        lblCurStaId->setVisible(false);
        lblNameDepTime->setVisible(false);
        lblNameCurSta->setVisible(false);
    }
    else
    {
        lblNameDepTime->setVisible(true);
        lblNameCurSta->setVisible(true);
        lblCurStaId->setVisible(true);
        lblDepTime->setVisible(true);
        if(SSAIdMap->contains(els_dmi_data->Current_SSA_Id))
            lblCurStaId->setText(SSAIdMap->value(els_dmi_data->Current_SSA_Id));
        else
            lblCurStaId->setText(QString::number(els_dmi_data->Current_SSA_Id));
    }

    QDateTime deptime;
    deptime.setTime_t(timeConvertDisplay(els_dmi_data->Current_SSA_Depature_Time));
    lblDepTime->setText(deptime.toString("hh:mm:ss"));

    tmpvalue=els_dmi_data->Distance_To_Downstream_Train;
    if(tmpvalue<=60000&&tmpvalue>0)
    {
        lblDownstreamDis->setText(QString::number(els_dmi_data->Distance_To_Downstream_Train)+tr("  米"));
    }
    else
    {
        if(tmpvalue==0)
        {
            lblDownstreamDis->setText("未知");
        }
        else
        {
            lblDownstreamDis->setText(">60000 米");
            QString content=QString("recv Distance_To_Downstream_Train error value ")+QString::number(els_dmi_data->Distance_To_Downstream_Train);
            if(!log->writeLog(content))
            {
                popFaultBox("fail to write log, now exit");
            }
        }

    }
    tmpvalue=els_dmi_data->Time_To_Downstream_Train;
    if(tmpvalue<=3600&&tmpvalue>0)
    {
        QString downstreamtime;
        quint16 min,sec;
        if(tmpvalue>=60)
        {
            min=tmpvalue/60;
            downstreamtime=QString::number(min)+" "+tr("分");
        }
        sec=tmpvalue%60;
        downstreamtime+=" "+QString::number(sec)+" "+tr("秒");
        lblDownstreamTime->setText(downstreamtime);
    }
    else
    {
        if(tmpvalue==0)
        {
            lblDownstreamTime->setText("未知");
        }
        else
        {
            lblDownstreamTime->setText(">60分");
            QString content=QString("recv Time_To_Downstream_Train error value ")+QString::number(els_dmi_data->Time_To_Downstream_Train);
            //            if(!log->writeLog(content))
            //            {
            //                popFaultBox("fail to write log, now exit");
            //            }
        }

    }
    tmpvalue=els_dmi_data->Distance_To_Upstream_Train;
    if(tmpvalue<=60000&&tmpvalue>0)
    {
        lblUpstreamDis->setText(QString::number(els_dmi_data->Distance_To_Upstream_Train)+tr("  米"));
    }
    else
    {
        if(tmpvalue==0)
        {
            lblUpstreamDis->setText("未知");
        }
        else
        {
            lblUpstreamDis->setText(">60000 米");
            QString content=QString("recv Distance_To_Upstream_Train error value")+QString::number(els_dmi_data->Distance_To_Upstream_Train);
            if(!log->writeLog(content))
            {
                popFaultBox("fail to write log, now exit");
            }
        }

    }
    tmpvalue=els_dmi_data->Time_To_Upstream_Train;
    if(tmpvalue<=3600&&tmpvalue>0)
    {
        QString upstreamtime;
        quint16 min,sec;
        if(tmpvalue>=60)
        {
            min=tmpvalue/60;
            upstreamtime=QString::number(min)+" "+tr("分");
        }
        sec=tmpvalue%60;
        upstreamtime+=" "+QString::number(sec)+" "+tr("秒");
        lblUpstreamTime->setText(upstreamtime);
    }
    else
    {
        if(tmpvalue==0)
        {
            lblUpstreamTime->setText("未知");
        }
        else
        {
            lblUpstreamTime->setText(">60分");
            QString content=QString("recv Time_To_Upstream_Train error value ")+QString::number(els_dmi_data->Time_To_Upstream_Train);
            //            if(!log->writeLog(content))
            //            {
            //                popFaultBox("fail to write log, now exit");
            //            }
        }

    }


    //    QDateTime deptime;
    //    deptime.setTime_t(timeConvertDisplay(els_dmi_data->Current_SSA_Depature_Time));
    //    lblDepTime->setText(deptime.toString("hh:mm:ss"));

}

void Casco_DMI::refreshGeoEvents()
{

    quint8 t=els_dmi_data->Number_of_Active_Events;
    if (t<=10)
    {
        quint8* eventidarray=new quint8[t];
        quint8* outputarray=new quint8[t];
        quint16* beginarray=new quint16[t];
        quint16* endarray=new quint16[t];

        for(int i=0;i<t;i++)
        {
            eventidarray[i]=els_dmi_data->Event_Id[i];
            outputarray[i]=els_dmi_data->Event_Output_Value[i];
            beginarray[i]=els_dmi_data->Geographic_Distance_To_begin[i];
            endarray[i]=els_dmi_data->Geographic_Distance_To_End[i];

            if(isDebug)
            {
                QString alarm="event ";
                alarm+="id "+QString::number(eventidarray[i])
                        +"outputvalue "+QString::number(outputarray[i]);
                alarmPromte(alarm);
            }

        }

        mygeoevents->setvalue(t,eventidarray,outputarray,beginarray,endarray);

        //        qDebug()<<"content";
        //        qDebug()<<dms_dmi_data->Signal_ID<<dms_dmi_data->RTU_Type
        //               <<dms_dmi_data->RTU_ID<<dms_dmi_data->Signal_Status;
        //        qDebug()<<last_dms_dmi_data->Signal_ID<<last_dms_dmi_data->RTU_Type
        //               <<last_dms_dmi_data->RTU_ID<<last_dms_dmi_data->Signal_Status;

        //        qDebug()<<"msgid"<<dms_dmi_data->Msg_ID;

        delete[] eventidarray;
        delete[] outputarray;
        delete[] beginarray;
        delete[] endarray;
    }
    else
    {
        //        QString content=QString("recv Number_of_Active_Events error value ")+QString::number(els_dmi_data->Number_of_Active_Events);
        //        if(!log->writeLog(content))
        //        {
        //            popFaultBox("fail to write log, now exit");
        //        }
    }

}

void Casco_DMI::refreshTLE()
{
    if(dms_dmi_data->Msg_ID!=102)
    {
        if(dms_dmi_data->Signal_ID==0)
        {
            widDMS->setHidden(true);
            return;
        }
        else
        {
            widDMS->setHidden(false);
        }

        switch(dms_dmi_data->RTU_ID)
        {
        case 1:
            sig=  map_CBI_Signal->value((quint8)dms_dmi_data->Signal_ID);

            break;
        case 7:
            sig=map_OLC_Signal->value((quint8)dms_dmi_data->Signal_ID);

            break;
        default:
            break;
        }
        backgroud=sig.scene;
        type=sig.type;
        position=sig.location;
        status=dms_dmi_data->Signal_Status;

        restricpos= sig.restricloc;

        quint8* bitmap= dms_dmi_data->Bitmap;
        DMI_CBI_Info bitinfo=map_DMI_CBI_Bitmap->value(sig.restricid);

        QByteArray tt((char*)bitmap,dms_dmi_data->Bitmap_Length);
        quint8 per=bitinfo.permissstatus;

        quint8 ifgreen=(tt[per/8]>>(8-per%8))&0x01;
        quint8 lef=bitinfo.leftstatus;
        quint8 ifleftopen=(tt[lef/8]>>(8-lef%8))&0x01;

        //        qDebug()<<per<<ifgreen<<lef<<ifleftopen;
        //        qDebug()<<"hex"<<(quint8)tt[5];
        if(ifgreen==1)
        {
            restricstaus=2; //green
        }
        else if(ifgreen==0)
        {
            if(ifleftopen==1)
            {
                restricstaus=3;//left yellow
            }
            else
            {
                restricstaus=1; //red
            }

        }

        mytleevents->setvalue(dms_dmi_data->Signal_ID,1,
                              1,
                              1,1,1);
        //        mytleevents->setvalue(backgroud,type,
        //                              position,
        //                              status,restricpos,restricstaus);
        //        qDebug()<<"tleeve"<<backgroud<<type<<position<<status<<restricpos<<restricstaus;

        dms_dmi_data->freePointer();
    }

}

void Casco_DMI::refreshMaintence()
{
    if(isDebug)
    {
        alarmPromte("logictrainid="+QString::number(els_dmi_data->Logical_Train_Id)
                    +" physicaltrianid="+QString::number(els_dmi_data->Physical_Train_Id)
                    +" lineid="+QString::number(els_dmi_data->Line_Id));
    }
    int t2=els_dmi_data->Number_of_Active_Fault;
    if(t2>0)
    {

        //        putAlarmInQue("有新的维护消息");

        //        tblwidgetMaintance->clearContents();
        //        tblwidgetMaintance->setRowCount(0);
        //        listAlarm->clear();
        //必须清空之前然后从头开始添加行
        for(int i=0;i<t2;i++)
        {
            int key=els_dmi_data->Id_of_Fault[t2-1-i];

            //            tblwidgetMaintance->insertRow(i);//清空之前所有行，从0开始

            QString value=DMI_Description->value(key).Chinese_Description;
            if(value=="")
                return;
            //            tblwidgetMaintance->setItem(i,0, new QTableWidgetItem(QString::number(key)));
            //            tblwidgetMaintance->setItem(i,1, new QTableWidgetItem(value));

            QDateTime t;
            t.setTime_t(timeConvertDisplay(els_dmi_data->Time_of_Appearance[t2-1-i]));

            //            tblwidgetMaintance->setItem(i,2, new QTableWidgetItem( t.toString("yyyy/MM/dd hh:mm:ss")));

            warning_msg="[维护 ] "+t.toString("hh:mm:ss")+" 车载 "+value;

            alarmPromte(warning_msg);
        }

    }
    else
    {
        //        tblwidgetMaintance->clearContents();
        //        listAlarm->clear();
    }
}
void Casco_DMI::alarmPromte(QString txt)
{
    list_alarm->enqueue(txt);
    listAlarm->clear();
    int listsize=list_alarm->size();
    for(int i=0;i<listsize&&i<m_smsdisplaycount;i++)
    {
        listAlarm->insertPlainText(list_alarm->at(listsize-1-i)+"\n");
    }
}

void Casco_DMI::smsPromte(QString txt)
{
    if(m_lastsms!=txt)
    {
        m_lastsms=txt;
    }
    else
        return;
    list_sms->enqueue(txt);
    if(displayno!=1)
        return;
    listSMS->clear();

    int listsize=list_sms->size();
    //        qDebug()<<"m_smsdisplaycount"<<m_smsdisplaycount<<"listsms"<<listsize;
    for(int i=0;i<listsize&&i<m_smsdisplaycount;i++)
    {
        listSMS->insertPlainText(list_sms->at(listsize-1-i)+"\n");
    }

}


void Casco_DMI::refreshSMS()
{
    qint16 tmpnum=els_dmi_data->Driver_Message_Number;
    if(tmpnum==-1)
        return; //no message
    quint8 tmpmsglen=els_dmi_data->Driver_Message_Length;
    if(tmpmsglen==0)
        return; //no message
    if(tmpmsglen>0&&tmpmsglen<=140)
    {
        QString tmptxt=QString::fromUtf8((char*)els_dmi_data->Driver_Message_Text);
        refreshAlarmQue(9,"[调度] ",tmptxt);

    }
    else
    {

    }


}
void Casco_DMI::writeAlarmToFile()
{
    if(list_alarm->size()>m_smsdisplaycount)
    {
        for(int i=list_alarm->size();i>m_smsdisplaycount;i--)
        {
            if(!log->writeLog(list_alarm->dequeue()))
            {
                popFaultBox("fail to write log, now exit");
            }
        }
    }
}
void Casco_DMI::writeSMSToFile()
{
    if(list_sms->size()>m_smsdisplaycount)
    {
        for(int i=list_sms->size();i>m_smsdisplaycount;i--)
        {
            if(!sms->writeSMS(list_sms->dequeue()))
            {
                popFaultBox("fail to write sms, now exit");
            }
        }
    }
}
void Casco_DMI::writeAllSMStoFile()
{
    if(list_sms->size()>0)
    {
        for(int i=list_sms->size();i>0;i--)
        {
            sms->writeSMS(list_sms->dequeue());
        }
    }
    if(list_alarm->size()>0)
    {
        for(int i=list_alarm->size();i>0;i--)
        {
            log->writeLog(list_alarm->dequeue());
        }
    }
}

void Casco_DMI::refreshMute()
{

    if(player->isMuted())
    {
        btnMute->setPixmap(resPath+"Mute.png");
        //        btnSlider->setVisible(false);

    }
    else
    {
        btnMute->setPixmap(resPath+"Sound.png");
        //        btnSlider->setVisible(true);

    }
    //    qDebug()<<"after refremute in main"<<player->isMuted();
}

void Casco_DMI::checkTimeWithELS(quint64 time)
{
    timeval p;
    p.tv_sec=time;
    p.tv_usec=0;
    int t =settimeofday(&p,NULL);
    if(t==0)
    {
        refreshAlarmText(12,"[软件] ","成功");
        //        alarmPromte("[软件] 校时成功");
    }
    else
    {
        refreshAlarmText(12,"[软件] ","失败，请使用管理员权限运行本程序，谢谢！");
        //        alarmPromte("[软件] 校时失败，请使用管理员权限运行本程序，谢谢！错误号为 "
        //                    +QString::number(t));
    }
}

void Casco_DMI::setScheduleId(qint8 scheduleid)
{
    dmi_els_data->Schedule_Id=scheduleid;
    is_scheduleid_send=true;
    //    qDebug()<<"set sch"<<dmi_els_data->Schedule_Id;
}

void Casco_DMI::setServiceId(qint8 serviceid)
{
    dmi_els_data->Service_Id=serviceid;
    is_serviceid_send=true;
    //    qDebug()<<"set service"<<dmi_els_data->Service_Id;
}

void Casco_DMI::setTripId(qint8 tripid)
{
    dmi_els_data->Trip_Id=tripid;
    is_tripid_send=true;
    //    qDebug()<<"set trip"<<dmi_els_data->Trip_Id;
}

void Casco_DMI::ToggleMute()
{
    if(player->isMuted())
    {
        player->setMuted(false);
        btnMute->setPixmap(resPath+"Sound.png");
        //        btnSlider->setVisible(true);
    }
    else
    {
        player->setMuted(true);
        btnMute->setPixmap(resPath+"Mute.png");
        //        btnSlider->setVisible(false);
    }
    //    qDebug()<<"after toogle  mute in main"<<player->isMuted();
}


void Casco_DMI::putAlarmInQue(Alarm_Record info)
{

    if(m_alarm_que->size()<20)
    {
        if(!m_Alarm_Record_map->value(info.id).couldplay)
        {
            return; //在当前playtick中直接丢弃
        }
        if(m_alarm_que->size()==0)
        {
            m_alarm_que->enqueue(info);
            //            qDebug()<<"que is empty now add"<<info.str<<info.couldplay;
        }
        else
        {
            for(int i=0;i<m_alarm_que->size();i++)
            {
                Alarm_Record record=m_alarm_que->at(i);
                if(info.id==record.id)
                {
                    return; //防止队列中存在相同
                }

                else if(info.priority<record.priority)
                {
                    m_alarm_que->insert(i,info);
                    //                    qDebug()<<"some in que is priority low than me"<<
                    //                              info.str<<i<<info.couldplay<<info.priority
                    //                           <<record.priority<<record.str;
                    return;
                }


            }
            m_alarm_que->enqueue(info);//优先级比队列所有都低，放最后
            //            return;
            //            qDebug()<<"my prio is lowest"<<info.str<<info.couldplay;
        }
    }

}

void Casco_DMI::sendMsgToELS()
{
    QByteArray qsend;

    if(m_sendTime!=255)
    {
        if(is_cp_send)
        {
            if(m_cp_sendtime>0)
                m_cp_sendtime--;
            else
            {
                is_cp_send=false;
                m_cp_sendtime=m_sendTime;
                dmi_els_data->Crossing_Priority_Request=0;
            }
        }
        if(is_elsmode_send)
        {
            if(m_elsmode_sendtime>0)
                m_elsmode_sendtime--;
            else
            {
                is_elsmode_send=false;
                m_elsmode_sendtime=m_sendTime;
                dmi_els_data->ELS_Mode_Selection=0;
            }
        }
        if(is_driverid_send)
        {
            if(m_driverid_sendtime>0)
                m_driverid_sendtime--;
            else
            {
                is_driverid_send=false;
                m_driverid_sendtime=m_sendTime;
                dmi_els_data->Driver_Id=-1;
            }
        }
        if(is_timeshift_send)
        {
            if(m_timeshift_sendtime>0)
                m_timeshift_sendtime--;
            else
            {
                is_timeshift_send=false;
                m_timeshift_sendtime=m_sendTime;
                dmi_els_data->Time_Shift_Request=0;
            }
        }
        if(is_scheduleid_send)
        {
            if(m_scheduleid_sendtime>0)
            {
                m_scheduleid_sendtime--;
            }
            else
            {
                is_scheduleid_send=false;
                m_scheduleid_sendtime=m_sendTime;
                dmi_els_data->Schedule_Id=-1;
            }
        }
        if(is_serviceid_send)
        {
            if(m_serviceid_sendtime>0)
                m_serviceid_sendtime--;
            else
            {
                is_serviceid_send=false;
                m_serviceid_sendtime=m_sendTime;
                dmi_els_data->Service_Id=-1;
            }
        }
        if(is_tripid_send)
        {
            if(m_tripid_sendtime>0)
            {
                m_tripid_sendtime--;
            }
            else
            {
                is_tripid_send=false;
                m_tripid_sendtime=m_sendTime;
                dmi_els_data->Trip_Id=-1;
            }
        }
        if(is_pathid_send)
        {
            if(m_pathid_sendtime>0)
                m_pathid_sendtime--;
            else
            {
                is_pathid_send=false;
                m_pathid_sendtime=m_sendTime;
                dmi_els_data->Path_Id=-1;
            }
        }
        if(is_desid_send)
        {
            if(m_desid_sendtime>0)
                m_desid_sendtime--;
            else
            {
                is_desid_send=false;
                m_desid_sendtime=m_sendTime;
                dmi_els_data->Destination_Id=-1;
            }
        }
    }


    int t= sizeof(DMI_ELS_Protocol)+sizeof(quint8)*dmi_els_data->OCC_Message_Length-sizeof(dmi_els_data->OCC_Message_Text)
            +sizeof(quint8)*dmi_els_data->OSS_Message_Length-sizeof(dmi_els_data->OSS_Message_Text);
    qsend.resize(sizeof(t));

    quint32 tmpsendtimeseconds=currentSecondsFromELS+els_dmi_data->Time_Zone*15*60;
    //    quint32 tmpsendtimeseconds=currentSecondsFromELS;
    if(isDebug)
    {
        alarmPromte("sendtime= "+QString::number(tmpsendtimeseconds));
        alarmPromte("scheduleid="+QString::number(dmi_els_data->Schedule_Id)+
                    "serviceid="+QString::number(dmi_els_data->Service_Id)+
                    "tripid="+QString::number(dmi_els_data->Trip_Id)+
                    "pathid="+QString::number(dmi_els_data->Path_Id)+
                    "destinationid="+QString::number(dmi_els_data->Destination_Id)+
                    "elsmode="+QString::number(dmi_els_data->ELS_Mode_Selection));
    }
    //    qDebug()<<"sendtime"<<tmpsendtimeseconds;
    dmi_els_data->DMI_Message_Time_Stamp=(quint64)(tmpsendtimeseconds)<<32;
    //    log->writeLog("send date time is no plus timezone "+QString::number(notimezonetime));
    //    log->writeLog("send date time is  plus timezone "+QString::number(tmpsendtimeseconds));
    dmi_els_data->Display_Status= els_dmi_data->DMI_Control;
    //    qDebug()<<"dmi_control"<<els_dmi_data->DMI_Control;


    sendDataFromStruct(qsend);
    dmi_els_data->DMI_Message_Number++;
    //    qDebug()<<dmi_els_data->DMI_Message_Number<<dmi_els_data->DMI_Message_Time_Stamp;
    mySocket->writeDatagram(qsend,QHostAddress(remoteip),remoteport);


    if(operateType!="")  //if driver operate, then log
    {
        char *c = qsend.data();
        //   qts<<"size is "<<QString::number(bytearray.size())<<endl;
        QString sendData="";
        for(int i=0;i<qsend.size();i++)
        {
            //        qDebug()<<(quint8)(c[i]);
            sendData=sendData+"0x"+QString::number( (quint8)(c[i]),16)+" ";
        }
        sendData+='\n'+operateType;
        //        if(!log->writeLog("DMI send msg is "+sendData))
        //        {
        //            popFaultBox("fail to write log, now exit");
        //        }
    }
}

void Casco_DMI::sendDataFromStruct(QByteArray& qsend)
{
    dmi_els_data->setBytesFromData(qsend);
}

QString ConvertSpeed(int src)
{
    int t1=src/100;
    int t2=src%100/10;
    int t3 = src%10;
    QString convertlimitspeed="";
    if(src!=0)
    {
        if(t1!=0)
        {
            convertlimitspeed+=QString::number(t1)+"百";
        }
        if(t2!=0)
        {

            if(t2!=1)
                convertlimitspeed+=QString::number(t2)+"十";
            else
            {
                convertlimitspeed+="十";
            }


        }

        if(t3!=0)
        {
            if(t3!=2)
            {

                convertlimitspeed+=QString::number(t3);
            }
            else
            {
                convertlimitspeed+="二";

            }
        }
        else
        {

        }
    }
    else
    {
        convertlimitspeed="零";
    }
    return convertlimitspeed;
}

int Casco_DMI::initStation(QMap<int,QString>* map,QString path)
{
    QFile* localFile = new QFile();
    QDomDocument* dom=new QDomDocument();
    QDomNode node;
    QDomNodeList itemlist;

    QString strfilename =path;
    if(!OpenAndSetFile(strfilename,localFile,dom))
    {
        return 0;
    }

    node=dom->elementsByTagName("Station_Info").at(0);//唯一的一条station_info记录
    itemlist= node.childNodes();
    for(int j=0;j<itemlist.size();j++)
    {
        QDomNode mynode=itemlist.at(j);
        QDomElement mye= mynode.toElement();
        int key = mye.attribute("ID").toInt();
        QString value=mye.attribute("Name");
        map->insert(key,value);
    }
    localFile->close();
    delete localFile;  //清理资源，避免内存泄露
    delete dom;
    return 1;
}

int Casco_DMI::initSchedule(QMap<int, QString> *map, QString path)
{
    QFile* localFile = new QFile();
    QDomDocument* dom=new QDomDocument();
    QDomNode node;
    QDomNodeList itemlist;

    QString strfilename =path;
    if(!OpenAndSetFile(strfilename,localFile,dom))
    {
        return 0;
    }

    node=dom->elementsByTagName("Schedule").at(0);//唯一的一条station_info记录
    itemlist= node.childNodes();
    for(int j=0;j<itemlist.size();j++)
    {
        QDomNode mynode=itemlist.at(j);
        QDomElement mye= mynode.toElement();
        int key = mye.attribute("ID").toInt();
        QString value=mye.attribute("Name");
        map->insert(key,value);
    }
    localFile->close();
    delete localFile;  //清理资源，避免内存泄露
    delete dom;
    return 1;
}

int Casco_DMI::initDriverPassword(QString path)
{
    QFile* localFile = new QFile();
    QDomDocument* dom=new QDomDocument();
    QDomNode node;
    QDomNodeList itemlist;

    QString strfilename =path;
    if(!OpenAndSetFile(strfilename,localFile,dom))
        return 0;

    node=dom->elementsByTagName("Driver_Info").at(0);//唯一的一条station_info记录
    itemlist= node.childNodes();
    for(int j=0;j<itemlist.size();j++)
    {
        QDomNode mynode=itemlist.at(j);
        QDomElement mye= mynode.toElement();
        quint16 key = mye.attribute("Name").toShort();
        quint16 value=mye.attribute("Password").toShort();
        driver_Password->insert(key,value);
    }
    localFile->close();
    delete localFile;  //清理资源，避免内存泄露
    delete dom;
    return 1;
}


int Casco_DMI::initNet(QString path)
{
    QFile* localFile = new QFile();
    QDomDocument* dom=new QDomDocument();
    QDomNode node;
    QDomNodeList itemlist;
    QString strfilename= path;
    if(!OpenAndSetFile(strfilename,localFile,dom))
        return 0;
    node=dom->elementsByTagName("Net_Info").at(0);
    itemlist= node.childNodes();

    QDomElement elenet=itemlist.at(0).toElement();
    port=elenet.attribute("localPort").toInt();
    remoteip=elenet.attribute("RemoteIP");
    remoteport=elenet.attribute("RemotePort").toInt();

    QDomElement elenet2=itemlist.at(1).toElement();
    port2=elenet2.attribute("localPort").toInt();
    remoteip2=elenet2.attribute("RemoteIP");
    remoteport2=elenet2.attribute("RemotePort").toInt();

    localFile->close();
    delete localFile;  //清理资源，避免内存泄露
    delete dom;
    return 1;
}

int Casco_DMI::initSignal(QString path, quint8 type)
{
    QFile* localFile = new QFile();
    QDomDocument* dom=new QDomDocument();
    QDomNode node;
    QDomNodeList itemlist;
    QString strfilename= path;
    if(!OpenAndSetFile(strfilename,localFile,dom))
        return 0;
    node=dom->elementsByTagName("Signal_Info").at(0);
    itemlist= node.childNodes();

    switch(type)
    {
    case 1:
        for(int i=0;i<itemlist.count();i++)
        {
            DMS_Signal_Info value;

            QDomElement elenet=itemlist.at(i).toElement();
            value.signalid =elenet.attribute("Id").toInt();
            value.type=elenet.attribute("Type").toInt();
            value.scene=elenet.attribute("Scene").toInt();
            value.location=elenet.attribute("Location").toInt();
            value.restricid=elenet.attribute("RestricId").toInt();
            value.restricloc=elenet.attribute("RestricLocation").toInt();

            map_CBI_Signal->insert(value.signalid,value);
        }
        break;
    case 2:
        for(int i=0;i<itemlist.count();i++)
        {
            DMS_Signal_Info value;

            QDomElement elenet=itemlist.at(i).toElement();
            value.signalid =elenet.attribute("Id").toInt();
            value.type=elenet.attribute("Type").toInt();
            value.scene=elenet.attribute("Scene").toInt();
            value.location=elenet.attribute("Location").toInt();

            map_OLC_Signal->insert(value.signalid,value);
        }
        break;
    }




    localFile->close();
    delete localFile;  //清理资源，避免内存泄露
    delete dom;
    return 1;
}

int Casco_DMI::initCBIBitmap(QString path)
{
    QFile* localFile = new QFile();
    QDomDocument* dom=new QDomDocument();
    QDomNode node;
    QDomNodeList itemlist;
    QString strfilename= path;
    if(!OpenAndSetFile(strfilename,localFile,dom))
        return 0;
    node=dom->elementsByTagName("Bit_Info").at(0);
    itemlist= node.childNodes();

    for(int i=0;i<itemlist.count();i++)
    {
        DMI_CBI_Info value;

        QDomElement elenet=itemlist.at(i).toElement();
        value.signalid =elenet.attribute("Id").toInt();
        value.permissstatus=elenet.attribute("permissbit").toInt();
        value.leftstatus=elenet.attribute("leftbit").toInt();

        map_DMI_CBI_Bitmap->insert(value.signalid,value);
    }


    localFile->close();
    delete localFile;  //清理资源，避免内存泄露
    delete dom;
    return 1;
}

int Casco_DMI::initFault(QString path)
{
    QFile* localFile = new QFile();
    QDomDocument* dom=new QDomDocument();
    QDomNode node;
    QDomNodeList itemlist;
    QString strfilename = path;
    if(!OpenAndSetFile(strfilename,localFile,dom))
        return 0;
    node=dom->elementsByTagName("Fault_Description_List").at(0);
    itemlist= node.childNodes();
    for(int j=0;j<itemlist.size();j++)
    {
        QDomNode mynode=itemlist.at(j);
        if(mynode.toElement().tagName()=="Fault_Description")
        {
            QDomNodeList childlist=mynode.childNodes();
            int key=0;
            Description value;
            for(int k=0;k<childlist.size();k++)
            {

                QString name=childlist.at(k).toElement().tagName();
                if(name=="Fault_Code")
                {
                    key=childlist.at(k).toElement().text().toInt();
                }
                else if(name=="English_Description")
                {
                    value.English_Description =childlist.at(k).toElement().text();
                }
                else if(name=="Chinese_Description")
                {
                    value.Chinese_Description=childlist.at(k).toElement().text();
                }

            }
            DMI_Description->insert(key,value);
        }
    }
    localFile->close();
    delete localFile;  //清理资源，避免内存泄露
    delete dom;
    return 1;
}


int Casco_DMI::initScreenmode(QString path)
{
    QFile* localFile = new QFile();
    QDomDocument* dom=new QDomDocument();
    QDomNode node;
    QDomNodeList itemlist;
    QString strfilename = path;
    if(!OpenAndSetFile(strfilename,localFile,dom))
        return 0;

    node=dom->elementsByTagName("Screen_Info").at(0);//唯一的一条station_info记录
    itemlist= node.childNodes();
    QDomElement elescreen=itemlist.at(0).toElement();

    QString value=elescreen.attribute("Mode");
    screenmode=value.toInt();

    localFile->close();
    delete localFile;  //清理资源，避免内存泄露
    delete dom;
    return 1;
}

int Casco_DMI::initOther(QString path)
{
    QFile* localFile = new QFile();
    QDomDocument* dom=new QDomDocument();
    QDomNode node;
    QDomNodeList itemlist;
    QString strfilename = path;
    if(!OpenAndSetFile(strfilename,localFile,dom))
        return 0;

    node=dom->elementsByTagName("Other_Info").at(0);//唯一的一条station_info记录
    itemlist= node.childNodes();
    QDomElement diffFlash=itemlist.at(0).toElement();
    QDomElement disConnection=itemlist.at(1).toElement();
    QDomElement appversion=itemlist.at(2).toElement();
    QDomElement dataversion=itemlist.at(3).toElement();
    QDomElement displaycount=itemlist.at(4).toElement();
    QDomElement promotesizemb=itemlist.at(5).toElement();
    QDomElement clearsizemb=itemlist.at(6).toElement();
    QDomElement splitsizemb=itemlist.at(7).toElement();
    QDomElement checktime=itemlist.at(8).toElement();
    QDomElement sendtime=itemlist.at(9).toElement();

    m_diffflash=diffFlash.attribute("Value").toUInt();
    m_discontime = disConnection.attribute("Time").toUInt();
    if(m_discontime>25)
    {
        m_discontime=25;
    }
    m_appversion=appversion.attribute("Version");
    m_dataversion=dataversion.attribute("Version");
    m_smsdisplaycount=displaycount.attribute("Displaycount").toUInt();

    m_promotesizeMB=promotesizemb.attribute("PromotesizeMB").toUInt();
    m_clearsizeMB =clearsizemb.attribute("ClearsizeMB").toUInt();
    m_writecount =splitsizemb.attribute("SplitsizeMB").toUInt();
    m_checktime = checktime.attribute("time").toUInt();
    m_sendTime= sendtime.attribute("time").toUInt();
    m_cp_sendtime=m_elsmode_sendtime=m_driverid_sendtime=m_timeshift_sendtime
            =m_scheduleid_sendtime=m_serviceid_sendtime
            =m_tripid_sendtime=m_pathid_sendtime
            =m_desid_sendtime=m_sendTime;
    //    qDebug()<<"check send"<<m_checktime<<m_sendTime;
    if(m_checktime>25)
    {
        m_checktime=25; //at most 25 minute check time
    }
    //    qDebug()<<"3 size"<<m_promotesizeMB<<m_clearsizeMB<<m_writecount;

    localFile->close();
    delete localFile;  //清理资源，避免内存泄露
    delete dom;
    return 1;
}

int Casco_DMI::initAlarm(QString path)
{
    QFile* localFile = new QFile();
    QDomDocument* dom=new QDomDocument();
    QDomNode node;
    QDomNodeList itemlist;
    QString strfilename = path;
    if(!OpenAndSetFile(strfilename,localFile,dom))
        return 0;

    node=dom->elementsByTagName("Alarm_Info").at(0);//唯一的一条station_info记录
    itemlist= node.childNodes();
    for(int i=0;i<itemlist.size();i++)
    {
        QDomElement e=itemlist.at(i).toElement();
        Alarm_Record value;
        value.couldplay=true;
        value.id=e.attribute("ID").toUInt();
        value.current_playtick= value.playtick=e.attribute("PlayTick").toUInt();
        value.priority=e.attribute("Priority").toUInt();
        value.str=e.attribute("PlayText");
        m_Alarm_Record_map->insert(value.id,value);
    }


    localFile->close();
    delete localFile;  //清理资源，避免内存泄露
    delete dom;
    return 1;
}

int  Casco_DMI::initialLogSMS()
{
    log = new DMI_Log(  m_promotesizeMB,
                        m_clearsizeMB ,
                        m_writecount );
    if(!log->initialLog(QDir::currentPath()+"/Log/"+QDateTime::currentDateTime().toString("yyyy-MM-dd_hh:mm:ss.zzz")+".txt"))
    {
        popFaultBox("fail to create log, now exit");
    }

    connect(log,&DMI_Log::aboveVolume,this,&Casco_DMI::recvMySMS);
    sms = new DMISMS(     m_promotesizeMB,
                          m_clearsizeMB ,
                          m_writecount );
    if(!sms->initialSMS(QDir::currentPath()+"/SMS/"+QDateTime::currentDateTime().toString("yyyy-MM-dd_hh:mm:ss.zzz")+".txt"))
    {
        popFaultBox("fail to create sms, now exit");
    }
    connect(sms,&DMISMS::aboveVolume,this,&Casco_DMI::recvMySMS);
    return 1;
}

int  Casco_DMI::setLogSMSVolume()
{
    log->setVolume(    m_promotesizeMB,
                       m_clearsizeMB ,
                       m_writecount );
    sms->setVolume(    m_promotesizeMB,
                       m_clearsizeMB ,
                       m_writecount );

    return 1;
}

int Casco_DMI::initTranslation(QString path)
{
    QFile* localFile = new QFile();
    QDomDocument* dom=new QDomDocument();
    QDomNode node;
    QDomNodeList itemlist;
    QString strfilename = path;
    if(!OpenAndSetFile(strfilename,localFile,dom))
        return 0;

    node=dom->elementsByTagName("Language_Info").at(0);//唯一的一条station_info记录
    itemlist= node.childNodes();
    QDomElement translationdom=itemlist.at(0).toElement();
    languageno=translationdom.attribute("ID").toUInt();

    localFile->close();
    delete localFile;  //清理资源，避免内存泄露
    delete dom;
    return 1;
}
void Casco_DMI::popExitBox(QString txt)
{
    DialogWarningbox b;
    b.setValue(txt);
    if(b.exec()==QDialog::Accepted)
    {
        //        isquit=true;
    }
}

int Casco_DMI::initialMapFromXml()
{
    QString cur = QDir::currentPath()+"/CFG/";
    if(!initStation(stationIdMap, cur+"Station.xml"))
    {
        popExitBox(tr("缺少车站名文件！"));
    }
    if(!initSchedule(scheduleIdMap, cur+"Schedule.xml"))
    {
        popExitBox(tr("缺少计划名文件！"));
    }
    if(!initStation(SSAIdMap, cur+"SSA.xml"))
    {
        popExitBox(tr("缺少SSA文件！"));
    }
    if(!initDriverPassword(cur+"Password.xml"))
    {
        popExitBox(tr("缺少密码文件！"));
    }

    if(!initNet(cur+"Net.xml"))
    {
        popExitBox(tr("缺少车载网络文件！"));
    }
    if(!initFault(cur+"Fault.xml"))
    {
        popExitBox(tr("缺少Fault文件！"));
    }
    if(!initScreenmode(cur+"Screenmode.xml"))
    {
        popExitBox(tr("缺少屏幕文件！"));
    }
    if(!initOther(cur+"Property.xml"))
    {
        popExitBox(tr("缺少Property文件！"));
    }
    if(!initAlarm(cur+"Alarm.xml"))
    {
        popExitBox(tr("缺少Alarm文件！"));
    }

    return 1;
}
void Casco_DMI:: popFaultBox(QString content)
{
    QMessageBox tmp;
    tmp.setText(content);
    tmp.exec();
    isquit=true;
}




int OpenAndSetFile(QString filename,QFile* file,QDomDocument* dom)
{
    file->setFileName(filename);
    if(!file->open(QFile::ReadOnly))
    {
        return 0;
    }
    if(!dom->setContent(file))
    {
        file->close();
        return 0;
    }
    return 1;
}
