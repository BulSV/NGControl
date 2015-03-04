#ifndef LCDSELECTSPINBOX_H
#define LCDSELECTSPINBOX_H

#include "LCDSpinBox.h"
#include <QVector>

class LCDSampleSpinBox : public LCDSpinBox
{
    Q_OBJECT
public:
    explicit LCDSampleSpinBox(const QVector<double> &samplesValues,
                              const QIcon &iconDown,
                              const QIcon &iconUp,
                              const QString &textDown = 0,
                              const QString &textUp = 0,
                              QLCDNumber::Mode mode = QLCDNumber::Dec,
                              LAYOUT layout = BOTTOM,
                              QWidget *parent = 0);
    explicit LCDSampleSpinBox(const QVector<double> &samplesValues,
                              const QString &textDown,
                              const QString &textUp,
                              QLCDNumber::Mode mode = QLCDNumber::Dec,
                              LAYOUT layout = BOTTOM,
                              QWidget *parent = 0);
    virtual void setRange(const double &min, const double& max);
public slots:
    virtual void setValue(const int &value);
private:
    QVector<double> m_SamplesValues;
    int m_CurrentSampleIndex;

    void setDigitCount();
protected slots:
    virtual void downStep();
    virtual void upStep();
};

#endif // LCDSELECTSPINBOX_H
