#include "PlotterDialog.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
//#include <QFrame>
#include <QStringList>
#include <qwt_plot_canvas.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_div.h>

#define XDIVISION 10
#define XMAJORDIVISION 10
#define XMINORDIVISION 5

#define YDIVISION 8
#define YMAJORDIVISION 10
#define YMINORDIVISION 5

PlotterDialog::PlotterDialog(const QString &title, QWidget *parent) :
    QDialog(parent),
    m_lcdTimeInterval(new QLCDNumber(this)),
    m_knobTimeInterval(new QwtKnob(this)),

    m_lcdInstalledTempInterval(new QLCDNumber(this)),
    m_knobInstalledTempInterval(new QwtKnob(this)),

    m_lcdSensor1TempInterval(new QLCDNumber(this)),
    m_knobSensor1TempInterval(new QwtKnob(this)),

    m_lcdSensor2TempInterval(new QLCDNumber(this)),
    m_knobSensor2TempInterval(new QwtKnob(this)),

    m_sbarInfo(new QStatusBar(this)),
    m_plot(new QwtPlot(this))
{
    setWindowTitle(title);

    QList<QwtKnob*> knobList;
    knobList << m_knobTimeInterval << m_knobInstalledTempInterval
             << m_knobSensor1TempInterval << m_knobSensor2TempInterval;
    knobStyling(knobList);

    QList<QLCDNumber*> lcdList;
    lcdList << m_lcdTimeInterval << m_lcdInstalledTempInterval
               << m_lcdSensor1TempInterval << m_lcdSensor2TempInterval;
    lcdStyling(lcdList);

    QList<double> timeTicksPos;
    timeTicksPos << 0.5 << 1 << 2 << 5 << 10 << 20 << 30 << 40 << 50 << 60;

    QwtScaleDiv timeScaleDiv;
    timeScaleDiv.setTicks(QwtScaleDiv::MajorTick, timeTicksPos);
    timeScaleDiv.setInterval(0.5, 60);

    m_knobTimeInterval->setScale(0, 600);
//    m_knobTimeInterval->setScaleStepSize(0);
    m_knobTimeInterval->setSingleSteps(0.5);
    m_knobTimeInterval->setScaleMaxMajor(10);
    m_knobTimeInterval->setScaleMaxMinor(1);
//    m_knobTimeInterval->setTotalSteps(60/0.5);
    m_knobTimeInterval->setValue(1);

    m_knobInstalledTempInterval->setScale(-50, 50);
    m_knobInstalledTempInterval->setScaleStepSize(10);
    m_knobInstalledTempInterval->setSingleSteps(1);
    m_knobInstalledTempInterval->setScaleMaxMajor(10);
    m_knobInstalledTempInterval->setScaleMaxMinor(10);
    m_knobInstalledTempInterval->setTotalSteps(100);
    m_knobInstalledTempInterval->setValue(35);

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
    m_plot->setAxisScale(QwtPlot::xBottom, -XDIVISION/2, XDIVISION/2);
    m_plot->setAxisMaxMajor( QwtPlot::xBottom, XMAJORDIVISION );
    m_plot->setAxisMaxMinor( QwtPlot::xBottom, XMINORDIVISION);

    m_plot->setAxisTitle( QwtPlot::yLeft, "Temperature, °C" );
    m_plot->setAxisScale(QwtPlot::yLeft, -YDIVISION/2, YDIVISION/2);
    m_plot->setAxisMaxMajor( QwtPlot::yLeft, YMAJORDIVISION );
    m_plot->setAxisMaxMinor( QwtPlot::yLeft, YMINORDIVISION );

//    m_plot->enableAxis(QwtPlot::xBottom, false);
//    m_plot->enableAxis(QwtPlot::yLeft, false);

    // curves
    QMultiMap<QString, QVector<double> > curves;
    QVector<double> x;
    QVector<double> y;

    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 60; ++j) {
            x.append(j);
            y.append(2*(2*i + 1)*qSin(j + 10*i));
        }
        curves.insertMulti(QString("Sensor ") + QString::number(i + 1), y);
        curves.insertMulti(QString("Sensor ") + QString::number(i + 1), x);
        x.clear();
        y.clear();
    }

    setCurves(curves);

    m_plot->setAutoReplot( true );

    setupGUI();

    setupConnections();
}

void PlotterDialog::setCurves(const QMultiMap<QString, QVector<double> > &curves)
{
    QStringList listKeys = curves.keys();
    listKeys.removeDuplicates();

    for(int i = 0; i < listKeys.size(); ++i) {
        m_Curves.append(new QwtPlotCurve);
        m_Curves[i]->setRenderHint( QwtPlotItem::RenderAntialiased );
        m_Curves[i]->setLegendAttribute( QwtPlotCurve::LegendShowLine );
        m_Curves[i]->setYAxis( QwtPlot::yLeft );
        m_Curves[i]->setXAxis( QwtPlot::xBottom );
        m_Curves[i]->setTitle(listKeys.at(i));
        m_Curves[i]->setPen(static_cast<Qt::GlobalColor>(i + 9));
//        m_Curves[i]->setPen(Qt::green);
        m_Curves[i]->setSamples( curves.lowerBound( listKeys.at(i) ).value(),
                                 ( ++curves.lowerBound( listKeys.at(i) ) ).value() );
        m_Curves[i]->attach(m_plot);
    }
}

void PlotterDialog::setupGUI()
{
    QGridLayout *knobsLayout = new QGridLayout;
    knobsLayout->addWidget(m_lcdTimeInterval, 0, 0);
    knobsLayout->addWidget(m_knobTimeInterval, 0, 1);
    knobsLayout->addWidget(m_lcdInstalledTempInterval, 1, 0);
    knobsLayout->addWidget(m_knobInstalledTempInterval, 1, 1);
    knobsLayout->addWidget(m_lcdSensor1TempInterval, 2, 0);
    knobsLayout->addWidget(m_knobSensor1TempInterval, 2, 1);
    knobsLayout->addWidget(m_lcdSensor2TempInterval, 3, 0);
    knobsLayout->addWidget(m_knobSensor2TempInterval, 3, 1);
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

void PlotterDialog::knobStyling(QList<QwtKnob *> &knobList)
{
    foreach (QwtKnob *knob, knobList) {
        knob->setKnobStyle(QwtKnob::Sunken);
        knob->setMarkerStyle(QwtKnob::Tick);
    }
}

void PlotterDialog::lcdStyling(QList<QLCDNumber *> &lcdList)
{
    foreach(QLCDNumber *lcd, lcdList) {
        lcd->setMinimumSize(80, 40);
        lcd->setMaximumSize(80, 40);
        lcd->setDigitCount(6);
        lcd->setSegmentStyle(QLCDNumber::Flat);
        lcd->setFrameStyle(QFrame::NoFrame);
    }
}

void PlotterDialog::changeTimeInterval(double interval)
{
    m_plot->setAxisScale(QwtPlot::xBottom, 0, interval);
}

void PlotterDialog::setupConnections()
{
    connect(m_knobTimeInterval, SIGNAL(valueChanged(double)), this, SLOT(changeTimeInterval(double)));

    connect(m_knobTimeInterval, SIGNAL(valueChanged(double)), m_lcdTimeInterval, SLOT(display(double)));
    connect(m_knobInstalledTempInterval, SIGNAL(valueChanged(double)), m_lcdInstalledTempInterval, SLOT(display(double)));
    connect(m_knobSensor1TempInterval, SIGNAL(valueChanged(double)), m_lcdSensor1TempInterval, SLOT(display(double)));
    connect(m_knobSensor2TempInterval, SIGNAL(valueChanged(double)), m_lcdSensor2TempInterval, SLOT(display(double)));
}
