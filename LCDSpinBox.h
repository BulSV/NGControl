#ifndef LCDSPINBOX_H
#define LCDSPINBOX_H

#include <QLCDNumber>
#include <QPushButton>
#include "ISpinBox.h"
#include "SpinBoxButton.h"

class LCDSpinBox : public ISpinBox
{
    Q_OBJECT
public:
    enum LAYOUT {
        TOP,
        BOTTOM,
        LEFT,
        RIGHT
    };

    explicit LCDSpinBox(const QIcon &iconDown,
                        const QIcon &iconUp,
                        const QString &textDown = 0,
                        const QString &textUp = 0,
                        QLCDNumber::Mode mode = QLCDNumber::Dec,
                        LAYOUT layout = BOTTOM,
                        QWidget *parent = 0);
    explicit LCDSpinBox(const QString &textDown,
                        const QString &textUp,
                        QLCDNumber::Mode mode = QLCDNumber::Dec,
                        LAYOUT layout = BOTTOM,
                        QWidget *parent = 0);
    virtual ~LCDSpinBox();

    virtual void setRange(const double &min, const double &max, const double &step);
    virtual int value() const;

    virtual QWidget *spinWidget() const;
    virtual QWidget *buttunDownWidget() const;
    virtual QWidget *buttonUpWidget() const;
public slots:
    virtual void setIconDownButton(const QIcon &icon);
    virtual void setTextDownButton(const QString &text);
    virtual void setIconUpButton(const QIcon &icon);
    virtual void setTextUpButton(const QString &text);

    virtual void setValue(const double &value);
private:
    QLCDNumber *m_LCDNumber;
    QPushButton *m_bDown;
    QPushButton *m_bUp;

    int m_base;
    int m_min;
    int m_max;
    int m_step;

    void setupGui(LAYOUT layout);
    void setupConnect();
    void digitsBase(QLCDNumber::Mode mode);
    int digitCount(const int &value);
protected slots:
    virtual void downStep();
    virtual void upStep();
};

#endif // LCDSPINBOX_H
