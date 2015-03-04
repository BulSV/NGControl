#ifndef MOVESPINBOX_H
#define MOVESPINBOX_H

#include "ISpinBox.h"
#include "SpinBoxButton.h"
#include <QLabel>

class MoveSpinBox : public ISpinBox
{
    Q_OBJECT
public:
    enum LAYOUT {
        TOP,
        BOTTOM,
        LEFT,
        RIGHT
    };

    explicit MoveSpinBox(const QString &textSpin,
                         const QIcon &iconDown,
                         const QIcon &iconUp,
                         const QString &textDown = 0,
                         const QString &textUp = 0,
                         LAYOUT layout = BOTTOM,
                         QWidget *parent = 0);
    explicit MoveSpinBox(const QString &textSpin,
                         const QString &textDown,
                         const QString &textUp,
                         LAYOUT layout = BOTTOM,
                         QWidget *parent = 0);

    virtual void setRange(const double &min, const double& max, const double &step);
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
    QLabel *m_Label;
    QPushButton *m_bDown;
    QPushButton *m_bUp;

    double m_min;
    double m_max;
    double m_step;
    double m_current;

    void setupGui(LAYOUT layout);
    void setupConnect();
protected slots:
    virtual void downStep();
    virtual void upStep();
};

#endif // MOVESPINBOX_H
