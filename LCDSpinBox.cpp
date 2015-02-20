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
  , m_bDown(new SpinBoxButton(iconDown, textDown, 500, 0.25, this))
  , m_bUp(new SpinBoxButton(iconUp, textUp, 500, 0.25, this))
{
    setupGui();
    digitsBase(mode);
    setupConnect();
}

LCDSpinBox::LCDSpinBox(const QString &textDown,
                       const QString &textUp,
                       DIGIT_MODE mode,
                       QWidget *parent) :
    ISpinBox(parent)
  , m_LCDNumber(new QLCDNumber(this))
  , m_bDown(new SpinBoxButton(textDown, 500, 0.25, this))
  , m_bUp(new SpinBoxButton(textUp, 500, 0.25, this))
{
    setupGui();
    digitsBase(mode);
    setupConnect();
}

LCDSpinBox::~LCDSpinBox()
{
}

void LCDSpinBox::setRange(const int &min, const int &max)
{
    m_min = min;
    m_max = max;
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

void LCDSpinBox::setupConnect()
{
    connect(m_bDown, SIGNAL(nextValue()), this, SLOT(downStep()));
    connect(m_bUp, SIGNAL(nextValue()), this, SLOT(upStep()));

    connect(m_bDown, SIGNAL(mousePressed()), this, SIGNAL(downButtonPressed()));
    connect(m_bDown, SIGNAL(mouseReleased()), this, SIGNAL(downButtonReleased()));

    connect(m_bUp, SIGNAL(mousePressed()), this, SIGNAL(upButtonPressed()));
    connect(m_bUp, SIGNAL(mouseReleased()), this, SIGNAL(upButtonReleased()));
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

void LCDSpinBox::downStep()
{
    if(static_cast<int>(m_LCDNumber->value()) - 1 >= m_min) {
        m_LCDNumber->display(static_cast<int>(m_LCDNumber->value()) - 1);
    }
}

void LCDSpinBox::upStep()
{
    if(static_cast<int>(m_LCDNumber->value()) + 1 <= m_max) {
        m_LCDNumber->display(static_cast<int>(m_LCDNumber->value()) + 1);
    }
}
