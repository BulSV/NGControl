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
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>

#define XDIVISION 10
#define XMAJORDIVISION 10
#define XMINORDIVISION 5
#define XSCALESTEP 1

#define YDIVISION 8
#define YMAJORDIVISION 10
#define YMINORDIVISION 5
#define YSCALESTEP 1

#define LOWTIME 0
#define UPTIME 1200
#define STEPTIME 1

#define LOWTEMP -140
#define UPTEMP 140
#define STEPTEMP 1

PlotterDialog::PlotterDialog(const QString &title, QWidget *parent) :
    QDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint),
    m_cbTimeAccurate(new QCheckBox(QString::fromUtf8("Accurate (x0.1)"), this)),
    m_cbTempAccurate(new QCheckBox(QString::fromUtf8("Accurate (x0.1)"), this)),
    m_bStop(new QPushButton(QString::fromUtf8("Stop"), this)),
    m_bRessume(new QPushButton(QString::fromUtf8("Ressume"), this)),
    m_TimeAccurateFactor(1.0),
    m_TempAccurateFactor(1.0),
    m_sbarInfo(new QStatusBar(this)),
    m_plot(new QwtPlot(this)),
    m_currentTime( new QTime()),
    m_offset( 0.0 )
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
    m_lcdTimeInterval->setValue(timeSamples.size());

    m_lcdTempInterval = new LCDSampleSpinBox(tempSamples,
                                                      QIcon(":/Resources/down.png"),
                                                      QIcon(":/Resources/up.png"),
                                                      QString::fromUtf8(""),
                                                      QString::fromUtf8(""),
                                                      QLCDNumber::Dec,
                                                      LCDSpinBox::RIGHT,
                                                      this);
    m_lcdTempInterval->setValue(tempSamples.size());

    m_msbTimeInterval = new MoveSpinBox("<img src=':Resources/LeftRight.png' height='48' width='48'/>",
                                        QIcon(":/Resources/left.png"),
                                        QIcon(":/Resources/right.png"),
                                        QString::fromUtf8(""),
                                        QString::fromUtf8(""),
                                        MoveSpinBox::BOTTOM,
                                        this);
    m_msbTimeInterval->setRange(LOWTIME, UPTIME, STEPTIME);

    m_msbTempInterval = new MoveSpinBox("<img src=':Resources/UpDown.png' height='48' width='48'/>",
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
            << dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget());
    lcdStyling(lcdList);

    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setBorderRadius(5);
    canvas->setFrameShadow(QwtPlotCanvas::Sunken);

    m_plot->setCanvas(canvas);
    m_plot->setCanvasBackground(QBrush(QColor("#FFFFFF")));

    // legend
    QwtLegend *legend = new QwtLegend;
    m_plot->insertLegend( legend, QwtPlot::TopLegend );

    // grid
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableXMin( true );
    grid->enableYMin( true );
    grid->setMajorPen( Qt::black, 0 );
    grid->setMinorPen( Qt::black, 0, Qt::DotLine );
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
                          -dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value() * YDIVISION/2,
                          dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value() * YDIVISION/2,
                          dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value() * YSCALESTEP );
    m_plot->setAxisMaxMajor( QwtPlot::yLeft, YMAJORDIVISION );
    m_plot->setAxisMaxMinor( QwtPlot::yLeft, YMINORDIVISION );
    m_plot->setAxisAutoScale( QwtPlot::yLeft, false);

    m_plot->setAutoReplot( false );

    QwtPlotPicker *d_picker = new QwtPlotPicker(QwtPlot::xBottom,
                                                QwtPlot::yLeft,
                                                QwtPlotPicker::CrossRubberBand,
                                                QwtPicker::ActiveOnly,
                                                m_plot->canvas());
    d_picker->setStateMachine(new QwtPickerDragPointMachine());
    d_picker->setRubberBandPen(QColor(Qt::black));
    d_picker->setTrackerPen(QColor(Qt::black));

    setupGUI();
    setupConnections();
}

void PlotterDialog::setCurves(const QMap<QString, Qt::GlobalColor > &curves)
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

void PlotterDialog::appendData(const QMap<QString, double> &curvesData)
{
    QStringList listKeys = curvesData.keys();
    if( m_currentTime->isNull() ) {
        m_currentTime->start();

        if( !m_timeAxis.isEmpty() && !m_timeAxis.isEmpty()) {
            m_timeAxis.clear();
            m_dataAxises.clear();
        }

        for(int i = 0; i < listKeys.size(); ++i) {
            QVector<double> data;
            m_dataAxises.push_back(data);
        }
    }

    double elapsedTime = static_cast<double>( m_currentTime->elapsed() ) / 1000; // sec
    qDebug() << "elapsedTime:" << elapsedTime;
    qDebug() << "terminal side:" << dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())->value() * ( XDIVISION + m_msbTimeInterval->value() + m_offset );

    /*if( elapsedTime > XDIVISION + m_offset ) {
        m_offset += XSCALESTEP;
        m_plot->setAxisScale( QwtPlot::xBottom,
                              m_offset,
                              XDIVISION + m_offset,
                              XSCALESTEP );
    }*/
    if( elapsedTime > dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())->value() * ( XDIVISION + m_msbTimeInterval->value() + m_offset ) ) {
        m_offset += dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())->value() * XSCALESTEP;
        m_plot->setAxisScale( QwtPlot::xBottom,
                              dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())->value() * ( m_msbTimeInterval->value() + m_offset ),
                              dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())->value() * ( XDIVISION + m_msbTimeInterval->value() + m_offset ),
                              dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())->value() * XSCALESTEP );
        m_plot->replot();
    }

    m_timeAxis.push_back( elapsedTime );
    for(int i = 0; i < listKeys.size(); ++i) {
        if( listKeys.contains( m_Curves.at(i)->title().text() ) ) { // protection from errored inputing data
            m_dataAxises[i].push_back( curvesData.value( m_Curves.at(i)->title().text() ) );
            m_Curves[i]->setSamples( m_timeAxis, m_dataAxises.at(i) );
        }
    }
}

void PlotterDialog::updatePlot()
{
    m_plot->replot();
}

void PlotterDialog::setupGUI()
{
    QVBoxLayout *timeLayout = new QVBoxLayout;
    timeLayout->addWidget(m_lcdTimeInterval);
    timeLayout->addWidget(m_cbTimeAccurate);
    timeLayout->addWidget(m_msbTimeInterval);
    timeLayout->setSpacing(5);

    QGroupBox *gbTime = new QGroupBox("sec/div", this);
    gbTime->setLayout(timeLayout);

    QVBoxLayout *tempLayout = new QVBoxLayout;
    tempLayout->addWidget(m_lcdTempInterval);
    tempLayout->addWidget(m_cbTempAccurate);
    tempLayout->addWidget(m_msbTempInterval);
    tempLayout->setSpacing(5);

    QGroupBox *gbTemp = new QGroupBox("°C/div", this);
    gbTemp->setLayout(tempLayout);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(m_bStop);
    buttonsLayout->addWidget(m_bRessume);
    buttonsLayout->setSpacing(5);

    QVBoxLayout *knobsLayout = new QVBoxLayout;
    knobsLayout->addWidget(gbTime);
    knobsLayout->addWidget(gbTemp);
    knobsLayout->addItem(buttonsLayout);
    knobsLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    knobsLayout->setSpacing(5);

    QHBoxLayout *plotLayout = new QHBoxLayout;
    plotLayout->addWidget(m_plot, 1);
    plotLayout->addItem(knobsLayout);
    plotLayout->setSpacing(5);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addItem(plotLayout);
    mainLayout->addWidget(m_sbarInfo);
    mainLayout->setSpacing(5);

    setLayout(mainLayout);
}

void PlotterDialog::lcdStyling(QList<QLCDNumber *> &lcdList)
{
    foreach(QLCDNumber *lcd, lcdList) {
        lcd->setMinimumSize(80, 40);
        lcd->setMaximumSize(80, 40);
        lcd->setSegmentStyle(QLCDNumber::Flat);
        lcd->setFrameStyle(QFrame::NoFrame);
    }
}

void PlotterDialog::changeTimeInterval()
{
    m_plot->setAxisScale( QwtPlot::xBottom,
                          dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())->value() * m_msbTimeInterval->value(),
                          dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())->value() * ( XDIVISION + m_msbTimeInterval->value() ),
                          dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())->value() * XSCALESTEP );
    m_plot->replot();
}

void PlotterDialog::changeTempInterval()
{
    m_plot->setAxisScale( QwtPlot::yLeft,
                          dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value() * ( m_msbTempInterval->value() - YDIVISION/2 ),
                          dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value() * ( m_msbTempInterval->value() + YDIVISION/2 ),
                          dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value() * YSCALESTEP );
    m_plot->replot();
}

void PlotterDialog::moveTimeInterval()
{
    m_plot->setAxisScale( QwtPlot::xBottom,
                          dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())->value() * m_msbTimeInterval->value(),
                          dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())->value() * ( XDIVISION + m_msbTimeInterval->value() ),
                          dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())->value() * XSCALESTEP );
    m_plot->replot();
}

void PlotterDialog::moveTempInterval()
{
    //    m_plot->setAxisScale(QwtPlot::yLeft,
    //                         -dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value() * YDIVISION/2 + m_msbTempInterval->value(),
    //                         dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value() * YDIVISION/2 + m_msbTempInterval->value(),
    //                         dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value() * YSCALESTEP );
    m_plot->setAxisScale( QwtPlot::yLeft,
                          dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value() * ( m_msbTempInterval->value() - YDIVISION/2 ),
                          dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value() * ( m_msbTempInterval->value() + YDIVISION/2 ),
                          dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value() * YSCALESTEP );
    m_plot->replot();
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

void PlotterDialog::setupConnections()
{
    connect(m_lcdTimeInterval, SIGNAL(valueChanged()), this, SLOT(changeTimeInterval()));
    connect(m_lcdTempInterval, SIGNAL(valueChanged()), this, SLOT(changeTempInterval()));
    connect(m_msbTimeInterval, SIGNAL(valueChanged()), this, SLOT(moveTimeInterval()));
    connect(m_msbTempInterval, SIGNAL(valueChanged()), this, SLOT(moveTempInterval()));

    connect(m_cbTimeAccurate, SIGNAL(clicked(bool)), this, SLOT(changeTimeAccurateFactor(bool)));
    connect(m_cbTempAccurate, SIGNAL(clicked(bool)), this, SLOT(changeTempAccurateFactor(bool)));
}
