#include <QGridLayout>
#include "LCDSpinBox.h"

LCDSpinBox::LCDSpinBox(const QIcon &iconDown,
                       const QIcon &iconUp,
                       const QString &textDown,
                       const QString &textUp,
                       DIGIT_MODE mode,
                       QWidget *parent) :
    ISpinBox(parent)
  , m_LCDNumber(new QLCDNumber(this))
  , m_bDown(new QPushButton(iconDown, textDown, this))
  , m_bUp(new QPushButton(iconUp, textUp, this))
{
    setupGui();
    digitsBase(mode);
}

LCDSpinBox::LCDSpinBox(const QString &textDown,
                       const QString &textUp,
                       DIGIT_MODE mode,
                       QWidget *parent) :
    ISpinBox(parent)
  , m_LCDNumber(new QLCDNumber(this))
  , m_bDown(new QPushButton(textDown, this))
  , m_bUp(new QPushButton(textUp, this))
{
    setupGui();
    digitsBase(mode);
}

LCDSpinBox::~LCDSpinBox()
{
}

void LCDSpinBox::setRange(const int &min, const int &max)
{
    m_LCDNumber->setDigitCount(qMax(digitCount(min), digitCount(max)));
}

int LCDSpinBox::value() const
{
    return m_LCDNumber->value();
}

QWidget *LCDSpinBox::spinWidget() const
{
    return m_LCDNumber;
}

QWidget *LCDSpinBox::buttunDownWidget() const
{
    return m_bDown;
}

QWidget *LCDSpinBox::buttonUpWidget() const
{
    return m_bUp;
}

void LCDSpinBox::setIconDownButton(const QIcon &icon)
{
    m_bDown->setIcon(icon);
}

void LCDSpinBox::setTextDownButton(const QString &text)
{
    m_bDown->setText(text);
}

void LCDSpinBox::setIconUpButton(const QIcon &icon)
{
    m_bUp->setIcon(icon);
}

void LCDSpinBox::setTextUpButton(const QString &text)
{
    m_bUp->setText(text);
}

void LCDSpinBox::setValue(const int &value)
{
    m_LCDNumber->display(QString::number(value));
}

void LCDSpinBox::setupGui()
{
    QGridLayout *mainLayout = new QGridLayout;
    setLayout(mainLayout);

    mainLayout->addWidget(m_LCDNumber, 0, 0, 1, 2);
    mainLayout->addWidget(m_bDown, 1, 0);
    mainLayout->addWidget(m_bUp, 1, 1);
    mainLayout->setSpacing(5);
}

void LCDSpinBox::digitsBase(LCDSpinBox::DIGIT_MODE mode)
{
    switch (mode) {
    case BIN_MODE:
        m_base = 2;
        break;
    case OCT_MODE:
        m_base = 8;
        break;
    case DEC_MODE:
        m_base = 10;
        break;
    case HEX_MODE:
        m_base = 16;
        break;
    default:
        m_base = 10;
        break;
    }
}

int LCDSpinBox::digitCount(const int &value)
{
    int tempValue = value;
    int digits = 0;

    if(value <= 0) {
        ++digits;
        tempValue *= -1;
    }

    while(tempValue) {
        tempValue /= m_base;
        ++digits;
    }

    return digits;
}