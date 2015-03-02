#include "PlotterDialog.h"
//#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
//#include <QFrame>
#include <QStringList>
#include <qwt_plot_canvas.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>

PlotterDialog::PlotterDialog(const QString &title, QWidget *parent) :
    QDialog(parent),
    m_lTimeInterval(new QLabel(this)),
    m_sdTimeInterval(new QwtKnob(this)),

    m_lInstalledTempInterval(new QLabel(this)),
    m_sdInstalledTempInterval(new QwtKnob(this)),

    m_lSensor1TempInterval(new QLabel(this)),
    m_sdSensor1TempInterval(new QwtKnob(this)),

    m_lSensor2TempInterval(new QLabel(this)),
    m_sdSensor2TempInterval(new QwtKnob(this)),

    m_sbarInfo(new QStatusBar(this)),
    m_plot(new QwtPlot(this))
{
    setWindowTitle(title);

    QList<QwtKnob*> knobList;
    knobList << m_sdTimeInterval << m_sdInstalledTempInterval
             << m_sdSensor1TempInterval << m_sdSensor2TempInterval;
    knobStyling(knobList);

    m_sdTimeInterval->setScale(10, 600);
    m_sdTimeInterval->setScaleStepSize(100);
    m_sdTimeInterval->setSingleSteps(1);
    m_sdTimeInterval->setScaleMaxMajor(10);
    m_sdTimeInterval->setScaleMaxMinor(10);
    m_sdTimeInterval->setTotalSteps(600-10);
    m_sdTimeInterval->setValue(60);

    m_sdInstalledTempInterval->setScale(-50, 50);
    m_sdInstalledTempInterval->setScaleStepSize(10);
    m_sdInstalledTempInterval->setSingleSteps(1);
    m_sdInstalledTempInterval->setScaleMaxMajor(10);
    m_sdInstalledTempInterval->setScaleMaxMinor(10);
    m_sdInstalledTempInterval->setTotalSteps(100);
    m_sdInstalledTempInterval->setValue(35);

    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setBorderRadius(5);
    canvas->setFrameShadow(QwtPlotCanvas::Sunken);
    m_plot->setCanvas(canvas);
    m_plot->setCanvasBackground(QBrush(QColor("#FFFFFF")));

    // legend
    QwtLegend *legend = new QwtLegend;
    m_plot->insertLegend( legend, QwtPlot::BottomLegend );

    // grid
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableXMin( true );
    grid->enableYMin( true );
    grid->setMajorPen( Qt::black, 0 );
    grid->setMinorPen( Qt::black, 0, Qt::DotLine );
    grid->attach( m_plot );

    // axes
    m_plot->setAxisTitle( QwtPlot::xBottom, "Time, sec" );
    m_plot->setAxisScale(QwtPlot::xBottom, 0, m_sdTimeInterval->value());
    m_plot->setAxisMaxMajor( QwtPlot::xBottom, 10 );
    m_plot->setAxisMaxMinor( QwtPlot::xBottom, 5);

    m_plot->setAxisTitle( QwtPlot::yLeft, "Temperature, °C" );
    m_plot->setAxisScale(QwtPlot::yLeft, -50, 50);
    m_plot->setAxisMaxMajor( QwtPlot::yLeft, 10 );
    m_plot->setAxisMaxMinor( QwtPlot::yLeft, 5 );

    // curves
    QMultiMap<QString, QVector<double> > curves;
    QVector<double> x;
    QVector<double> y;

    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < m_sdTimeInterval->value(); ++j) {
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

    connect(m_sdTimeInterval, SIGNAL(valueChanged(double)), this, SLOT(changeTimeInterval(double)));
    connect(m_sdTimeInterval, SIGNAL(valueChanged(double)), m_lTimeInterval, SLOT(setNum(double)));
    connect(m_sdInstalledTempInterval, SIGNAL(valueChanged(double)), m_lInstalledTempInterval, SLOT(setNum(double)));
    connect(m_sdSensor1TempInterval, SIGNAL(valueChanged(double)), m_lSensor1TempInterval, SLOT(setNum(double)));
    connect(m_sdSensor2TempInterval, SIGNAL(valueChanged(double)), m_lSensor2TempInterval, SLOT(setNum(double)));
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
    QVBoxLayout *knobsLayout = new QVBoxLayout;
    knobsLayout->addWidget(m_lTimeInterval, Qt::AlignCenter);
    knobsLayout->addWidget(m_sdTimeInterval, Qt::AlignCenter);
    knobsLayout->addWidget(m_lInstalledTempInterval, Qt::AlignCenter);
    knobsLayout->addWidget(m_sdInstalledTempInterval, Qt::AlignCenter);
    knobsLayout->addWidget(m_lSensor1TempInterval, Qt::AlignCenter);
    knobsLayout->addWidget(m_sdSensor1TempInterval, Qt::AlignCenter);
    knobsLayout->addWidget(m_lSensor2TempInterval, Qt::AlignCenter);
    knobsLayout->addWidget(m_sdSensor2TempInterval, Qt::AlignCenter);
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

void PlotterDialog::changeTimeInterval(double interval)
{
    m_plot->setAxisScale(QwtPlot::xBottom, 0, interval);
}
