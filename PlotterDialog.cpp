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
#include <qwt_picker_machine.h>
#include <QApplication>
#include <QFile>
#include <QDesktopWidget>
#include <QShortcut>
#include <QSerialPortInfo>
#include <QPalette>
#include <QIcon>
#include <qwt_scale_widget.h>
#include <qwt_text_label.h>
#include <QInputDialog>
#include <QGraphicsTextItem>
//#include <QFontMetricsF>
//#include <qwt_widget_overlay.h>

#define STARTBYTE 0x55
#define STOPBYTE 0xAA
#define BYTESLENGTH 8

#define PRECISION 2

#define BLINKTIMETX 200 // ms
#define BLINKTIMERX 500 // ms
#define DISPLAYTIME 100 // ms

#define TEMPRANGE_MIN -70 // degrees Celsius
#define TEMPRANGE_MAX 70 // degrees Celsius
#define TEMPSTEP 1 // degrees Celsius
#define NORMAL_TEMP 35 // degrees Celsius

#define XDIVISION 10
#define XMAJORDIVISION 10
#define XMINORDIVISION 10
#define XSCALESTEP 1

#define YDIVISION 8
#define YMAJORDIVISION 8
#define YMINORDIVISION 10
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
    m_cbTempAccurateLeft(new QCheckBox(QString::fromUtf8("Accurate (x0.1)"), this)),
    m_cbTempAccurateRight(new QCheckBox(QString::fromUtf8("Accurate (x0.1)"), this)),
    m_bReset(new QPushButton(QString::fromUtf8("Reset"), this)),
    m_bPauseRessume(new QPushButton(QString::fromUtf8("Pause"), this)),
    m_TimeAccurateFactor(1.0),
    m_TempAccurateFactorLeft(1.0),
    m_TempAccurateFactorRight(1.0),
    m_plot(new QwtPlot(this)),
    m_currentTime( new QTime()),
    m_isReseted( false ),
    m_isRessumed( true ),
    m_prevCurrentTime( 0.0 ),
    m_prevCentralTempLeft( 0.0 ),
    m_prevTempOffsetLeft( 0.0 ),
    m_prevCentralTempRight( 0.0 ),
    m_prevTempOffsetRight( 0.0 ),
    m_rbRelateLeft(new QRadioButton(this)),
    m_rbRelateRight(new QRadioButton(this)),
    m_bgRelate(new QButtonGroup(this)),
    m_bArrow(new QPushButton(QIcon(":/Resources/arrow.png"), QString::null, this)),
    m_bEditNotes(new QPushButton(QIcon(":/Resources/editText.png"), QString::null, this)),
    m_bMoveNotes(new QPushButton(QIcon(":/Resources/moveText.png"), QString::null, this)),
    m_bDeleteNotes(new QPushButton(QIcon(":/Resources/deleteText.png"), QString::null, this)),
    m_bgNotes(new QButtonGroup(this)),
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
    itsPort(new QSerialPort(this)),
    itsComPort(new ComPort(itsPort, STARTBYTE, STOPBYTE, BYTESLENGTH, false, this)),
    itsProtocol(new NGProtocol(itsComPort, this)),
    itsBlinkTimeTxNone(new QTimer(this)),
    itsBlinkTimeRxNone(new QTimer(this)),
    itsBlinkTimeTxColor(new QTimer(this)),
    itsBlinkTimeRxColor(new QTimer(this)),
    itsTimeToDisplay(new QTimer(this)),
    m_notesDialog(new NotesDialog(this)),
    m_prevNotesFont(QFont("Arial", 12)),
    m_prevNotesColor(QColor(Qt::black))
{
    QVector<double> timeSamples;
    timeSamples << 1 << 2 << 5 << 10 << 20 << 50 << 100;

    QVector<double> tempSamples;
    tempSamples << 0.5 << 1 << 2 << 5 << 10 << 20;

    m_lcdTimeInterval = new LCDSampleSpinBox(timeSamples,
                                             QIcon(":/Resources/down.png"),
                                             QIcon(":/Resources/up.png"),
                                             QString::fromUtf8(""),
                                             QString::fromUtf8(""),
                                             QLCDNumber::Dec,
                                             LCDSpinBox::RIGHT,
                                             this);
    m_lcdTimeInterval->setValue(5);

    m_lcdTempIntervalLeft = new LCDSampleSpinBox(tempSamples,
                                                 QIcon(":/Resources/down.png"),
                                                 QIcon(":/Resources/up.png"),
                                                 QString::fromUtf8(""),
                                                 QString::fromUtf8(""),
                                                 QLCDNumber::Dec,
                                                 LCDSpinBox::RIGHT,
                                                 this);
    m_lcdTempIntervalLeft->setValue(0);

    m_lcdTempIntervalRight = new LCDSampleSpinBox(tempSamples,
                                                  QIcon(":/Resources/down.png"),
                                                  QIcon(":/Resources/up.png"),
                                                  QString::fromUtf8(""),
                                                  QString::fromUtf8(""),
                                                  QLCDNumber::Dec,
                                                  LCDSpinBox::RIGHT,
                                                  this);
    m_lcdTempIntervalRight->setValue(0);

    m_msbTimeInterval = new MoveSpinBox("<img src=':Resources/LeftRight.png' height='20' width='45'/>",
                                        QIcon(":/Resources/left.png"),
                                        QIcon(":/Resources/right.png"),
                                        QString::fromUtf8(""),
                                        QString::fromUtf8(""),
                                        MoveSpinBox::BOTTOM,
                                        this);
    m_msbTimeInterval->setRange(LOWTIME, UPTIME, STEPTIME);

    m_msbTempIntervalLeft = new MoveSpinBox("<img src=':Resources/UpDown.png' height='45' width='20'/>",
                                            QIcon(":/Resources/down.png"),
                                            QIcon(":/Resources/up.png"),
                                            QString::fromUtf8(""),
                                            QString::fromUtf8(""),
                                            MoveSpinBox::RIGHT,
                                            this);
    m_msbTempIntervalLeft->setRange(LOWTEMP, UPTEMP, STEPTEMP);

    m_msbTempIntervalRight = new MoveSpinBox("<img src=':Resources/UpDown.png' height='45' width='20'/>",
                                             QIcon(":/Resources/down.png"),
                                             QIcon(":/Resources/up.png"),
                                             QString::fromUtf8(""),
                                             QString::fromUtf8(""),
                                             MoveSpinBox::RIGHT,
                                             this);
    m_msbTempIntervalRight->setRange(LOWTEMP, UPTEMP, STEPTEMP);

    setWindowTitle(title);

    QList<QLCDNumber*> lcdList;
    lcdList << dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())
            << dynamic_cast<QLCDNumber*>(m_lcdTempIntervalLeft->spinWidget())
            << dynamic_cast<QLCDNumber*>(m_lcdTempIntervalRight->spinWidget())
            << lcdInstalledTemp << lcdSensor1Termo << lcdSensor2Termo
            << dynamic_cast<QLCDNumber*>(sbSetTemp->spinWidget());
    lcdStyling(lcdList);
    setColorLCD(dynamic_cast<QLCDNumber*>(m_lcdTempIntervalLeft->spinWidget()), QColor("#FF0000"));
    setColorLCD(dynamic_cast<QLCDNumber*>(m_lcdTempIntervalRight->spinWidget()), QColor("green"));

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

    // grid
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableXMin( true );
    grid->enableYMin( true );
    grid->setMajorPen( Qt::black, 0 );
    grid->setMinorPen( Qt::gray, 0, Qt::DotLine );
    grid->attach( m_plot );

    // axes
    // Time
    m_plot->setAxisTitle( QwtPlot::xBottom, "Time, sec" );
    m_plot->setAxisScale( QwtPlot::xBottom,
                          0,
                          dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())->value() * XDIVISION,
                          dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())->value() * XSCALESTEP );
    m_plot->setAxisMaxMajor( QwtPlot::xBottom, XMAJORDIVISION );
    m_plot->setAxisMaxMinor( QwtPlot::xBottom, XMINORDIVISION );
    m_plot->setAxisAutoScale( QwtPlot::xBottom, false );

    // Temperature Left
    m_plot->setAxisTitle( QwtPlot::yLeft, "Sensor 1, °C" );
    m_plot->setAxisScale( QwtPlot::yLeft,
                          - dynamic_cast<QLCDNumber*>(m_lcdTempIntervalLeft->spinWidget())->value() * YDIVISION/2,
                          dynamic_cast<QLCDNumber*>(m_lcdTempIntervalLeft->spinWidget())->value() * YDIVISION/2,
                          dynamic_cast<QLCDNumber*>(m_lcdTempIntervalLeft->spinWidget())->value() * YSCALESTEP );
    m_plot->setAxisMaxMajor( QwtPlot::yLeft, YMAJORDIVISION );
    m_plot->setAxisMaxMinor( QwtPlot::yLeft, YMINORDIVISION );
    m_plot->setAxisAutoScale( QwtPlot::yLeft, false );

    // Temperature Right
    m_plot->enableAxis(QwtPlot::yRight);
    m_plot->setAxisTitle( QwtPlot::yRight, "Sensor 2, °C" );
    m_plot->setAxisScale( QwtPlot::yRight,
                          - dynamic_cast<QLCDNumber*>(m_lcdTempIntervalRight->spinWidget())->value() * YDIVISION/2,
                          dynamic_cast<QLCDNumber*>(m_lcdTempIntervalRight->spinWidget())->value() * YDIVISION/2,
                          dynamic_cast<QLCDNumber*>(m_lcdTempIntervalRight->spinWidget())->value() * YSCALESTEP );
    m_plot->setAxisMaxMajor( QwtPlot::yRight, YMAJORDIVISION );
    m_plot->setAxisMaxMinor( QwtPlot::yRight, YMINORDIVISION );
    m_plot->setAxisAutoScale( QwtPlot::yRight, false );

    // Colored Temperature Axis
    QPalette yLeftPalette = m_plot->axisWidget(QwtPlot::yLeft)->palette();
    yLeftPalette.setColor( QPalette::WindowText, QColor("#FF0000")); // for ticks
    yLeftPalette.setColor( QPalette::Text, QColor("#FF0000")); // for ticks' labels
    m_plot->axisWidget(QwtPlot::yLeft)->setPalette( yLeftPalette );

    QPalette yRightPalette = m_plot->axisWidget(QwtPlot::yRight)->palette();
    yRightPalette.setColor( QPalette::WindowText, QColor("green")); // for ticks
    yRightPalette.setColor( QPalette::Text, QColor("green")); // for ticks' labels
    m_plot->axisWidget(QwtPlot::yRight)->setPalette( yRightPalette );

    m_plot->setAutoReplot( false );

    // pickers
    m_pickerLeft = new QwtPlotPicker(QwtPlot::xBottom,
                                     QwtPlot::yLeft,
                                     QwtPlotPicker::CrossRubberBand,
                                     QwtPicker::ActiveOnly,
                                     m_plot->canvas());
    m_pickerLeft->setStateMachine(new QwtPickerDragPointMachine());
    m_pickerLeft->setRubberBandPen(QColor("#FF0000"));
    m_pickerLeft->setTrackerPen(QColor(Qt::black));
    m_pickerLeft->setTrackerFont(QFont(m_pickerLeft->trackerFont().family(), 12));
    m_pickerLeft->setMousePattern(QwtPicker::MouseSelect1, Qt::LeftButton);

    m_pickerRight = new QwtPlotPicker(QwtPlot::xBottom,
                                      QwtPlot::yRight,
                                      QwtPlotPicker::CrossRubberBand,
                                      QwtPicker::ActiveOnly,
                                      m_plot->canvas());
    m_pickerRight->setStateMachine(new QwtPickerDragPointMachine());
    m_pickerRight->setRubberBandPen(QColor(Qt::green));
    m_pickerRight->setTrackerPen(QColor(Qt::black));
    m_pickerRight->setTrackerFont(QFont(m_pickerRight->trackerFont().family(), 12));
    m_pickerRight->setMousePattern(QwtPicker::MouseSelect1, Qt::RightButton);

    // pickers left axis notes manipulate
    m_pickerNoteEditLeft = new QwtPlotPicker(QwtPlot::xBottom,
                                             QwtPlot::yLeft,
                                             QwtPlotPicker::NoRubberBand,
                                             QwtPicker::AlwaysOff,
                                             m_plot->canvas());
    m_pickerNoteEditLeft->setStateMachine(new QwtPickerClickPointMachine);
    m_pickerNoteEditLeft->setMousePattern(QwtPicker::MouseSelect1, Qt::LeftButton);

    m_pickerNoteMoveLeft = new QwtPlotPicker(QwtPlot::xBottom,
                                             QwtPlot::yLeft,
                                             QwtPlotPicker::NoRubberBand,
                                             QwtPicker::AlwaysOff,
                                             m_plot->canvas());
    m_pickerNoteMoveLeft->setStateMachine(new QwtPickerDragPointMachine());
    m_pickerNoteMoveLeft->setMousePattern(QwtPicker::MouseSelect1, Qt::LeftButton);

    m_pickerNoteDeleteLeft = new QwtPlotPicker(QwtPlot::xBottom,
                                               QwtPlot::yLeft,
                                               QwtPlotPicker::NoRubberBand,
                                               QwtPicker::AlwaysOff,
                                               m_plot->canvas());
    m_pickerNoteDeleteLeft->setStateMachine(new QwtPickerClickPointMachine);
    m_pickerNoteDeleteLeft->setMousePattern(QwtPicker::MouseSelect1, Qt::LeftButton);

    // pickers right axis notes manipulate
    m_pickerNoteEditRight = new QwtPlotPicker(QwtPlot::xBottom,
                                              QwtPlot::yRight,
                                              QwtPlotPicker::NoRubberBand,
                                              QwtPicker::AlwaysOff,
                                              m_plot->canvas());
    m_pickerNoteEditRight->setStateMachine(new QwtPickerClickPointMachine);
    m_pickerNoteEditRight->setMousePattern(QwtPicker::MouseSelect1, Qt::RightButton);

    m_pickerNoteMoveRight = new QwtPlotPicker(QwtPlot::xBottom,
                                              QwtPlot::yRight,
                                              QwtPlotPicker::NoRubberBand,
                                              QwtPicker::AlwaysOff,
                                              m_plot->canvas());
    m_pickerNoteMoveRight->setStateMachine(new QwtPickerDragPointMachine());
    m_pickerNoteMoveRight->setMousePattern(QwtPicker::MouseSelect1, Qt::RightButton);

    m_pickerNoteDeleteRight = new QwtPlotPicker(QwtPlot::xBottom,
                                                QwtPlot::yRight,
                                                QwtPlotPicker::NoRubberBand,
                                                QwtPicker::AlwaysOff,
                                                m_plot->canvas());
    m_pickerNoteDeleteRight->setStateMachine(new QwtPickerClickPointMachine);
    m_pickerNoteDeleteRight->setMousePattern(QwtPicker::MouseSelect1, Qt::RightButton);

    dynamic_cast<QPushButton *>( m_msbTimeInterval->buttonUpWidget() )->setEnabled( false );
    dynamic_cast<QPushButton *>( m_msbTimeInterval->buttonDownWidget() )->setEnabled( false );
    m_cbTimeAccurate->setEnabled( false );

    // Radiobuttons config
    m_bgRelate->addButton(m_rbRelateLeft);
    m_bgRelate->addButton(m_rbRelateRight);
    m_bgRelate->setId(m_rbRelateLeft, 1);
    m_bgRelate->setId(m_rbRelateRight, 2);
    m_bgRelate->setExclusive(true);
    m_rbRelateLeft->setChecked(true);

    // Notes buttons config
    m_bgNotes->addButton(m_bArrow);
    m_bgNotes->addButton(m_bEditNotes);
    m_bgNotes->addButton(m_bMoveNotes);
    m_bgNotes->addButton(m_bDeleteNotes);
    m_bgNotes->setId(m_bArrow, 0);
    m_bgNotes->setId(m_bEditNotes, 1);
    m_bgNotes->setId(m_bMoveNotes, 2);
    m_bgNotes->setId(m_bDeleteNotes, 3);
    m_bgNotes->setExclusive(true);
    m_bArrow->setCheckable(true);
    m_bArrow->setChecked(true);
    arrow(true);
    m_bEditNotes->setCheckable(true);
    m_bMoveNotes->setCheckable(true);
    m_bDeleteNotes->setCheckable(true);

    QStringList portsNames;

    foreach(QSerialPortInfo portsAvailable, QSerialPortInfo::availablePorts())
    {
        portsNames << portsAvailable.portName();
    }

    cbPort->addItems(portsNames);
#if defined (Q_OS_LINUX)
    cbPort->setEditable(true); // TODO Make correct viewing available virtual ports in Linux
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
    QMap<QString, QwtPlot::Axis> axis;
    curves.insert("TEMP", QPen(QBrush(QColor("#0000FF")), 1.5));
    axis.insert("TEMP", QwtPlot::yLeft);
    curves.insert("SENS1", QPen(QBrush(QColor("#FF0000")), 1.5));
    axis.insert("SENS1", QwtPlot::yLeft);
    curves.insert("SENS2", QPen(QBrush(QColor("green")), 1.5));
    axis.insert("SENS2", QwtPlot::yRight);
    setCurves(curves, axis);

    setupGUI();
    setupConnections();
}

PlotterDialog::~PlotterDialog()
{
    itsPort->close();
}

void PlotterDialog::setCurves(const QMap<QString, QPen > &curves,
                              const QMap<QString, QwtPlot::Axis> &axis)
{
    int size = qMin(curves.size(), axis.size());

    QStringList curvesListKeys = curves.keys();
    QStringList axisListKeys = axis.keys();

    for(int i = 0; i < size; ++i) {
        m_Curves.append(new QwtPlotCurve);
        m_Curves[i]->setRenderHint( QwtPlotItem::RenderAntialiased );
        m_Curves[i]->setLegendAttribute( QwtPlotCurve::LegendShowLine );
        m_Curves[i]->setYAxis( axis.value( axisListKeys.at(i) ) );
        m_Curves[i]->setXAxis( QwtPlot::xBottom );
        m_Curves[i]->setTitle( curvesListKeys.at(i) );
        m_Curves[i]->setPen( curves.value( curvesListKeys.at(i) ) );
        m_Curves[i]->attach(m_plot);
    }
}

void PlotterDialog::autoScroll(const double &elapsedTime)
{
    double timeFactor = dynamic_cast<QLCDNumber *>(m_lcdTimeInterval->spinWidget())->value();
    if( elapsedTime > timeFactor * XDIVISION && m_isRessumed ) {
        toCurrentTime();
        updatePlot();
    }

    double tempFactorLeft = dynamic_cast<QLCDNumber *>( m_lcdTempIntervalLeft->spinWidget() )->value();
    double tempFactorRight = dynamic_cast<QLCDNumber *>( m_lcdTempIntervalRight->spinWidget() )->value();

    // TODO Make it universal
    if( m_dataAxises.value(QString::fromUtf8("SENS1")).last() > m_prevCentralTempLeft + tempFactorLeft * YDIVISION / 2 ) {
        m_msbTempIntervalLeft->upStep();
    }

    if( m_dataAxises.value(QString::fromUtf8("SENS1")).last() < m_prevCentralTempLeft - tempFactorLeft * YDIVISION / 2 ) {
        m_msbTempIntervalLeft->downStep();
    }

    if( m_dataAxises.value(QString::fromUtf8("SENS2")).last() > m_prevCentralTempRight + tempFactorRight * YDIVISION / 2 ) {
        m_msbTempIntervalRight->upStep();
    }

    if( m_dataAxises.value(QString::fromUtf8("SENS2")).last() < m_prevCentralTempRight - tempFactorRight * YDIVISION / 2 ) {
        m_msbTempIntervalRight->downStep();
    }
}

double PlotterDialog::roundToStep(const double &value, const double &step)
{
    int sign = 0;
    value >= 0 ? sign = 1 : sign = -1;

    if( step <= 0 ) {
        return 0;
    }

    int tempValue = static_cast<int>( qAbs(value) / step );

    return step * static_cast<double>( sign * ( tempValue + 1 ) );
}

void PlotterDialog::arrow(bool isChecked)
{
    m_pickerLeft->setEnabled(isChecked);
    m_pickerRight->setEnabled(isChecked);

    m_pickerNoteEditLeft->setEnabled(!isChecked);
    m_pickerNoteMoveLeft->setEnabled(!isChecked);
    m_pickerNoteDeleteLeft->setEnabled(!isChecked);

    m_pickerNoteEditRight->setEnabled(!isChecked);
    m_pickerNoteMoveRight->setEnabled(!isChecked);
    m_pickerNoteDeleteRight->setEnabled(!isChecked);
}

void PlotterDialog::textEdit(bool isChecked)
{
    m_pickerLeft->setEnabled(!isChecked);
    m_pickerRight->setEnabled(!isChecked);

    m_pickerNoteEditLeft->setEnabled(isChecked);
    m_pickerNoteMoveLeft->setEnabled(!isChecked);
    m_pickerNoteDeleteLeft->setEnabled(!isChecked);

    m_pickerNoteEditRight->setEnabled(isChecked);
    m_pickerNoteMoveRight->setEnabled(!isChecked);
    m_pickerNoteDeleteRight->setEnabled(!isChecked);
}

void PlotterDialog::textMove(bool isChecked)
{
    m_pickerLeft->setEnabled(!isChecked);
    m_pickerRight->setEnabled(!isChecked);

    m_pickerNoteEditLeft->setEnabled(!isChecked);
    m_pickerNoteMoveLeft->setEnabled(isChecked);
    m_pickerNoteDeleteLeft->setEnabled(!isChecked);

    m_pickerNoteEditRight->setEnabled(!isChecked);
    m_pickerNoteMoveRight->setEnabled(isChecked);
    m_pickerNoteDeleteRight->setEnabled(!isChecked);
}

void PlotterDialog::textDelete(bool isChecked)
{
    m_pickerLeft->setEnabled(!isChecked);
    m_pickerRight->setEnabled(!isChecked);

    m_pickerNoteEditLeft->setEnabled(!isChecked);
    m_pickerNoteMoveLeft->setEnabled(!isChecked);
    m_pickerNoteDeleteLeft->setEnabled(isChecked);

    m_pickerNoteEditRight->setEnabled(!isChecked);
    m_pickerNoteMoveRight->setEnabled(!isChecked);
    m_pickerNoteDeleteRight->setEnabled(isChecked);
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
            m_dataAxises.insert(listKeys.at(i), data);
        }
    }

    double elapsedTime = static_cast<double>( m_currentTime->elapsed() ) / 1000; // sec

    // keeping vectors sizes constant
    if( elapsedTime > 2 * 100 * XDIVISION ) {
        m_timeAxis.pop_front();
        for( int i = 0; i < m_dataAxises.size(); ++i ) {
            QVector<double> data = m_dataAxises.value( listKeys.at(i) );
            data.pop_front();
            m_dataAxises.remove( listKeys.at(i) );
            m_dataAxises.insert( listKeys.at(i), data );
        }
    }

    autoScroll(elapsedTime);

    m_timeAxis.push_back( elapsedTime );
    for(int i = 0; i < listKeys.size(); ++i) {
        if( listKeys.contains( m_Curves.at(i)->title().text() ) ) { // protection from errored inputing data
            QVector<double> data = m_dataAxises.value( listKeys.at(i) );
            data.push_back( curvesData.value( m_Curves.at(i)->title().text() ) );
            m_dataAxises.remove( listKeys.at(i) );
            m_dataAxises.insert( listKeys.at(i), data );

            if(m_isRessumed) {
                m_Curves[i]->setSamples( m_timeAxis, m_dataAxises.value( listKeys.at(i) ) );
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

    dynamic_cast<QPushButton *>( m_lcdTempIntervalLeft->buttonUpWidget() )->setMaximumSize(20, 20);
    dynamic_cast<QPushButton *>( m_lcdTempIntervalLeft->buttonDownWidget() )->setMaximumSize(20, 20);

    dynamic_cast<QPushButton *>( m_msbTempIntervalLeft->buttonUpWidget() )->setMaximumSize(20, 20);
    dynamic_cast<QPushButton *>( m_msbTempIntervalLeft->buttonDownWidget() )->setMaximumSize(20, 20);

    dynamic_cast<QPushButton *>( m_lcdTempIntervalRight->buttonUpWidget() )->setMaximumSize(20, 20);
    dynamic_cast<QPushButton *>( m_lcdTempIntervalRight->buttonDownWidget() )->setMaximumSize(20, 20);

    dynamic_cast<QPushButton *>( m_msbTempIntervalRight->buttonUpWidget() )->setMaximumSize(20, 20);
    dynamic_cast<QPushButton *>( m_msbTempIntervalRight->buttonDownWidget() )->setMaximumSize(20, 20);

    dynamic_cast<QPushButton *>( m_lcdTimeInterval->buttonUpWidget() )->setMaximumSize(20, 20);
    dynamic_cast<QPushButton *>( m_lcdTimeInterval->buttonDownWidget() )->setMaximumSize(20, 20);

    dynamic_cast<QPushButton *>( m_msbTimeInterval->buttonUpWidget() )->setMaximumSize(20, 20);
    dynamic_cast<QPushButton *>( m_msbTimeInterval->buttonDownWidget() )->setMaximumSize(20, 20);

    QGridLayout *gridInfo = new QGridLayout;
    QLabel *lInstalled = new QLabel("Installed:", this);
    lInstalled->setStyleSheet("color: #0000FF; font: bold; font-size: 12pt");
    gridInfo->addWidget(lInstalled, 0, 0, 1, 2);
    gridInfo->addWidget(lcdInstalledTemp, 0, 2);
    gridInfo->addWidget(m_rbRelateLeft, 1, 0, 1, 1, Qt::AlignLeft);
    QLabel *lSensor1 = new QLabel(QString::fromUtf8("Sensor 1:"));
    lSensor1->setStyleSheet("color: #FF0000; font: bold; font-size: 12pt");
    gridInfo->addWidget(lSensor1, 1, 1);
    gridInfo->addWidget(lcdSensor1Termo, 1, 2);
    gridInfo->addWidget(m_rbRelateRight, 2, 0, 1, 1, Qt::AlignLeft);
    QLabel *lSensor2 = new QLabel(QString::fromUtf8("Sensor 2:"));
    lSensor2->setStyleSheet("color: green; font: bold; font-size: 12pt");
    gridInfo->addWidget(lSensor2, 2, 1);
    gridInfo->addWidget(lcdSensor2Termo, 2, 2);
    gridInfo->setSpacing(5);

    bSetTemp->setFixedHeight(45);
    QHBoxLayout *setTempLayout0 = new QHBoxLayout;
    setTempLayout0->addWidget(sbSetTemp);
    setTempLayout0->addWidget(bSetTemp);
    setTempLayout0->setSpacing(5);

    QVBoxLayout *setTempLayout = new QVBoxLayout;
    setTempLayout->addItem(setTempLayout0);
    setTempLayout->addWidget(chbSynchronize);
    setTempLayout->addItem(gridInfo);
    setTempLayout->setSpacing(5);

    gbSetTemp->setLayout(setTempLayout);

    QHBoxLayout *notesLayout = new QHBoxLayout;
    notesLayout->addWidget(m_bArrow);
    notesLayout->addWidget(m_bEditNotes);
    notesLayout->addWidget(m_bMoveNotes);
    notesLayout->addWidget(m_bDeleteNotes);

    QGroupBox *notes = new QGroupBox();
    notes->setTitle("Notes");
    notes->setLayout(notesLayout);

    QGridLayout *grid = new QGridLayout;
    // пещаю логотип фирмы
    grid->addWidget(new QLabel("<img src=':/Resources/elisat.png' height='20' width='75'/>", this), 0, 0);
    grid->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding), 0, 1);
    grid->addWidget(lPort, 0, 2);
    grid->addWidget(cbPort, 0, 3);
    grid->addWidget(lBaud, 0, 4);
    grid->addWidget(cbBaud, 0, 5);
    grid->addWidget(bPortStart, 0, 6);
    grid->addWidget(bPortStop, 0, 7);
    grid->addWidget(lTx, 0, 8);
    grid->addWidget(lRx, 0, 9);
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

    QHBoxLayout *tempLeftLayout0 = new QHBoxLayout;
    tempLeftLayout0->addWidget(m_lcdTempIntervalLeft);
    tempLeftLayout0->addWidget(m_msbTempIntervalLeft);
    tempLeftLayout0->setSpacing(5);

    QVBoxLayout *tempLeftLayout = new QVBoxLayout;
    tempLeftLayout->addItem(tempLeftLayout0);
    tempLeftLayout->addWidget(m_cbTempAccurateLeft);
    tempLeftLayout->setSpacing(5);

    QHBoxLayout *tempRightLayout0 = new QHBoxLayout;
    tempRightLayout0->addWidget(m_lcdTempIntervalRight);
    tempRightLayout0->addWidget(m_msbTempIntervalRight);
    tempRightLayout0->setSpacing(5);

    QVBoxLayout *tempRightLayout = new QVBoxLayout;
    tempRightLayout->addItem(tempRightLayout0);
    tempRightLayout->addWidget(m_cbTempAccurateRight);
    tempRightLayout->setSpacing(5);

    QVBoxLayout *tempLayout = new QVBoxLayout;
    tempLayout->addItem(tempLeftLayout);
    tempLayout->addItem(tempRightLayout);
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
    knobsLayout->addWidget(notes);
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

void PlotterDialog::changeTempIntervalLeft()
{
    double factor = dynamic_cast<QLCDNumber*>(m_lcdTempIntervalLeft->spinWidget())->value();
    m_plot->setAxisScale( QwtPlot::yLeft,
                          m_prevCentralTempLeft - factor * YDIVISION/2,
                          m_prevCentralTempLeft + factor * YDIVISION/2,
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

void PlotterDialog::moveTempIntervalLeft()
{
    double factor = dynamic_cast<QLCDNumber*>(m_lcdTempIntervalLeft->spinWidget())->value();
    double offset = 0.0;
    if( m_prevTempOffsetLeft - factor * m_msbTempIntervalLeft->value() > 0 ) {
        offset = -m_msbTempIntervalLeft->step();
    } else {
        offset = m_msbTempIntervalLeft->step();
    }
    m_prevTempOffsetLeft = factor * m_msbTempIntervalLeft->value();

    m_plot->setAxisScale( QwtPlot::yLeft,
                          m_prevCentralTempLeft + factor * ( offset - YDIVISION/2 ) ,
                          m_prevCentralTempLeft + factor * ( offset + YDIVISION/2 ),
                          factor * YSCALESTEP );

    m_prevCentralTempLeft += factor * offset;

    updatePlot();
}

void PlotterDialog::changeTempIntervalRight()
{
    double factor = dynamic_cast<QLCDNumber*>(m_lcdTempIntervalRight->spinWidget())->value();
    m_plot->setAxisScale( QwtPlot::yRight,
                          m_prevCentralTempRight - factor * YDIVISION/2,
                          m_prevCentralTempRight + factor * YDIVISION/2,
                          factor * YSCALESTEP );
    updatePlot();
}

void PlotterDialog::moveTempIntervalRight()
{
    double factor = dynamic_cast<QLCDNumber*>(m_lcdTempIntervalRight->spinWidget())->value();
    double offset = 0.0;
    if( m_prevTempOffsetRight - factor * m_msbTempIntervalRight->value() > 0 ) {
        offset = -m_msbTempIntervalRight->step();
    } else {
        offset = m_msbTempIntervalRight->step();
    }
    m_prevTempOffsetRight = factor * m_msbTempIntervalRight->value();

    m_plot->setAxisScale( QwtPlot::yRight,
                          m_prevCentralTempRight + factor * ( offset - YDIVISION/2 ) ,
                          m_prevCentralTempRight + factor * ( offset + YDIVISION/2 ),
                          factor * YSCALESTEP );

    m_prevCentralTempRight += factor * offset;

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

void PlotterDialog::changeTempAccurateFactorLeft(bool isChecked)
{
    if(isChecked) {
        m_TempAccurateFactorLeft *= 0.1;
    } else {
        m_TempAccurateFactorLeft *= 10;
    }
    m_msbTempIntervalLeft->setRange(LOWTEMP,
                                    UPTEMP,
                                    m_TempAccurateFactorLeft * STEPTEMP);
}

void PlotterDialog::changeTempAccurateFactorRight(bool isChecked)
{
    if(isChecked) {
        m_TempAccurateFactorRight *= 0.1;
    } else {
        m_TempAccurateFactorRight *= 10;
    }
    m_msbTempIntervalRight->setRange(LOWTEMP,
                                     UPTEMP,
                                     m_TempAccurateFactorRight * STEPTEMP);
}

void PlotterDialog::radioButtonClicked(int id)
{
    if( id == 1 ) {
        m_Curves[2]->setYAxis( QwtPlot::yLeft );
    } else {
        m_Curves[2]->setYAxis( QwtPlot::yRight );
    }

    updatePlot();
}

void PlotterDialog::notesButtonToggled(int id, bool isChecked)
{
    switch ( id ) {
    case 0:
        arrow(isChecked);
        break;
    case 1:
        textEdit(isChecked);
        break;
    case 2:
        textMove(isChecked);
        break;
    case 3:
        textDelete(isChecked);
        break;
    }
}

void PlotterDialog::resetTime()
{
    m_isReseted = true;

    QStringList dataAxisKeysList = m_dataAxises.keys();
    // Protection from reset at the begin & from incomplete data
    if( dataAxisKeysList.size() != m_Curves.size() ) {
        return;
    }

    m_timeAxis.clear();
    m_dataAxises.clear();

    for(int i = 0; i < m_Curves.size(); ++i) {
        QVector<double> data;
        m_dataAxises.insert( dataAxisKeysList.at(i), data );
    }

    toCurrentTime();

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
    connect(m_msbTimeInterval, SIGNAL(valueChanged()), this, SLOT(moveTimeInterval()));
    connect(m_cbTimeAccurate, SIGNAL(clicked(bool)), this, SLOT(changeTimeAccurateFactor(bool)));

    connect(m_lcdTempIntervalLeft, SIGNAL(valueChanged()), this, SLOT(changeTempIntervalLeft()));
    connect(m_msbTempIntervalLeft, SIGNAL(valueChanged()), this, SLOT(moveTempIntervalLeft()));
    connect(m_cbTempAccurateLeft, SIGNAL(clicked(bool)), this, SLOT(changeTempAccurateFactorLeft(bool)));

    connect(m_lcdTempIntervalRight, SIGNAL(valueChanged()), this, SLOT(changeTempIntervalRight()));
    connect(m_msbTempIntervalRight, SIGNAL(valueChanged()), this, SLOT(moveTempIntervalRight()));
    connect(m_cbTempAccurateRight, SIGNAL(clicked(bool)), this, SLOT(changeTempAccurateFactorRight(bool)));

    connect(m_bgRelate, SIGNAL(buttonClicked(int)), this, SLOT(radioButtonClicked(int)));
    connect(m_bgNotes, SIGNAL(buttonToggled(int,bool)), this, SLOT(notesButtonToggled(int,bool)));

    connect(m_bReset, SIGNAL(clicked()), this, SLOT(resetTime()));
    connect(m_bPauseRessume, SIGNAL(clicked()), this, SLOT(pauseRessume()));

    connect(m_pickerNoteEditLeft, SIGNAL(appended(QPoint)), this, SLOT(linkNotesTo_yLeft()));
    connect(m_pickerNoteMoveLeft, SIGNAL(appended(QPoint)), this, SLOT(linkNotesTo_yLeft()));
    connect(m_pickerNoteDeleteLeft, SIGNAL(appended(QPoint)), this, SLOT(linkNotesTo_yLeft()));

    connect(m_pickerNoteEditLeft, SIGNAL(selected(QPointF)), this, SLOT(editNotes(QPointF)));
    connect(m_pickerNoteMoveLeft, SIGNAL(moved(QPointF)), this, SLOT(moveNotes(QPointF)));
    connect(m_pickerNoteMoveLeft, SIGNAL(appended(QPointF)), this, SLOT(moveNotes(QPointF)));
    connect(m_pickerNoteDeleteLeft, SIGNAL(selected(QPointF)), this, SLOT(deleteNotes(QPointF)));

    connect(m_pickerNoteEditRight, SIGNAL(appended(QPoint)), this, SLOT(linkNotesTo_yRight()));
    connect(m_pickerNoteMoveRight, SIGNAL(appended(QPoint)), this, SLOT(linkNotesTo_yRight()));
    connect(m_pickerNoteDeleteRight, SIGNAL(appended(QPoint)), this, SLOT(linkNotesTo_yRight()));

    connect(m_pickerNoteEditRight, SIGNAL(selected(QPointF)), this, SLOT(editNotes(QPointF)));
    connect(m_pickerNoteMoveRight, SIGNAL(moved(QPointF)), this, SLOT(moveNotes(QPointF)));
    connect(m_pickerNoteMoveRight, SIGNAL(appended(QPointF)), this, SLOT(moveNotes(QPointF)));
    connect(m_pickerNoteDeleteRight, SIGNAL(selected(QPointF)), this, SLOT(deleteNotes(QPointF)));

    connect(m_notesDialog, SIGNAL(textInputed(QTextEdit*,QPointF)), this, SLOT(addText(QTextEdit*,QPointF)));

    connect(bPortStart, SIGNAL(clicked()), this, SLOT(openPort()));
    connect(bPortStop, SIGNAL(clicked()), this, SLOT(closePort()));
    connect(cbPort, SIGNAL(currentIndexChanged(int)), this, SLOT(closePort()));
    connect(cbBaud, SIGNAL(currentIndexChanged(int)), this, SLOT(closePort()));
    connect(itsProtocol, SIGNAL(DataIsReaded(bool)), this, SLOT(received(bool)));
    connect(itsProtocol, SIGNAL(DataIsWrited(bool)), this, SLOT(written(bool)));
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

void PlotterDialog::written(bool isWritten)
{
    if(isWritten && !itsBlinkTimeTxColor->isActive() && !itsBlinkTimeTxNone->isActive()) {
        itsBlinkTimeTxColor->start();
        lTx->setStyleSheet("background: red; font: bold; font-size: 10pt");
    }
}

void PlotterDialog::writeTemp()
{
    if(itsPort->isOpen()) {
        QMultiMap<QString, QString> dataTemp;

        dataTemp.insert("CODE", "0");
        dataTemp.insert("TEMP", QString::number(static_cast<int>(sbSetTemp->value())));
        itsProtocol->setDataToWrite(dataTemp);
        itsProtocol->writeData();

        if(chbSynchronize->isChecked()) {
            resetTime();
        }
    }
}

void PlotterDialog::colorTxNone()
{
    itsBlinkTimeTxNone->stop();
}

void PlotterDialog::setColorLCD(QLCDNumber *lcd, bool isHeat)
{
    if(isHeat) {
        setColorLCD(lcd, QColor(100, 0, 0));
    } else {
        setColorLCD(lcd, QColor(0, 0, 100));
    }
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

void PlotterDialog::setColorLCD(QLCDNumber *lcd, const QColor &color)
{
    QPalette palette;
    // get the palette
    palette = lcd->palette();

    // foreground color
    palette.setColor(palette.WindowText, color);
    // "light" border
    palette.setColor(palette.Light, color);
    // "dark" border
    palette.setColor(palette.Dark, color);

    // set the palette
    lcd->setPalette(palette);
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
    }

    itsSensorsList.clear();
    updatePlot();
}

void PlotterDialog::colorSetTempLCD()
{
    setColorLCD(dynamic_cast<QLCDNumber*>(sbSetTemp->spinWidget()), sbSetTemp->value() > 0);
}

void PlotterDialog::moveNotes(const QPointF &pos)
{
    int index = whichNoteSelected(pos);

    if( index != -1 ) {
        m_notesList.at(index)->setValue(pos);
    }

    updatePlot();
}

void PlotterDialog::deleteNotes(const QPointF &pos)
{
    int index = whichNoteSelected(pos);

    if( index != -1 ) {
        QwtPlotMarker *note = m_notesList[index];
        m_notesList.removeAt(index);
        note->detach();
        delete note;
    }

    updatePlot();
}

void PlotterDialog::editNotes(const QPointF &pos)
{
    int index = whichNoteSelected(pos);

    if( index != -1 ) {
        QwtText text = m_notesList.at(index)->label();
        QwtPlotMarker *note = m_notesList[index];
#ifdef DEBUG
        qDebug() << "\neditNotes\nfont:" << "\nfamily:" << text.font().family()
                 << "\nsize:" << text.font().pointSize() << "\nbold:" << text.font().bold()
                 << "\nitalic:" << text.font().italic() << "\nunderline:" << text.font().underline();
        qDebug() << "color:" << text.color().name();
#endif
        m_notesDialog->setText( text.text(), note->value(), text.font(), text.color() );
    } else {
#ifdef DEBUG
        qDebug() << "\neditNotes\nfont:" << "\nfamily:" << m_prevNotesFont.family()
                 << "\nsize:" << m_prevNotesFont.pointSize() << "\nbold:" << m_prevNotesFont.bold()
                 << "\nitalic:" << m_prevNotesFont.italic() << "\nunderline:" << m_prevNotesFont.underline();
        qDebug() << "color:" <<  m_prevNotesColor.name();
#endif
        m_notesDialog->setText(QString::null, pos, m_prevNotesFont, m_prevNotesColor);
    }

    m_notesDialog->show();
}

int PlotterDialog::whichNoteSelected(const QPointF &pos)
{
    double pxXsec = m_plot->canvas()->size().width() / ( XDIVISION * m_lcdTimeInterval->value() );
    double pxYdeg = 0.5;
    if( m_notesLinkAxis == QwtPlot::yLeft ) {
        pxYdeg = m_plot->canvas()->size().height() / ( YDIVISION * m_lcdTempIntervalLeft->value() );
    } else {
        pxYdeg = m_plot->canvas()->size().height() / ( YDIVISION * m_lcdTempIntervalRight->value() );
    }

    for( int i = 0; i < m_notesList.size(); ++i ) {
//        QFontMetricsF fontMetrics( m_notesList.at(i)->label().font() );
//        QRectF textRect = fontMetrics.boundingRect( m_notesList.at(i)->label().text() );
        QGraphicsTextItem fontMetrics( m_notesList.at(i)->label().text() );
        fontMetrics.setFont( m_notesList.at(i)->label().font() );
        QRectF textRect = fontMetrics.boundingRect();

        if(  qAbs( pos.x() - m_notesList.at(i)->value().x() ) <= textRect.width()/( 2 * pxXsec )
             && qAbs( pos.y() - m_notesList.at(i)->value().y() ) <= textRect.height()/( 2 * pxYdeg )
             && m_notesLinkAxis == m_notesList.at(i)->yAxis() ) {

            return i;
        }
    }

    return -1;
}

void PlotterDialog::addText(QTextEdit *text, const QPointF &pos)
{
    int index = whichNoteSelected(pos);

    if( index != -1 ) {
        QwtPlotMarker *note = m_notesList[index];

        m_notesList.removeAt(index);
        note->detach();
        delete note;
        note = 0;
    }
    m_prevNotesFont = text->font();
    m_prevNotesColor = text->textColor();

    QwtPlotMarker *marker = new QwtPlotMarker();
    QwtText label;
    label.setColor(text->textColor());
    label.setFont(text->font());
    label.setText(text->toPlainText());
    label.setBackgroundBrush(QBrush(QColor(255, 255, 255, 150)));
    label.setBorderRadius(5);

    if( m_notesLinkAxis == QwtPlot::yLeft ) {
        label.setBorderPen(QPen(QBrush(Qt::red), 2));
    } else {
        label.setBorderPen(QPen(QBrush(Qt::green), 2));
    }

    marker->setLabel(label);
    marker->attach(m_plot);
    marker->setValue(pos);
    marker->setAxes(QwtPlot::xBottom, m_notesLinkAxis);

    m_notesList.push_back(marker);

    updatePlot();
}

void PlotterDialog::linkNotesTo_yLeft()
{
    m_notesLinkAxis = QwtPlot::yLeft;
}

void PlotterDialog::linkNotesTo_yRight()
{
    m_notesLinkAxis = QwtPlot::yRight;
}
