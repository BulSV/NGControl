#include "SpinBox.h"
#include <QGridLayout>

SpinBox::SpinBox(const QIcon &iconDown,
                 const QIcon &iconUp,
                 const QString &textDown,
                 const QString &textUp,
                 int timeToRewind,
                 QWidget *parent)
    : ISpinBox(parent)
    , sbInput(new QSpinBox(this))
    , bDown(new RewindButton(iconDown, textDown, timeToRewind, this))
    , bUp(new RewindButton(iconUp, textUp, timeToRewind, this))
{
    sbInput->setButtonSymbols(QAbstractSpinBox::NoButtons);

    QGridLayout *grid = new QGridLayout(this);
    grid->addWidget(sbInput, 0, 0, 1, 2);
    grid->addWidget(bDown, 1, 0);
    grid->addWidget(bUp, 1, 1);
    grid->setSpacing(5);

    setLayout(grid);

    connect(bDown, SIGNAL(nextValueSetting()), sbInput, SLOT(stepDown()));
    connect(bUp, SIGNAL(nextValueSetting()), sbInput, SLOT(stepUp()));

    connect(bDown, SIGNAL(mousePressed()), this, SIGNAL(downButtonPressed()));
    connect(bDown, SIGNAL(mouseReleased()), this, SIGNAL(downButtonReleased()));
    connect(bDown, SIGNAL(mouseReleased()), this, SLOT(ButtonsReleasedFocus()));

    connect(bUp, SIGNAL(mousePressed()), this, SIGNAL(upButtonPressed()));
    connect(bUp, SIGNAL(mouseReleased()), this, SIGNAL(upButtonReleased()));
    connect(bUp, SIGNAL(mouseReleased()), this, SLOT(ButtonsReleasedFocus()));
}

SpinBox::SpinBox(const QString &textDown,
                 const QString &textUp,
                 int timeToRewind,
                 QWidget *parent)
    : ISpinBox(parent)
    , sbInput(new QSpinBox(this))
    , bDown(new RewindButton(textDown, timeToRewind, this))
    , bUp(new RewindButton(textUp, timeToRewind, this))
{
    sbInput->setButtonSymbols(QAbstractSpinBox::NoButtons);

    QGridLayout *grid = new QGridLayout(this);
    grid->addWidget(sbInput, 0, 0, 1, 2);
    grid->addWidget(bDown, 1, 0);
    grid->addWidget(bUp, 1, 1);
    grid->setSpacing(5);

    setLayout(grid);

    connect(bDown, SIGNAL(nextValueSetting()), sbInput, SLOT(stepDown()));
    connect(bUp, SIGNAL(nextValueSetting()), sbInput, SLOT(stepUp()));

    connect(bDown, SIGNAL(mousePressed()), this, SIGNAL(downButtonPressed()));
    connect(bDown, SIGNAL(mouseReleased()), this, SIGNAL(downButtonReleased()));
    connect(bDown, SIGNAL(mouseReleased()), sbInput, SLOT(deselectSB()));

    connect(bUp, SIGNAL(mousePressed()), this, SIGNAL(upButtonPressed()));
    connect(bUp, SIGNAL(mouseReleased()), this, SIGNAL(upButtonReleased()));
    connect(bUp, SIGNAL(mouseReleased()), sbInput, SLOT(deselectSB()));
}

void SpinBox::setIconDownButton(const QIcon &icon)
{
    bDown->setIcon(icon);
}

void SpinBox::setTextDownButton(const QString &text)
{
    bDown->setText(text);
}

void SpinBox::setIconUpButton(const QIcon &icon)
{
    bUp->setIcon(icon);
}

void SpinBox::setTextUpButton(const QString &text)
{
    bUp->setText(text);
}

void SpinBox::setValue(const int &value)
{
    sbInput->setValue(value);
}

void SpinBox::ButtonsReleasedFocus()
{
    sbInput->setFocus();
}

void SpinBox::setRange(const int &min, const int &max)
{
    sbInput->setRange(min, max);
}

int SpinBox::value() const
{
    return sbInput->value();
}
