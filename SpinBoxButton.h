#ifndef SPINBOXBUTTON_H
#define SPINBOXBUTTON_H

#include <QPushButton>
#include <QMouseEvent>
#include <QTimer>

class SpinBoxButton : public QPushButton
{
    Q_OBJECT
public:
    explicit SpinBoxButton(const QIcon &icon,
                           const QString &text = 0,
                           int timeToSpeedRewind = 500,
                           float speedRewindFactor = 0.5,
                           QWidget *parent = 0);
    explicit SpinBoxButton(const QString &text,
                           int timeToSpeedRewind = 500,
                           float speedRewindFactor = 0.5,
                           QWidget *parent = 0);
signals:
    void mousePressed();
    void mouseReleased();
    void nextValue();
public slots:
    void setTimeToSpeedRewind(int timeToSpeedRewind);
    int timeToSpeedRewind() const;

    void setSpeedRewindFactor(float speedRewindFactor);
    float speedRewindFactor() const;
protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
private:
    QTimer *m_RewindTimer;
    int m_TimeToSpeedRewind;
    float m_SpeedRewindFactor;
private slots:
    void speedUp();
};

#endif // SPINBOXBUTTON_H
