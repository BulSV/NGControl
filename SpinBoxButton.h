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
                           int timeToDoubleRewind = 500,
                           QPushButton *parent = 0);

signals:
    void mousePressed();
    void mouseReleased();
public slots:
protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
private:
    QTimer *m_RewindTimer;
    int m_TimeToDoubleRewind;
private slots:
    void speedUp();
};

#endif // SPINBOXBUTTON_H
