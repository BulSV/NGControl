#include <QGridLayout>
#include "LCDSpinBox.h"

LCDSpinBox::LCDSpinBox(const QIcon &iconDown,
                       const QIcon &iconUp,
                       const QString &textDown,
                       const QString &textUp,
                       QLCDNumber::Mode mode,
                       LAYOUT layout,
                       QWidget *parent) :
    ISpinBox(parent)
  , m_LCDNumber(new QLCDNumber(this))
  , m_bDown(new SpinBoxButton(iconDown, textDown, 500, 0.25, this))
  , m_bUp(new SpinBoxButton(iconUp, textUp, 500, 0.25, this))
  , m_min(0)
  , m_max(0)
  , m_step(0)
{
    digitsBase(mode);
    setupGui(layout);
    setupConnect();
}

LCDSpinBox::LCDSpinBox(const QString &textDown,
                       const QString &textUp,
                       QLCDNumber::Mode mode,
                       LAYOUT layout,
                       QWidget *parent) :
    ISpinBox(parent)
  , m_LCDNumber(new QLCDNumber(this))
  , m_bDown(new SpinBoxButton(textDown, 500, 0.25, this))
  , m_bUp(new SpinBoxButton(textUp, 500, 0.25, this))
  , m_min(0)
  , m_max(0)
  , m_step(0)
{
    digitsBase(mode);
    setupGui(layout);
    setupConnect();
}

LCDSpinBox::~LCDSpinBox()
{
}

void LCDSpinBox::setRange(const double &min, const double &max, const double &step)
{
    m_min = static_cast<int>(min);
    m_max = static_cast<int>(max);
    m_LCDNumber->setDigitCount(qMax(digitCount(min), digitCount(max)));
    m_step = static_cast<int>(step);
}

double LCDSpinBox::value() const
{
    return m_LCDNumber->value();
}

double LCDSpinBox::step() const
{
    return m_step;
}

double LCDSpinBox::min() const
{
    return m_min;
}

double LCDSpinBox::max() const
{
    return m_max;
}

QWidget *LCDSpinBox::spinWidget() const
{
    return m_LCDNumber;
}

QWidget *LCDSpinBox::buttonDownWidget() const
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

void LCDSpinBox::setValue(const double &value)
{
    m_LCDNumber->display(QString::number(value));
}

void LCDSpinBox::setupGui(LAYOUT layout)
{
    QGridLayout *mainLayout = new QGridLayout;
    setLayout(mainLayout);

    switch (layout) {
    case TOP:
        mainLayout->addWidget(m_bDown, 0, 0);
        mainLayout->addWidget(m_bUp, 0, 1);
        mainLayout->addWidget(m_LCDNumber, 1, 0, 1, 2, Qt::AlignCenter);
        mainLayout->setSpacing(5);
        break;
    case BOTTOM:
        mainLayout->addWidget(m_LCDNumber, 0, 0, 1, 2, Qt::AlignCenter);
        mainLayout->addWidget(m_bDown, 1, 0);
        mainLayout->addWidget(m_bUp, 1, 1);
        mainLayout->setSpacing(5);
        break;
    case LEFT:
        mainLayout->addWidget(m_bUp, 0, 0);
        mainLayout->addWidget(m_LCDNumber, 0, 1, 2, 1, Qt::AlignLeft);
        mainLayout->addWidget(m_bDown, 1, 0);
        mainLayout->setSpacing(5);
        break;
    case RIGHT:
        mainLayout->addWidget(m_LCDNumber, 0, 0, 2, 1, Qt::AlignRight);
        mainLayout->addWidget(m_bUp, 0, 1);
        mainLayout->addWidget(m_bDown, 1, 1);
        mainLayout->setSpacing(5);
        break;
    default: // BOTTOM
        mainLayout->addWidget(m_LCDNumber, 0, 0, 1, 2, Qt::AlignCenter);
        mainLayout->addWidget(m_bDown, 1, 0);
        mainLayout->addWidget(m_bUp, 1, 1);
        mainLayout->setSpacing(5);
        break;
    }

}

void LCDSpinBox::setupConnect()
{
    connect(m_bDown, SIGNAL(nextValue()), this, SLOT(downStep()));
    connect(m_bUp, SIGNAL(nextValue()), this, SLOT(upStep()));

    connect(m_bDown, SIGNAL(nextValue()), this, SIGNAL(valueChanged()));
    connect(m_bUp, SIGNAL(nextValue()), this, SIGNAL(valueChanged()));

    connect(m_bDown, SIGNAL(mousePressed()), this, SIGNAL(downButtonPressed()));
    connect(m_bDown, SIGNAL(mouseReleased()), this, SIGNAL(downButtonReleased()));

    connect(m_bUp, SIGNAL(mousePressed()), this, SIGNAL(upButtonPressed()));
    connect(m_bUp, SIGNAL(mouseReleased()), this, SIGNAL(upButtonReleased()));
}

void LCDSpinBox::digitsBase(QLCDNumber::Mode mode)
{
    switch (mode) {
    case QLCDNumber::Bin:
        m_base = 2;
        m_LCDNumber->setMode(QLCDNumber::Bin);
        break;
    case QLCDNumber::Oct:
        m_base = 8;
        m_LCDNumber->setMode(QLCDNumber::Oct);
        break;
    case QLCDNumber::Dec:
        m_base = 10;
        m_LCDNumber->setMode(QLCDNumber::Dec);
        break;
    case QLCDNumber::Hex:
        m_base = 16;
        m_LCDNumber->setMode(QLCDNumber::Hex);
        break;
    default:
        m_base = 10;
        m_LCDNumber->setMode(QLCDNumber::Dec);
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
    if(static_cast<int>(m_LCDNumber->value()) - m_step >= m_min) {
        m_LCDNumber->display(static_cast<int>(m_LCDNumber->value()) - m_step);
    }
}

void LCDSpinBox::upStep()
{
    if(static_cast<int>(m_LCDNumber->value()) + m_step <= m_max) {
        m_LCDNumber->display(static_cast<int>(m_LCDNumber->value()) + m_step);
    }
}
