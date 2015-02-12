#include "RewindButton.h"
#include <QMouseEvent>

RewindButton::RewindButton(const QIcon &icon,
                           const QString &text,
                           int timeToRewind,
                           QWidget *parent)
    : QPushButton(icon, text, parent)
    , itsTimeToRewind(new QTimer(this))
    , itsTimeToDoubleRewind(new QTimer(this))
    , itsTimeRewind(timeToRewind)
{
    itsTimeToRewind->setInterval(itsTimeRewind);
    itsTimeToDoubleRewind->setInterval(3000);
    connect(itsTimeToRewind, SIGNAL(timeout()), this, SLOT(nextValueIsReady()));
    connect(itsTimeToDoubleRewind, SIGNAL(timeout()), this, SLOT(speedUp()));
}

RewindButton::RewindButton(const QString &text,
                           int timeToRewind,
                           QWidget *parent)
    : QPushButton(text, parent)
    , itsTimeToRewind(new QTimer(this))
    , itsTimeToDoubleRewind(new QTimer(this))
    , itsTimeRewind(timeToRewind)
{
    itsTimeToRewind->setInterval(itsTimeRewind);
    itsTimeToDoubleRewind->setInterval(3000);
    connect(itsTimeToRewind, SIGNAL(timeout()), this, SLOT(nextValueIsReady()));
    connect(itsTimeToDoubleRewind, SIGNAL(timeout()), this, SLOT(speedUp()));
}


void RewindButton::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        itsTimeToRewind->setInterval(itsTimeRewind);
        itsTimeToRewind->start();
        itsTimeToDoubleRewind->start();
        emit mousePressed();
    }
    e->ignore();
}

void RewindButton::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        itsTimeToRewind->stop();
        emit mouseReleased();
    }
    e->ignore();
}


void RewindButton::nextValueIsReady()
{
    emit nextValueSetting();
}


void RewindButton::speedUp()
{
    itsTimeToRewind->setInterval(0.5*itsTimeRewind);
    itsTimeToDoubleRewind->stop();
}
