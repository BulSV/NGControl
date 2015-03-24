#include "LCDSampleSpinBox.h"

LCDSampleSpinBox::LCDSampleSpinBox(const QVector<double> &samplesValues,
                                   const QIcon &iconDown,
                                   const QIcon &iconUp,
                                   const QString &textDown,
                                   const QString &textUp,
                                   QLCDNumber::Mode mode,
                                   LAYOUT layout,
                                   QWidget *parent) :
    LCDSpinBox(iconDown, iconUp, textDown, textUp, mode, layout, parent),
    m_SamplesValues(samplesValues),
    m_CurrentSampleIndex(0)
{
    setDigitCount();
}

LCDSampleSpinBox::LCDSampleSpinBox(const QVector<double> &samplesValues,
                                   const QString &textDown,
                                   const QString &textUp,
                                   QLCDNumber::Mode mode,
                                   LCDSpinBox::LAYOUT layout,
                                   QWidget *parent) :
    LCDSpinBox(textDown, textUp, mode, layout, parent),
    m_SamplesValues(samplesValues),
    m_CurrentSampleIndex(0)
{
    setDigitCount();
}

void LCDSampleSpinBox::setRange(const double &min, const double &max, const double &step)
{
}

void LCDSampleSpinBox::setValue(const double &sampleIndex)
{
    if(static_cast<int>(sampleIndex) < 0 || static_cast<int>(sampleIndex) > m_SamplesValues.size() - 1) {
        m_CurrentSampleIndex = 0;
    } else {
        m_CurrentSampleIndex = sampleIndex;
    }
    dynamic_cast<QLCDNumber*>(spinWidget())->display(m_SamplesValues.at(m_CurrentSampleIndex));
}

void LCDSampleSpinBox::setDigitCount()
{
    int digitCount = 0;
    for( int i = 0; i < m_SamplesValues.size(); ++i ) {
        if( QString::number(m_SamplesValues.at(i)).size() > digitCount ) {
            digitCount = QString::number(m_SamplesValues.at(i)).size();
        }
    }
    dynamic_cast<QLCDNumber*>(spinWidget())->setDigitCount(digitCount);
}

void LCDSampleSpinBox::downStep()
{
    if(m_CurrentSampleIndex > 0) {
        dynamic_cast<QLCDNumber*>(spinWidget())->display(m_SamplesValues.at(--m_CurrentSampleIndex));
        emit valueChanged();
    }
}

void LCDSampleSpinBox::upStep()
{
    if(m_CurrentSampleIndex < m_SamplesValues.size() - 1) {
        dynamic_cast<QLCDNumber*>(spinWidget())->display(m_SamplesValues.at(++m_CurrentSampleIndex));
        emit valueChanged();
    }
}
