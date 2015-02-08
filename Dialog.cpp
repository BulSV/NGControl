#include "Dialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QApplication>
#include <QFile>
#include <QDesktopWidget>
#include <QShortcut>
#include <QSerialPortInfo>
#include <QPalette>
#include <QIcon>

#define STARTBYTE 0x55
#define STOPBYTE 0xAA
#define BYTESLENTH 8

#define NEGATIVE 32768 // 2^15
#define OFFSET 65536 // 2^16
#define SLOPE 128

#define CPU_FACTOR 0.537
#define CPU_OFFSET 900
#define CPU_SLOPE 2.95

#define ACCURACY 0.02

#define FORMAT 'f'
#define PRECISION 2

#define BLINKTIME 500 // ms
#define DISPLAYTIME 100 // ms

Dialog::Dialog(QWidget *parent) :
        QDialog(parent),
        lPort(new QLabel(QString::fromUtf8("Port"), this)),
        cbPort(new QComboBox(this)),
        lBaud(new QLabel(QString::fromUtf8("Baud"), this)),
        cbBaud(new QComboBox(this)),
        bPortStart(new QPushButton(QString::fromUtf8("Start"), this)),
        bPortStop(new QPushButton(QString::fromUtf8("Stop"), this)),
        lTx(new QLabel("  Tx  ", this)),
        lRx(new QLabel("  Rx  ", this)),
        lcdSetTemp(new QLCDNumber(this)),
        lcdSensor1Termo(new QLCDNumber(this)),
        lcdSensor2Termo(new QLCDNumber(this)),
        lSensor1(new QLabel(QString::fromUtf8("Sensor 1:"), this)),
        lSensor2(new QLabel(QString::fromUtf8("Sensor 2:"), this)),
        bSetTemp(new QPushButton(QString::fromUtf8("Set"), this)),
        bDownTemp(new QPushButton(QIcon(":/Resources/left.png"), QString::fromUtf8(""), this)),
        bUpTemp(new QPushButton(QIcon(":/Resources/right.png"), QString::fromUtf8(""), this)),
        gbSetTemp(new QGroupBox(QString::fromUtf8("Temperature"), this)),
        gbSensors(new QGroupBox(QString::fromUtf8("Info"), this)),
        itsPort(new QSerialPort(this)),
        itsComPort(new ComPort(itsPort, ComPort::READ, STARTBYTE, STOPBYTE, BYTESLENTH, this)),
//        itsProtocol(new ReadSensorProtocol(itsComPort, this)),
        itsStatusBar (new QStatusBar(this)),
        itsBlinkTimeNone(new QTimer(this)),
        itsBlinkTimeColor(new QTimer(this)),
        itsTimeToDisplay(new QTimer(this))
{
    setLayout(new QVBoxLayout(this));

    lTx->setStyleSheet("background: red; font: bold; font-size: 10pt");
    lTx->setFrameStyle(QFrame::Box);
    lTx->setAlignment(Qt::AlignCenter);
    lTx->setMargin(2);

    lRx->setStyleSheet("background: red; font: bold; font-size: 10pt");
    lRx->setFrameStyle(QFrame::Box);
    lRx->setAlignment(Qt::AlignCenter);
    lRx->setMargin(2);

    QGridLayout *gridUp = new QGridLayout;
    gridUp->addWidget(lPort, 0, 0);
    gridUp->addWidget(cbPort, 0, 1);
    gridUp->addWidget(lBaud, 1, 0);
    gridUp->addWidget(cbBaud, 1, 1);
    // помещаю логотип фирмы
    gridUp->addWidget(new QLabel("<img src=':/Resources/elisat.png' height='40' width='150'/>", this), 0, 2, 2, 5);
    gridUp->addWidget(bPortStart, 2, 1);
    gridUp->addWidget(bPortStop, 2, 2);
    gridUp->addWidget(lTx, 2, 3);
    gridUp->addWidget(lRx, 2, 4);
    gridUp->setSpacing(5);

    QGridLayout *gridTemp = new QGridLayout;
    gridTemp->addWidget(lcdSetTemp, 0, 0, 1, 3);
    gridTemp->addWidget(bDownTemp, 1, 0);
    gridTemp->addWidget(bSetTemp, 1, 1);
    gridTemp->addWidget(bUpTemp, 1, 2);

    QGridLayout *gridInfo = new QGridLayout;
    gridInfo->addWidget(lSensor1, 0, 0);
    gridInfo->addWidget(lcdSensor1Termo, 0, 1);
    gridInfo->addWidget(lSensor2, 1, 0);
    gridInfo->addWidget(lcdSensor2Termo, 1, 1);
    gridInfo->setSpacing(5);

    gbSetTemp->setLayout(gridTemp);
    gbSensors->setLayout(gridInfo);

    QGridLayout *grid = new QGridLayout;
    grid->addItem(gridUp, 0, 0, 1, 2);
    grid->addWidget(gbSetTemp, 1, 0);
    grid->addWidget(gbSensors, 1, 1);

    layout()->addItem(grid);
    layout()->addWidget(itsStatusBar);
    layout()->setSpacing(5);

    // делает окно фиксированного размера
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);

    QStringList portsNames;

    foreach(QSerialPortInfo portsAvailable, QSerialPortInfo::availablePorts())
    {
        portsNames << portsAvailable.portName();
    }

    cbPort->addItems(portsNames);
#if defined (Q_OS_LINUX)
    cbPort->setEditable(true); // TODO Make correct viewing available ports in Linux
#else
    cbPort->setEditable(false);
#endif

    QStringList portsBauds;
    portsBauds << "115200" << "57600" << "38400";
    cbBaud->addItems(portsBauds);
    cbBaud->setEditable(false);
    bPortStop->setEnabled(false);

    bDownTemp->setMaximumSize(25, 25);
    bSetTemp->setMaximumSize(40, 25);
    bUpTemp->setMaximumSize(25, 25);

    itsStatusBar->show();

    itsBlinkTimeNone->setInterval(BLINKTIME);
    itsBlinkTimeColor->setInterval(BLINKTIME);
    itsTimeToDisplay->setInterval(DISPLAYTIME);

    QList<QLCDNumber*> list;
    list << lcdSetTemp << lcdSensor1Termo << lcdSensor2Termo;
    foreach(QLCDNumber *lcd, list) {
        lcd->setMinimumSize(80, 40);
        lcd->setMaximumSize(80, 40);
        lcd->setDigitCount(6);
        lcd->setSegmentStyle(QLCDNumber::Flat);
        lcd->setFrameStyle(QFrame::NoFrame);
    }

    connect(bPortStart, SIGNAL(clicked()), this, SLOT(openPort()));
    connect(bPortStop, SIGNAL(clicked()), this, SLOT(closePort()));
    connect(cbPort, SIGNAL(currentIndexChanged(int)), this, SLOT(cbPortChanged()));
    connect(cbBaud, SIGNAL(currentIndexChanged(int)), this, SLOT(cbPortChanged()));
//    connect(itsProtocol, SIGNAL(DataIsReaded(bool)), this, SLOT(received(bool)));
    connect(itsBlinkTimeColor, SIGNAL(timeout()), this, SLOT(colorIsRx()));
    connect(itsBlinkTimeNone, SIGNAL(timeout()), this, SLOT(colorNoneRx()));
    connect(itsTimeToDisplay, SIGNAL(timeout()), this, SLOT(display()));

    QShortcut *aboutShortcut = new QShortcut(QKeySequence("F1"), this);
    connect(aboutShortcut, SIGNAL(activated()), qApp, SLOT(aboutQt()));
}

Dialog::~Dialog()
{    
    itsPort->close();
}

void Dialog::openPort()
{
    itsPort->close();
    itsPort->setPortName(cbPort->currentText());

    if(itsPort->open(QSerialPort::ReadOnly))
    {
        switch (cbBaud->currentIndex()) {
        case 0:
            itsPort->setBaudRate(QSerialPort::Baud115200);
            break;
        case 1:
            itsPort->setBaudRate(QSerialPort::Baud57600);
            break;
        case 2:
            itsPort->setBaudRate(QSerialPort::Baud38400);
            break;
        default:
            itsPort->setBaudRate(QSerialPort::Baud115200);
            break;
        }

        itsPort->setDataBits(QSerialPort::Data8);
        itsPort->setParity(QSerialPort::NoParity);
        itsPort->setFlowControl(QSerialPort::NoFlowControl);

        itsStatusBar->showMessage(QString::fromUtf8("Port: ") +
                             QString(itsPort->portName()) +                             
                             QString::fromUtf8(" | Baud: ") +
                             QString(QString::number(itsPort->baudRate())) +                             
                             QString::fromUtf8(" | Data bits: ") +
                             QString(QString::number(itsPort->dataBits())));
        bPortStart->setEnabled(false);
        bPortStop->setEnabled(true);
        lTx->setStyleSheet("background: none; font: bold; font-size: 10pt");
        lRx->setStyleSheet("background: none; font: bold; font-size: 10pt");
    }
    else
    {
        itsStatusBar->showMessage(QString::fromUtf8("Error opening port: ") +
                             QString(itsPort->portName()));
        lTx->setStyleSheet("background: red; font: bold; font-size: 10pt");
        lRx->setStyleSheet("background: red; font: bold; font-size: 10pt");
    }
}

void Dialog::closePort()
{
    itsPort->close();
    itsBlinkTimeNone->stop();
    itsBlinkTimeColor->stop();
    lTx->setStyleSheet("background: red; font: bold; font-size: 10pt");
    lRx->setStyleSheet("background: red; font: bold; font-size: 10pt");
    bPortStop->setEnabled(false);
    bPortStart->setEnabled(true);
//    itsProtocol->resetProtocol();
}

void Dialog::cbPortChanged()
{
    bPortStart->setEnabled(true);
    bPortStop->setEnabled(false);
}

void Dialog::received(bool isReceived)
{
//    if(isReceived) {
//        if(!itsBlinkTimeColor->isActive() && !itsBlinkTimeNone->isActive()) {
//            itsBlinkTimeColor->start();
//            lRx->setStyleSheet("background: green; font: bold; font-size: 10pt");
//        }

//        if(!itsTimeToDisplay->isActive()) {
//            itsTimeToDisplay->start();
//        }

//        QList<QString> strKeysList = itsProtocol->getReadedData().keys();
//        for(int i = 0; i < itsProtocol->getReadedData().size(); ++i) {
//            itsSensorsList.append(itsProtocol->getReadedData().value(strKeysList.at(i)));
//        }
//    }
}

void Dialog::setColorLCD(QLCDNumber *lcd, bool isHeat)
{
    QPalette palette;
    // get the palette
    palette = lcd->palette();
    if(isHeat) {
        // foreground color
        palette.setColor(palette.WindowText, QColor(100, 0, 0));
        // "light" border
        palette.setColor(palette.Light, QColor(100, 0, 0));
        // "dark" border
        palette.setColor(palette.Dark, QColor(100, 0, 0));
    } else {
        // foreground color
        palette.setColor(palette.WindowText, QColor(0, 0, 100));
        // "light" border
        palette.setColor(palette.Light, QColor(0, 0, 100));
        // "dark" border
        palette.setColor(palette.Dark, QColor(0, 0, 100));
    }
    // set the palette
    lcd->setPalette(palette);
}

QString &Dialog::addTrailingZeros(QString &str, int prec)
{
    if(str.isEmpty() || prec < 1) { // if prec == 0 then it's no sense
        return str;
    }

    int pointIndex = str.indexOf(".");
    if(pointIndex == -1) {
        str.append(".");
        pointIndex = str.size() - 1;
    }

    if(str.size() - 1 - pointIndex < prec) {
        for(int i = 0; i < prec - (str.size() - 1 - pointIndex); ++i) {
            str.append("0");
        }
    }

    return str;
}

void Dialog::colorIsRx()
{
    lTx->setStyleSheet("background: none; font: bold; font-size: 10pt");
    lRx->setStyleSheet("background: none; font: bold; font-size: 10pt");
    itsBlinkTimeColor->stop();
    itsBlinkTimeNone->start();
}

void Dialog::colorNoneRx()
{
    itsBlinkTimeNone->stop();
}

void Dialog::display()
{
    itsTimeToDisplay->stop();

    QList<QLCDNumber*> list;
    list << lcdSensor2Termo << lcdSensor1Termo << lcdSetTemp;
    QString tempStr;

    for(int k = 0; k < list.size(); ++k) {
        tempStr = itsSensorsList.at(itsSensorsList.size() - 1 - k);

        if(list.at(k)->digitCount() < addTrailingZeros(tempStr, PRECISION).size())
        {
            list[k]->display("ERR"); // Overflow
        } else {
            list[k]->display(addTrailingZeros(tempStr, PRECISION));
        }

        setColorLCD(list[k], tempStr.toDouble() > 0.0);
#ifdef DEBUG
        qDebug() << "itsTempSensorsList.size() =" << itsSensorsList.size();
        qDebug() << "Temperature[" << k << "] =" << list.at(k)->value();
#endif
    }

    itsSensorsList.clear();
}
