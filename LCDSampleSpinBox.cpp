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
}

void LCDSampleSpinBox::setValue(const int &sampleIndex)
{
    if(sampleIndex < 0 || sampleIndex > m_SamplesValues.size() - 1) {
        m_CurrentSampleIndex = 0;
    } else {
        m_CurrentSampleIndex = sampleIndex;
    }
    dynamic_cast<QLCDNumber*>(spinWidget())->display(m_SamplesValues.at(m_CurrentSampleIndex));
}

void LCDSampleSpinBox::downStep()
{
    if(m_CurrentSampleIndex > 0) {
        dynamic_cast<QLCDNumber*>(spinWidget())->display(m_SamplesValues.at(--m_CurrentSampleIndex));
    }
}

void LCDSampleSpinBox::upStep()
{
    if(m_CurrentSampleIndex < m_SamplesValues.size() - 1) {
        dynamic_cast<QLCDNumber*>(spinWidget())->display(m_SamplesValues.at(++m_CurrentSampleIndex));
    }
}
