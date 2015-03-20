#include "PlotterDialog.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QStringList>
#include <qwt_plot_canvas.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_div.h>
//#include <qwt_panner.h>
#include <qwt_picker_machine.h>

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

#define BLINKTIMETX 200 // ms
#define BLINKTIMERX 500 // ms
#define DISPLAYTIME 100 // ms

#define REWINDTIME 50 // ms

#define TEMPRANGE_MIN -70 // degrees Celsius
#define TEMPRANGE_MAX 70 // degrees Celsius
#define TEMPSTEP 1 // degrees Celsius
#define NORMAL_TEMP 35 // degrees Celsius

#define XDIVISION 10
#define XMAJORDIVISION 10
#define XMINORDIVISION 5
#define XSCALESTEP 1

#define YDIVISION 8
#define YMAJORDIVISION 10
#define YMINORDIVISION 5
#define YSCALESTEP 1

#define MIN_INT -32767
#define MAX_INT 32767

#define LOWTIME MIN_INT
#define UPTIME MAX_INT
#define STEPTIME 1

#define LOWTEMP MIN_INT
#define UPTEMP MAX_INT
#define STEPTEMP 1

PlotterDialog::PlotterDialog(const QString &title, QWidget *parent) :
    QDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint),
    m_cbTimeAccurate(new QCheckBox(QString::fromUtf8("Accurate (x0.1)"), this)),
    m_cbTempAccurate(new QCheckBox(QString::fromUtf8("Accurate (x0.1)"), this)),
    m_bReset(new QPushButton(QString::fromUtf8("Reset"), this)),
    m_bPauseRessume(new QPushButton(QString::fromUtf8("Pause"), this)),
    m_TimeAccurateFactor(1.0),
    m_TempAccurateFactor(1.0),
    m_plot(new QwtPlot(this)),
    m_currentTime( new QTime()),
    m_isReseted( false ),
    m_isRessumed( true ),
    m_prevCurrentTime( 0.0 ),
    m_prevCentralTemp( 0.0 ),
    m_prevTempOffset( 0.0 ),
    lPort(new QLabel(QString::fromUtf8("Port"), this)),
    cbPort(new QComboBox(this)),
    lBaud(new QLabel(QString::fromUtf8("Baud"), this)),
    cbBaud(new QComboBox(this)),
    bPortStart(new QPushButton(QString::fromUtf8("Start"), this)),
    bPortStop(new QPushButton(QString::fromUtf8("Stop"), this)),
    chbSynchronize(new QCheckBox(QString::fromUtf8("Enable synchronization"), this)),
    lTx(new QLabel("  Tx  ", this)),
    lRx(new QLabel("  Rx  ", this)),
    sbSetTemp(new LCDSpinBox(QIcon(":/Resources/down.png"),
                             QIcon(":/Resources/up.png"),
                             QString::fromUtf8(""),
                             QString::fromUtf8(""),
                             QLCDNumber::Dec,
                             LCDSpinBox::RIGHT,
                             this)),
    lcdInstalledTemp(new QLCDNumber(this)),
    lcdSensor1Termo(new QLCDNumber(this)),
    lcdSensor2Termo(new QLCDNumber(this)),
    bSetTemp(new QPushButton(QString::fromUtf8("Set"), this)),
    gbSetTemp(new QGroupBox(QString::fromUtf8("Temperature, °C"), this)),
    gbSensors(new QGroupBox(QString::fromUtf8("Information"), this)),
    itsPort(new QSerialPort(this)),
    itsComPort(new ComPort(itsPort, STARTBYTE, STOPBYTE, BYTESLENTH, this)),
    itsProtocol(new NGProtocol(itsComPort, this)),
    itsBlinkTimeTxNone(new QTimer(this)),
    itsBlinkTimeRxNone(new QTimer(this)),
    itsBlinkTimeTxColor(new QTimer(this)),
    itsBlinkTimeRxColor(new QTimer(this)),
    itsTimeToDisplay(new QTimer(this))
{
    QVector<double> timeSamples;
    timeSamples << 0.5 << 1 << 2 << 5 << 10 << 20 << 30 << 40 << 50 << 60;

    QVector<double> tempSamples;
    tempSamples << 0.5 << 1 << 2 << 3 << 5 << 10;

    m_lcdTimeInterval = new LCDSampleSpinBox(timeSamples,
                                             QIcon(":/Resources/down.png"),
                                             QIcon(":/Resources/up.png"),
                                             QString::fromUtf8(""),
                                             QString::fromUtf8(""),
                                             QLCDNumber::Dec,
                                             LCDSpinBox::RIGHT,
                                             this);
    m_lcdTimeInterval->setValue(4);

    m_lcdTempInterval = new LCDSampleSpinBox(tempSamples,
                                                      QIcon(":/Resources/down.png"),
                                                      QIcon(":/Resources/up.png"),
                                                      QString::fromUtf8(""),
                                                      QString::fromUtf8(""),
                                                      QLCDNumber::Dec,
                                                      LCDSpinBox::RIGHT,
                                                      this);
    m_lcdTempInterval->setValue(5);

    m_msbTimeInterval = new MoveSpinBox("<img src=':Resources/LeftRight.png' height='20' width='45'/>",
                                        QIcon(":/Resources/left.png"),
                                        QIcon(":/Resources/right.png"),
                                        QString::fromUtf8(""),
                                        QString::fromUtf8(""),
                                        MoveSpinBox::BOTTOM,
                                        this);
    m_msbTimeInterval->setRange(LOWTIME, UPTIME, STEPTIME);

    m_msbTempInterval = new MoveSpinBox("<img src=':Resources/UpDown.png' height='45' width='20'/>",
                                        QIcon(":/Resources/down.png"),
                                        QIcon(":/Resources/up.png"),
                                        QString::fromUtf8(""),
                                        QString::fromUtf8(""),
                                        MoveSpinBox::RIGHT,
                                        this);
    m_msbTempInterval->setRange(LOWTEMP, UPTEMP, STEPTEMP);

    setWindowTitle(title);

    QList<QLCDNumber*> lcdList;
    lcdList << dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())
            << dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())
            << lcdInstalledTemp << lcdSensor1Termo << lcdSensor2Termo
            << dynamic_cast<QLCDNumber*>(sbSetTemp->spinWidget());
    lcdStyling(lcdList);

    sbSetTemp->setRange(TEMPRANGE_MIN, TEMPRANGE_MAX, TEMPSTEP);
    sbSetTemp->setValue(NORMAL_TEMP);

    QList<QLCDNumber*> list;
    list << lcdInstalledTemp << lcdSensor1Termo << lcdSensor2Termo;
    foreach(QLCDNumber *lcd, list) {
        lcd->setDigitCount(6);
        lcd->setFixedSize(100, 40);
    }
    colorSetTempLCD();

    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setBorderRadius(5);
    canvas->setFrameShadow(QwtPlotCanvas::Sunken);

    m_plot->setCanvas(canvas);
    m_plot->setCanvasBackground(QBrush(QColor("#FFFFFF")));

    // legend
//    QwtLegend *legend = new QwtLegend;
//    m_plot->insertLegend( legend, QwtPlot::TopLegend );

    // grid
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableXMin( true );
    grid->enableYMin( true );
    grid->setMajorPen( Qt::black, 0 );
    grid->setMinorPen( Qt::gray, 0, Qt::DotLine );
    grid->attach( m_plot );

    // axes
    m_plot->setAxisTitle( QwtPlot::xBottom, "Time, sec" );
    m_plot->setAxisScale( QwtPlot::xBottom,
                          0,
                          dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())->value() * XDIVISION,
                          dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())->value() * XSCALESTEP );
    m_plot->setAxisMaxMajor( QwtPlot::xBottom, XMAJORDIVISION );
    m_plot->setAxisMaxMinor( QwtPlot::xBottom, XMINORDIVISION );
    m_plot->setAxisAutoScale( QwtPlot::xBottom, false);

    m_plot->setAxisTitle( QwtPlot::yLeft, "Temperature, °C" );
    m_plot->setAxisScale( QwtPlot::yLeft,
                          - dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value() * YDIVISION/2,
                          dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value() * YDIVISION/2,
                          dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value() * YSCALESTEP );
    m_plot->setAxisMaxMajor( QwtPlot::yLeft, YMAJORDIVISION );
    m_plot->setAxisMaxMinor( QwtPlot::yLeft, YMINORDIVISION );
    m_plot->setAxisAutoScale( QwtPlot::yLeft, false);

    m_plot->setAutoReplot( false );

    m_picker = new QwtPlotPicker(QwtPlot::xBottom,
                                 QwtPlot::yLeft,
                                 QwtPlotPicker::CrossRubberBand,
                                 QwtPicker::ActiveOnly,
                                 m_plot->canvas());
    m_picker->setStateMachine(new QwtPickerDragPointMachine());
    m_picker->setRubberBandPen(QColor(Qt::magenta));
    m_picker->setTrackerPen(QColor(Qt::magenta));
    m_picker->setTrackerFont(QFont(m_picker->trackerFont().family(), 12));

    dynamic_cast<QPushButton *>( m_msbTimeInterval->buttonUpWidget() )->setEnabled( false );
    dynamic_cast<QPushButton *>( m_msbTimeInterval->buttonDownWidget() )->setEnabled( false );
    m_cbTimeAccurate->setEnabled( false );

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

    lTx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
    lTx->setFrameStyle(QFrame::Box);
    lTx->setAlignment(Qt::AlignCenter);
    lTx->setMargin(2);

    lRx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
    lRx->setFrameStyle(QFrame::Box);
    lRx->setAlignment(Qt::AlignCenter);
    lRx->setMargin(2);

    itsBlinkTimeTxNone->setInterval(BLINKTIMETX);
    itsBlinkTimeRxNone->setInterval(BLINKTIMERX);
    itsBlinkTimeTxColor->setInterval(BLINKTIMETX);
    itsBlinkTimeRxColor->setInterval(BLINKTIMERX);
    itsTimeToDisplay->setInterval(DISPLAYTIME);

    QMap<QString, QPen> curves;
    curves.insert("TEMP", QPen(QBrush(QColor("#0000FF")), 1.5));
    curves.insert("SENS1", QPen(QBrush(QColor("#FF0000")), 1.5));
    curves.insert("SENS2", QPen(QBrush(QColor("#00FF00")), 1.5));
    setCurves(curves);

    setupGUI();
    setupConnections();
}

PlotterDialog::~PlotterDialog()
{
    itsPort->close();
}

void PlotterDialog::setCurves(const QMap<QString, QPen > &curves)
{
    QStringList listKeys = curves.keys();

    for(int i = 0; i < listKeys.size(); ++i) {
        m_Curves.append(new QwtPlotCurve);
        m_Curves[i]->setRenderHint( QwtPlotItem::RenderAntialiased );
        m_Curves[i]->setLegendAttribute( QwtPlotCurve::LegendShowLine );
        m_Curves[i]->setYAxis( QwtPlot::yLeft );
        m_Curves[i]->setXAxis( QwtPlot::xBottom );
        m_Curves[i]->setTitle( listKeys.at(i) );
        m_Curves[i]->setPen( curves.value( listKeys.at(i) ) );
        m_Curves[i]->attach(m_plot);
    }
}

void PlotterDialog::autoScroll(const double &elapsedTime)
{
    if( elapsedTime > dynamic_cast<QLCDNumber *>(m_lcdTimeInterval->spinWidget())->value() * XDIVISION
            && m_isRessumed ) {
        toCurrentTime();
        updatePlot();
    }
}

double PlotterDialog::roundToStep(const double &value, const double &step)
{
    int tempValue = static_cast<int>( value / step );

    if( value > static_cast<double>( tempValue ) * step ) {
        return step * static_cast<double>( ( tempValue + 1 ) );
    } else {
        return step * static_cast<double>( tempValue );
    }
}

void PlotterDialog::appendData(const QMap<QString, double> &curvesData)
{
    if(m_isReseted) {
        m_currentTime->restart();
        m_isReseted = false;
    }

    QStringList listKeys = curvesData.keys();
    if( m_currentTime->isNull() ) {
        m_currentTime->start();

        for(int i = 0; i < listKeys.size(); ++i) {
            QVector<double> data;
            m_dataAxises.push_back(data);
        }
    }

    double elapsedTime = static_cast<double>( m_currentTime->elapsed() ) / 1000; // sec

    // keeping vectors sizes constant
    if( elapsedTime > 2 * 60 * XDIVISION ) {
        m_timeAxis.pop_front();
        for( int i = 0; i < m_dataAxises.size(); ++i ) {
            m_dataAxises[i].pop_front();
        }
    }

    autoScroll(elapsedTime);

    m_timeAxis.push_back( elapsedTime );
    for(int i = 0; i < listKeys.size(); ++i) {
        if( listKeys.contains( m_Curves.at(i)->title().text() ) ) { // protection from errored inputing data
            m_dataAxises[i].push_back( curvesData.value( m_Curves.at(i)->title().text() ) );

            if(m_isRessumed) {
                m_Curves[i]->setSamples( m_timeAxis, m_dataAxises.at(i) );
                m_prevCurrentTime = m_timeAxis.last();
            }
        }
    }
}

void PlotterDialog::updatePlot()
{
    m_plot->replot();
}

void PlotterDialog::setupGUI()
{
    dynamic_cast<QPushButton *>( sbSetTemp->buttonUpWidget() )->setMaximumSize(20, 20);
    dynamic_cast<QPushButton *>( sbSetTemp->buttonDownWidget() )->setMaximumSize(20, 20);

    dynamic_cast<QPushButton *>( m_lcdTempInterval->buttonUpWidget() )->setMaximumSize(20, 20);
    dynamic_cast<QPushButton *>( m_lcdTempInterval->buttonDownWidget() )->setMaximumSize(20, 20);

    dynamic_cast<QPushButton *>( m_lcdTimeInterval->buttonUpWidget() )->setMaximumSize(20, 20);
    dynamic_cast<QPushButton *>( m_lcdTimeInterval->buttonDownWidget() )->setMaximumSize(20, 20);

    dynamic_cast<QPushButton *>( m_msbTempInterval->buttonUpWidget() )->setMaximumSize(20, 20);
    dynamic_cast<QPushButton *>( m_msbTempInterval->buttonDownWidget() )->setMaximumSize(20, 20);

    dynamic_cast<QPushButton *>( m_msbTimeInterval->buttonUpWidget() )->setMaximumSize(20, 20);
    dynamic_cast<QPushButton *>( m_msbTimeInterval->buttonDownWidget() )->setMaximumSize(20, 20);

    bSetTemp->setFixedHeight(45);
    QHBoxLayout *setTempLayout0 = new QHBoxLayout;
    setTempLayout0->addWidget(sbSetTemp);
    setTempLayout0->addWidget(bSetTemp);
    setTempLayout0->setSpacing(5);

    QVBoxLayout *setTempLayout = new QVBoxLayout;
    setTempLayout->addItem(setTempLayout0);
    setTempLayout->addWidget(chbSynchronize);
    setTempLayout->setSpacing(5);


    QGridLayout *gridInfo = new QGridLayout;
    QLabel *lInstalled = new QLabel("Installed, °C:", this);
    lInstalled->setStyleSheet("color: #0000FF; font: bold; font-size: 12pt");
    gridInfo->addWidget(lInstalled, 0, 0);
    gridInfo->addWidget(lcdInstalledTemp, 0, 1);
    QLabel *lSensor1 = new QLabel(QString::fromUtf8("Sensor 1, °C:"));
    lSensor1->setStyleSheet("color: #FF0000; font: bold; font-size: 12pt");
    gridInfo->addWidget(lSensor1, 1, 0);
    gridInfo->addWidget(lcdSensor1Termo, 1, 1);
    QLabel *lSensor2 = new QLabel(QString::fromUtf8("Sensor 2, °C:"));
    lSensor2->setStyleSheet("color: #00FF00; font: bold; font-size: 12pt");
    gridInfo->addWidget(lSensor2, 2, 0);
    gridInfo->addWidget(lcdSensor2Termo, 2, 1);
    gridInfo->setSpacing(5);

    gbSetTemp->setLayout(setTempLayout);
    gbSensors->setLayout(gridInfo);

    QGridLayout *grid = new QGridLayout;
    grid->addWidget(lPort, 0, 0);
    grid->addWidget(cbPort, 0, 1);
    grid->addWidget(lBaud, 0, 2);
    grid->addWidget(cbBaud, 0, 3);
    // пещаю логотип фирмы
//    grid->addWidget(new QLabel("<img src=':/Resources/elisat.png' height='40' width='150'/>", this), 0, 7, 2, 5, Qt::AlignRight);
    grid->addWidget(bPortStart, 0, 4);
    grid->addWidget(bPortStop, 0, 5);
    grid->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding), 0, 6);
    grid->addWidget(lTx, 0, 7);
    grid->addWidget(lRx, 0, 8);
    grid->setSpacing(5);

    QHBoxLayout *timeLayout0 = new QHBoxLayout;
    timeLayout0->addWidget(m_lcdTimeInterval);
    timeLayout0->addWidget(m_msbTimeInterval);
    timeLayout0->setSpacing(5);

    QVBoxLayout *timeLayout = new QVBoxLayout;
    timeLayout->addItem(timeLayout0);
    timeLayout->addWidget(m_cbTimeAccurate);
    timeLayout->setSpacing(5);

    QGroupBox *gbTime = new QGroupBox("sec/div", this);
    gbTime->setLayout(timeLayout);

    QHBoxLayout *tempLayout0 = new QHBoxLayout;
    tempLayout0->addWidget(m_lcdTempInterval);
    tempLayout0->addWidget(m_msbTempInterval);
    tempLayout0->setSpacing(5);

    QVBoxLayout *tempLayout = new QVBoxLayout;
    tempLayout->addItem(tempLayout0);
    tempLayout->addWidget(m_cbTempAccurate);
    tempLayout->setSpacing(5);

    QGroupBox *gbTemp = new QGroupBox("°C/div", this);
    gbTemp->setLayout(tempLayout);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(m_bReset);
    buttonsLayout->addWidget(m_bPauseRessume);
    buttonsLayout->setSpacing(5);

    QVBoxLayout *knobsLayout = new QVBoxLayout;
    knobsLayout->addWidget(gbTime);
    knobsLayout->addWidget(gbTemp);
    knobsLayout->addItem(buttonsLayout);
    knobsLayout->addWidget(gbSetTemp);
    knobsLayout->addWidget(gbSensors);
    knobsLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    knobsLayout->setSpacing(5);

    QHBoxLayout *plotLayout = new QHBoxLayout;
    plotLayout->addWidget(m_plot, 1);
    plotLayout->addItem(knobsLayout);
    plotLayout->setSpacing(5);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addItem(plotLayout);
    mainLayout->addItem(grid);
    mainLayout->setSpacing(5);

    setLayout(mainLayout);
}

void PlotterDialog::lcdStyling(QList<QLCDNumber *> &lcdList)
{
    foreach(QLCDNumber *lcd, lcdList) {
        lcd->setSegmentStyle(QLCDNumber::Flat);
        lcd->setFrameStyle(QFrame::NoFrame);
    }
}

void PlotterDialog::changeTimeInterval()
{
    toCurrentTime();
    updatePlot();
}

void PlotterDialog::changeTempInterval()
{
    double factor = dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value();
    m_plot->setAxisScale( QwtPlot::yLeft,
                          m_prevCentralTemp - factor * YDIVISION/2,
                          m_prevCentralTemp + factor * YDIVISION/2,
                          factor * YSCALESTEP );
    updatePlot();
}

void PlotterDialog::moveTimeInterval()
{
    double factor = dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())->value();
    if( m_prevCurrentTime + factor * ( m_msbTimeInterval->value() - XDIVISION ) < 0 ) {
        m_plot->setAxisScale( QwtPlot::xBottom,
                              0,
                              factor * XDIVISION,
                              factor * XSCALESTEP );
        m_msbTimeInterval->setValue( m_msbTimeInterval->value() + m_msbTimeInterval->step() );
    } else {
        m_plot->setAxisScale( QwtPlot::xBottom,
                              roundToStep( m_prevCurrentTime, factor * XSCALESTEP )
                              + factor * ( m_msbTimeInterval->value() - XDIVISION ),
                              roundToStep( m_prevCurrentTime, factor * XSCALESTEP )
                              + factor * m_msbTimeInterval->value(),
                              factor * XSCALESTEP );
    }

    updatePlot();
}

void PlotterDialog::moveTempInterval()
{
    double factor = dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value();
    double offset = 0.0;
    if( m_prevTempOffset - m_msbTempInterval->value() > 0 ) {
        offset = -m_msbTempInterval->step();
    } else {
        offset = m_msbTempInterval->step();
    }
    m_prevTempOffset = m_msbTempInterval->value();

    m_plot->setAxisScale( QwtPlot::yLeft,
                          m_prevCentralTemp + factor * ( offset - YDIVISION/2 ) ,
                          m_prevCentralTemp + factor * ( offset + YDIVISION/2 ),
                          factor * YSCALESTEP );

    m_prevCentralTemp += factor * offset;

    qDebug() << m_prevCentralTemp;
    qDebug() << m_msbTempInterval->value() << "\n";
    updatePlot();
}

void PlotterDialog::changeTimeAccurateFactor(bool isChecked)
{
    if(isChecked) {
        m_TimeAccurateFactor *= 0.1;
    } else {
        m_TimeAccurateFactor *= 10;
    }
    m_msbTimeInterval->setRange(LOWTIME,
                                UPTIME,
                                m_TimeAccurateFactor * STEPTIME);
}

void PlotterDialog::changeTempAccurateFactor(bool isChecked)
{
    if(isChecked) {
        m_TempAccurateFactor *= 0.1;
    } else {
        m_TempAccurateFactor *= 10;
    }
    m_msbTempInterval->setRange(LOWTEMP,
                                UPTEMP,
                                m_TempAccurateFactor * STEPTEMP);
}

void PlotterDialog::resetTime()
{
    m_isReseted = true;

    m_timeAxis.clear();
    m_dataAxises.clear();

    for(int i = 0; i < m_Curves.size(); ++i) {
        QVector<double> data;
        m_dataAxises.push_back(data);
    }

    updatePlot();
}

void PlotterDialog::pauseRessume()
{
    m_isReseted = false;

    if(m_bPauseRessume->text() == QString::fromUtf8("Pause")) {
        m_bPauseRessume->setText("Ressume");
        m_isRessumed = false;
        dynamic_cast<QPushButton *>( m_msbTimeInterval->buttonUpWidget() )->setEnabled( true );
        dynamic_cast<QPushButton *>( m_msbTimeInterval->buttonDownWidget() )->setEnabled( true );
        m_cbTimeAccurate->setEnabled( true );
    } else {
        m_bPauseRessume->setText("Pause");
        m_isRessumed = true;
        dynamic_cast<QPushButton *>( m_msbTimeInterval->buttonUpWidget() )->setEnabled( false );
        dynamic_cast<QPushButton *>( m_msbTimeInterval->buttonDownWidget() )->setEnabled( false );
        m_cbTimeAccurate->setEnabled( false );
    }
}

void PlotterDialog::toCurrentTime()
{
    double factor = dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())->value();
    if(m_isRessumed) {
        m_prevCurrentTime = m_timeAxis.last();
    }

    if(m_prevCurrentTime - factor * XDIVISION > 0) {
        m_plot->setAxisScale( QwtPlot::xBottom,
                              roundToStep(m_prevCurrentTime, factor * XSCALESTEP )
                              - factor * XDIVISION,
                              roundToStep(m_prevCurrentTime, factor * XSCALESTEP ),
                              factor * XSCALESTEP );
    } else {
        m_plot->setAxisScale( QwtPlot::xBottom,
                              0,
                              factor * XDIVISION,
                              factor * XSCALESTEP );
    }
    updatePlot();
}

void PlotterDialog::setupConnections()
{
    connect(m_lcdTimeInterval, SIGNAL(valueChanged()), this, SLOT(changeTimeInterval()));
    connect(m_lcdTempInterval, SIGNAL(valueChanged()), this, SLOT(changeTempInterval()));
    connect(m_msbTimeInterval, SIGNAL(valueChanged()), this, SLOT(moveTimeInterval()));
    connect(m_msbTempInterval, SIGNAL(valueChanged()), this, SLOT(moveTempInterval()));

    connect(m_cbTimeAccurate, SIGNAL(clicked(bool)), this, SLOT(changeTimeAccurateFactor(bool)));
    connect(m_cbTempAccurate, SIGNAL(clicked(bool)), this, SLOT(changeTempAccurateFactor(bool)));

    connect(m_bReset, SIGNAL(clicked()), this, SLOT(resetTime()));
    connect(m_bPauseRessume, SIGNAL(clicked()), this, SLOT(pauseRessume()));

    connect(bPortStart, SIGNAL(clicked()), this, SLOT(openPort()));
    connect(bPortStop, SIGNAL(clicked()), this, SLOT(closePort()));
    connect(cbPort, SIGNAL(currentIndexChanged(int)), this, SLOT(closePort()));
    connect(cbBaud, SIGNAL(currentIndexChanged(int)), this, SLOT(closePort()));
    connect(itsProtocol, SIGNAL(DataIsReaded(bool)), this, SLOT(received(bool)));
    connect(bSetTemp, SIGNAL(clicked()), this, SLOT(writeTemp()));
    connect(itsBlinkTimeTxColor, SIGNAL(timeout()), this, SLOT(colorIsTx()));
    connect(itsBlinkTimeRxColor, SIGNAL(timeout()), this, SLOT(colorIsRx()));
    connect(itsBlinkTimeTxNone, SIGNAL(timeout()), this, SLOT(colorTxNone()));
    connect(itsBlinkTimeRxNone, SIGNAL(timeout()), this, SLOT(colorRxNone()));
    connect(itsTimeToDisplay, SIGNAL(timeout()), this, SLOT(display()));

    connect(sbSetTemp, SIGNAL(valueChanged()), this, SLOT(colorSetTempLCD()));

    QShortcut *aboutShortcut = new QShortcut(QKeySequence("F1"), this);
    connect(aboutShortcut, SIGNAL(activated()), qApp, SLOT(aboutQt()));
}

void PlotterDialog::openPort()
{
    itsPort->close();
    itsPort->setPortName(cbPort->currentText());

    if(itsPort->open(QSerialPort::ReadWrite))
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

        bPortStart->setEnabled(false);
        bPortStop->setEnabled(true);
        lTx->setStyleSheet("background: none; font: bold; font-size: 10pt");
        lRx->setStyleSheet("background: none; font: bold; font-size: 10pt");
    }
    else
    {
        lTx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
        lRx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
    }
}

void PlotterDialog::closePort()
{
    itsPort->close();
    itsBlinkTimeTxNone->stop();
    itsBlinkTimeTxColor->stop();
    itsBlinkTimeRxNone->stop();
    itsBlinkTimeRxColor->stop();
    lTx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
    lRx->setStyleSheet("background: yellow; font: bold; font-size: 10pt");
    bPortStop->setEnabled(false);
    bPortStart->setEnabled(true);
    itsProtocol->resetProtocol();
}

void PlotterDialog::received(bool isReceived)
{
    if(isReceived) {
        if(!itsBlinkTimeRxColor->isActive() && !itsBlinkTimeRxNone->isActive()) {
            itsBlinkTimeRxColor->start();
            lRx->setStyleSheet("background: green; font: bold; font-size: 10pt");
        }

        if(!itsTimeToDisplay->isActive()) {
            itsTimeToDisplay->start();
        }

        QList<QString> strKeysList = itsProtocol->getReadedData().keys();
        QMap<QString, double> curvesData;
        for(int i = 0; i < itsProtocol->getReadedData().size(); ++i) {
            itsSensorsList.append(itsProtocol->getReadedData().value(strKeysList.at(i)));
            curvesData.insert(strKeysList.at(i), itsSensorsList.at(i).toDouble());
        }
        appendData(curvesData);
    }
}

void PlotterDialog::writeTemp()
{
    if(itsPort->isOpen()) {
        QMultiMap<QString, QString> dataTemp;

        if(!itsBlinkTimeTxColor->isActive() && !itsBlinkTimeTxNone->isActive()) {
            itsBlinkTimeTxColor->start();
            lTx->setStyleSheet("background: red; font: bold; font-size: 10pt");
        }

        dataTemp.insert("CODE", "0");
        dataTemp.insert("TEMP", QString::number(static_cast<int>(sbSetTemp->value())));
        itsProtocol->setDataToWrite(dataTemp);
        itsProtocol->writeData();
        if(chbSynchronize->isChecked()) {
            resetTime();
            toCurrentTime();
        }
    }
}

void PlotterDialog::colorTxNone()
{
    itsBlinkTimeTxNone->stop();
}

void PlotterDialog::setColorLCD(QLCDNumber *lcd, bool isHeat)
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

QString &PlotterDialog::addTrailingZeros(QString &str, int prec)
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
        int size = str.size();
        for(int i = 0; i < prec - (size - 1 - pointIndex); ++i) {
            str.append("0");
        }
    }

    return str;
}

void PlotterDialog::colorIsRx()
{
    lRx->setStyleSheet("background: none; font: bold; font-size: 10pt");
    itsBlinkTimeRxColor->stop();
    itsBlinkTimeRxNone->start();
}

void PlotterDialog::colorRxNone()
{
    itsBlinkTimeRxNone->stop();
}

void PlotterDialog::colorIsTx()
{
    lTx->setStyleSheet("background: none; font: bold; font-size: 10pt");
    itsBlinkTimeTxColor->stop();
    itsBlinkTimeTxNone->start();
}

void PlotterDialog::display()
{
    itsTimeToDisplay->stop();

    QList<QLCDNumber*> list;
    list << lcdInstalledTemp << lcdSensor2Termo << lcdSensor1Termo;
    QString tempStr;

    for(int k = 0; k < list.size() && k < itsSensorsList.size(); ++k) {
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
    updatePlot();
}

void PlotterDialog::colorSetTempLCD()
{
    setColorLCD(dynamic_cast<QLCDNumber*>(sbSetTemp->spinWidget()), sbSetTemp->value() > 0);
}
