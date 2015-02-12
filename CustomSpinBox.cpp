#include "CustomSpinBox.h"
#include <QLineEdit>

CustomSpinBox::CustomSpinBox(QWidget *parent) :
    QSpinBox(parent)
{
}

void CustomSpinBox::deselectSB()
{
    lineEdit()->setFocus(Qt::MouseFocusReason);
}
