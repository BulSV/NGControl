#include "SpinBoxButton.h"

SpinBoxButton::SpinBoxButton(const QIcon &icon,
                             const QString &text,
                             int timeToSpeedRewind,
                             float speedRewindFactor,
                             QWidget *parent) :
    QPushButton(icon, text, parent)
  , m_RewindTimer(new QTimer(this))
{
    setTimeToSpeedRewind(timeToSpeedRewind);
    setSpeedRewindFactor(speedRewindFactor);

    m_RewindTimer->setInterval(m_TimeToSpeedRewind);

    connect(m_RewindTimer, SIGNAL(timeout()), this, SLOT(speedUp()));
    connect(m_RewindTimer, SIGNAL(timeout()), this, SIGNAL(nextValue()));
    connect(this, SIGNAL(mouseReleased()), this, SIGNAL(nextValue()));
}

SpinBoxButton::SpinBoxButton(const QString &text,
                             int timeToSpeedRewind,
                             float speedRewindFactor,
                             QWidget *parent) :
    QPushButton(text, parent)
  , m_RewindTimer(new QTimer(this))
{
    setTimeToSpeedRewind(timeToSpeedRewind);
    setSpeedRewindFactor(speedRewindFactor);

    m_RewindTimer->setInterval(m_TimeToSpeedRewind);

    connect(m_RewindTimer, SIGNAL(timeout()), this, SLOT(speedUp()));
    connect(m_RewindTimer, SIGNAL(timeout()), this, SIGNAL(nextValue()));
    connect(this, SIGNAL(mouseReleased()), this, SIGNAL(nextValue()));
}

void SpinBoxButton::setTimeToSpeedRewind(int timeToSpeedRewind)
{
    timeToSpeedRewind > 0 ? m_TimeToSpeedRewind = timeToSpeedRewind : m_TimeToSpeedRewind = 500;
}

int SpinBoxButton::timeToSpeedRewind() const
{
    return m_TimeToSpeedRewind;
}

void SpinBoxButton::setSpeedRewindFactor(float speedRewindFactor)
{
    speedRewindFactor > 0.0 ? m_SpeedRewindFactor = speedRewindFactor : m_SpeedRewindFactor = 0.5;
}

float SpinBoxButton::speedRewindFactor() const
{
    return m_SpeedRewindFactor;
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
        m_RewindTimer->stop();
        m_RewindTimer->setInterval(m_TimeToSpeedRewind);
        emit mouseReleased();
    }
    e->ignore();
}

void SpinBoxButton::speedUp()
{
    m_RewindTimer->setInterval(static_cast<int>( m_SpeedRewindFactor * static_cast<float>(m_TimeToSpeedRewind) ));
    m_RewindTimer->start();
}
