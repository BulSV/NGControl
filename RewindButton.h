#ifndef REWINDBUTTON_H
#define REWINDBUTTON_H

#include <QPushButton>
#include <QTimer>

class RewindButton : public QPushButton
{
    Q_OBJECT
    QTimer *itsTimeToRewind;
    QTimer *itsTimeToDoubleRewind;
    int itsTimeRewind;
private slots:
    void speedUp();
protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
public:
    explicit RewindButton(const QIcon &icon,
                          const QString &text = 0,
                          int timeToRewind = 200,
                          QWidget *parent = 0);
    explicit RewindButton(const QString &text,
                          int timeToRewind = 200,
                          QWidget *parent = 0);

signals:
    void mousePressed();
    void mouseReleased();
    void nextValueSetting();
public slots:
    void nextValueIsReady();
};

#endif // REWINDBUTTON_H
