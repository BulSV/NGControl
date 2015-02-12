#ifndef SPINBOX_H
#define SPINBOX_H

#include <QSpinBox>
#include <QTimer>

#include "RewindButton.h"

class SpinBox : public QWidget
{
    Q_OBJECT
    QSpinBox *sbInput;
    RewindButton *bDown;
    RewindButton *bUp;
public:
    explicit SpinBox(const QIcon &iconDown,
                     const QIcon &iconUp,
                     const QString &textDown = 0,
                     const QString &textUp = 0,
                     int timeToRewind = 200,
                     QWidget *parent = 0);
    explicit SpinBox(const QString &textDown,
                     const QString &textUp,
                     int timeToRewind = 200,
                     QWidget *parent = 0);
    void setRange(const int &min, const int& max);
    int value() const;
public slots:
    void setIconDownButton(const QIcon &icon);
    void setTextDownButton(const QString &text);
    void setIconUpButton(const QIcon &icon);
    void setTextUpButton(const QString &text);

    void setValue(const int &value);
signals:
    void downButtonPressed();
    void downButtonReleased();
    void upButtonPressed();
    void upButtonReleased();
};

#endif // SPINBOX_H
