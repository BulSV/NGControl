#ifndef SPINBOX_H
#define SPINBOX_H

#include <QSpinBox>
#include <QTimer>

#include "RewindButton.h"
#include "ISpinBox.h"

class SpinBox : public ISpinBox
{
    Q_OBJECT
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

    virtual void setRange(const int &min, const int& max);
    virtual int value() const;

    virtual QWidget *spinWidget() const;
    virtual QWidget *buttunDownWidget() const;
    virtual QWidget *buttonUpWidget() const;
public slots:
    virtual void setIconDownButton(const QIcon &icon);
    virtual void setTextDownButton(const QString &text);
    virtual void setIconUpButton(const QIcon &icon);
    virtual void setTextUpButton(const QString &text);

    virtual void setValue(const int &value);
signals:
    void downButtonPressed();
    void downButtonReleased();
    void upButtonPressed();
    void upButtonReleased();
private slots:
    void ButtonsReleasedFocus();
private:
    QSpinBox *sbInput;
    RewindButton *bDown;
    RewindButton *bUp;
};

#endif // SPINBOX_H
