#ifndef DIALOG_H
#define DIALOG_H

#ifdef DEBUG
#include <QDebug>
#endif

#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>
#include <QStatusBar>
#include <QTimer>
#include <QLCDNumber>
#include <QSpinBox>
#include "ComPort.h"
#include "IProtocol.h"
#include "NGProtocol.h"
#include "LCDSpinBox.h"
#include "PlotterDialog.h"

class Dialog : public QDialog
{
    Q_OBJECT

    QLabel *lPort;
    QComboBox *cbPort;
    QLabel *lBaud;
    QComboBox *cbBaud;
    QPushButton *bPortStart;
    QPushButton *bPortStop;
    QLabel *lTx;
    QLabel *lRx;

    ISpinBox *sbSetTemp;
    QLCDNumber *lcdInstalledTemp;
    QLCDNumber *lcdSensor1Termo;
    QLCDNumber *lcdSensor2Termo;

    QLabel *lSensor1;
    QLabel *lSensor2;

    QPushButton *bSetTemp;

    QGroupBox *gbSetTemp;
    QGroupBox *gbSensors;

    QSerialPort *itsPort;
    ComPort *itsComPort;
    IProtocol *itsProtocol;

    QStringList itsSensorsList;

    // цвет индикации температуры >0 & <=0
    void setColorLCD(QLCDNumber *lcd, bool isHeat);
    // добавляет завершающие нули
    QString &addTrailingZeros(QString &str, int prec);

    QStatusBar *itsStatusBar;
    QTimer *itsBlinkTimeTxNone;
    QTimer *itsBlinkTimeRxNone;
    QTimer *itsBlinkTimeTxColor;
    QTimer *itsBlinkTimeRxColor;
    QTimer *itsTimeToDisplay;

    PlotterDialog *plotterDialog;
private slots:
    void openPort();
    void closePort();
    void received(bool isReceived);
    void writeTemp();
    // мигание надписей "Rx" - при получении и "Tx" - при отправке пакета
    void colorTxNone();
    void colorRxNone();
    void colorIsTx();
    void colorIsRx();
    // display current Rx data
    void display();
    // colored set temp LCD if value < 0
    void colorSetTempLCD();
public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
};

#endif // DIALOG_H
