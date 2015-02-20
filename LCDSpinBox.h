#ifndef LCDSPINBOX_H
#define LCDSPINBOX_H

#include <QLCDNumber>
#include <QPushButton>
#include "ISpinBox.h"

class LCDSpinBox : public ISpinBox
{
    Q_OBJECT
public:
    enum DIGIT_MODE {
        BIN_MODE,
        OCT_MODE,
        DEC_MODE,
        HEX_MODE
    };

    explicit LCDSpinBox(const QIcon &iconDown,
                        const QIcon &iconUp,
                        const QString &textDown = 0,
                        const QString &textUp = 0,
                        DIGIT_MODE mode = DEC_MODE,
                        QWidget *parent = 0);
    explicit LCDSpinBox(const QString &textDown,
                        const QString &textUp,
                        DIGIT_MODE mode = DEC_MODE,
                        QWidget *parent = 0);
    virtual ~LCDSpinBox();

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
private:
    QLCDNumber *m_LCDNumber;
    QPushButton *m_bDown;
    QPushButton *m_bUp;

    int m_base;

    void setupGui();
    void digitsBase(DIGIT_MODE mode);
    int digitCount(const int &value);
};

#endif // LCDSPINBOX_H
