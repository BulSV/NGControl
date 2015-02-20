#ifndef ISPINBOX_H
#define ISPINBOX_H

#include <QWidget>

class ISpinBox : public QWidget
{
    Q_OBJECT
public:
    explicit ISpinBox(QWidget *parent = 0);
    virtual ~ISpinBox();
    virtual void setRange(const int &min, const int& max) = 0;
    virtual int value() const = 0;
    virtual QWidget *spinWidget() const = 0;
    virtual QWidget *buttunDownWidget() const = 0;
    virtual QWidget *buttonUpWidget() const = 0;
public slots:
    virtual void setIconDownButton(const QIcon &icon) = 0;
    virtual void setTextDownButton(const QString &text) = 0;
    virtual void setIconUpButton(const QIcon &icon) = 0;
    virtual void setTextUpButton(const QString &text) = 0;

    virtual void setValue(const int &value) = 0;
signals:
    void downButtonPressed();
    void downButtonReleased();
    void upButtonPressed();
    void upButtonReleased();
    void valueChanged();
};

#endif // ISPINBOX_H
