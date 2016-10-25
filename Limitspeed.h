#ifndef LIMITSPEED_H
#define LIMITSPEED_H

#include <QWidget>
#include <QPainter>
#include <QDebug>

class LimitSpeed : public QWidget
{
    Q_OBJECT
public:
    explicit LimitSpeed(QWidget *parent = 0);
    void setSpeed(quint8 speed);

private:
    quint8 m_speed;

protected:
    void paintEvent(QPaintEvent *);
    QSize sizeHint() const;
signals:

public slots:
};

#endif // LIMITSPEED_H
