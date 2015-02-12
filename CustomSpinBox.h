#ifndef CUSTOMSPINBOX_H
#define CUSTOMSPINBOX_H

#include <QSpinBox>

class CustomSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit CustomSpinBox(QWidget *parent = 0);

signals:

public slots:
    void deselectSB();

};

#endif // CUSTOMSPINBOX_H
