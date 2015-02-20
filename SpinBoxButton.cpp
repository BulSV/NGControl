#include "SpinBoxButton.h"

SpinBoxButton::SpinBoxButton(const QIcon &icon,
                             const QString &text,
                             int timeToDoubleRewind,
                             QPushButton *parent) :
    QPushButton(icon, text, parent)
  , m_RewindTimer(new QTimer(this))
  , m_TimeToDoubleRewind(timeToDoubleRewind)
{
    m_RewindTimer->setInterval(m_TimeToDoubleRewind);

    connect(m_RewindTimer, SIGNAL(timeout()), this, SLOT(speedUp()));
}

void SpinBoxButton::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        m_RewindTimer->start();
        emit mousePressed();
    }
    e->ignore();
}

void SpinBoxButton::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        emit mouseReleased();
    }
    e->ignore();
}

void SpinBoxButton::speedUp()
{
    m_RewindTimer->setInterval(0.5*m_TimeToDoubleRewind);
}
