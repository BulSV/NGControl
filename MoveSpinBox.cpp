#include "MoveSpinBox.h"
#include <QGridLayout>

MoveSpinBox::MoveSpinBox(const QString &textSpin,
                         const QIcon &iconDown,
                         const QIcon &iconUp,
                         const QString &textDown,
                         const QString &textUp,
                         LAYOUT layout,
                         QWidget *parent) :
    ISpinBox(parent),
    m_Label(new QLabel(textSpin, this)),
    m_bDown(new SpinBoxButton(iconDown, textDown, 500, 0.25, this)),
    m_bUp(new SpinBoxButton(iconUp, textUp, 500, 0.25, this)),
    m_min(0),
    m_max(0),
    m_step(0),
    m_current(0)
{
    setupGui(layout);
    setupConnect();
}

MoveSpinBox::MoveSpinBox(const QString &textSpin,
                         const QString &textDown,
                         const QString &textUp,
                         MoveSpinBox::LAYOUT layout,
                         QWidget *parent) :
    ISpinBox(parent),
    m_Label(new QLabel(textSpin, this)),
    m_bDown(new SpinBoxButton(textDown, 500, 0.25, this)),
    m_bUp(new SpinBoxButton(textUp, 500, 0.25, this)),
    m_min(0),
    m_max(0),
    m_step(0),
    m_current(0)
{
    setupGui(layout);
    setupConnect();
}

void MoveSpinBox::setRange(const double &min, const double &max, const double &step)
{
    m_min = min;
    m_max = max;
    m_step = step;
}

double MoveSpinBox::value() const
{
    return m_current;
}

double MoveSpinBox::step() const
{
    return m_step;
}

double MoveSpinBox::min() const
{
    return m_min;
}

double MoveSpinBox::max() const
{
    return m_max;
}

QWidget *MoveSpinBox::spinWidget() const
{
    return m_Label;
}

QWidget *MoveSpinBox::buttunDownWidget() const
{
    return m_bDown;
}

QWidget *MoveSpinBox::buttonUpWidget() const
{
    return m_bUp;
}

void MoveSpinBox::setIconDownButton(const QIcon &icon)
{
    m_bDown->setIcon(icon);
}

void MoveSpinBox::setTextDownButton(const QString &text)
{
    m_bDown->setText(text);
}

void MoveSpinBox::setIconUpButton(const QIcon &icon)
{
    m_bUp->setIcon(icon);
}

void MoveSpinBox::setTextUpButton(const QString &text)
{
    m_bUp->setText(text);
}

void MoveSpinBox::setValue(const double &value)
{
    m_current = value;
}

void MoveSpinBox::setupGui(MoveSpinBox::LAYOUT layout)
{
    QGridLayout *mainLayout = new QGridLayout;
    setLayout(mainLayout);

    switch (layout) {
    case TOP:
        mainLayout->addWidget(m_bDown, 0, 0);
        mainLayout->addWidget(m_bUp, 0, 1);
        mainLayout->addWidget(m_Label, 1, 0, 1, 2, Qt::AlignCenter);
        mainLayout->setSpacing(5);
        break;
    case BOTTOM:
        mainLayout->addWidget(m_Label, 0, 0, 1, 2, Qt::AlignCenter);
        mainLayout->addWidget(m_bDown, 1, 0);
        mainLayout->addWidget(m_bUp, 1, 1);
        mainLayout->setSpacing(5);
        break;
    case LEFT:
        mainLayout->addWidget(m_bUp, 0, 0);
        mainLayout->addWidget(m_Label, 0, 1, 2, 1);
        mainLayout->addWidget(m_bDown, 1, 0);
        mainLayout->setSpacing(5);
        break;
    case RIGHT:
        mainLayout->addWidget(m_Label, 0, 0, 2, 1);
        mainLayout->addWidget(m_bUp, 0, 1);
        mainLayout->addWidget(m_bDown, 1, 1);
        mainLayout->setSpacing(5);
        break;
    default: // BOTTOM
        mainLayout->addWidget(m_Label, 0, 0, 1, 2);
        mainLayout->addWidget(m_bDown, 1, 0);
        mainLayout->addWidget(m_bUp, 1, 1);
        mainLayout->setSpacing(5);
        break;
    }
}

void MoveSpinBox::setupConnect()
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

void MoveSpinBox::downStep()
{
    if(m_current - m_step >= m_min) {
        m_current -= m_step;
    }
}

void MoveSpinBox::upStep()
{
    if(m_current + m_step <= m_max) {
        m_current += m_step;
    }
}
