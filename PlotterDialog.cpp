#include "PlotterDialog.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
//#include <QFrame>
#include <QStringList>
#include <qwt_plot_canvas.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_div.h>
//#include <qwt_panner.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_marker.h>
//#include <qwt_symbol.h>

#define XDIVISION 10
#define XMAJORDIVISION 10
#define XMINORDIVISION 5

#define YDIVISION 8
#define YMAJORDIVISION 10
#define YMINORDIVISION 5

PlotterDialog::PlotterDialog(const QString &title, QWidget *parent) :
    QDialog(parent),
    m_sbarInfo(new QStatusBar(this)),
    m_plot(new QwtPlot(this))
{
    QVector<double> timeSamples;
    timeSamples << 0.5 << 1 << 2 << 5 << 10 << 20 << 30 << 40 << 50 << 60;

    QVector<double> tempSamples;
    tempSamples << 0.5 << 1 << 2 << 5 << 10;

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
    m_msbTimeInterval->setRange(0, 600, 1);

    m_msbTempInterval = new MoveSpinBox("<img src=':Resources/UpDown.png' height='48' width='48'/>",
                                        QIcon(":/Resources/down.png"),
                                        QIcon(":/Resources/up.png"),
                                        QString::fromUtf8(""),
                                        QString::fromUtf8(""),
                                        MoveSpinBox::RIGHT,
                                        this);
    m_msbTempInterval->setRange(-50, 50, 1);

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
    m_plot->setAxisScale(QwtPlot::xBottom,
                         0,
                         dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())->value() * XDIVISION);
    m_plot->setAxisMaxMajor( QwtPlot::xBottom, XMAJORDIVISION );
    m_plot->setAxisMaxMinor( QwtPlot::xBottom, XMINORDIVISION );
    m_plot->setAxisAutoScale( QwtPlot::xBottom, false);

    m_plot->setAxisTitle( QwtPlot::yLeft, "Temperature, °C" );
    m_plot->setAxisScale(QwtPlot::yLeft,
                         -dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value() * YDIVISION/2,
                         dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value() * YDIVISION/2);
    m_plot->setAxisMaxMajor( QwtPlot::yLeft, YMAJORDIVISION );
    m_plot->setAxisMaxMinor( QwtPlot::yLeft, YMINORDIVISION );
    m_plot->setAxisAutoScale( QwtPlot::yLeft, false);

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

    QwtPlotPicker *d_picker = new QwtPlotPicker(QwtPlot::xBottom,
                                                QwtPlot::yLeft,
                                                QwtPlotPicker::CrossRubberBand,
                                                QwtPicker::ActiveOnly,
                                                m_plot->canvas());
    d_picker->setStateMachine(new QwtPickerDragPointMachine());
    d_picker->setRubberBandPen(QColor(Qt::black));
    d_picker->setTrackerPen(QColor(Qt::black));

    /*QwtPlotMarker *d_marker1 = new QwtPlotMarker();
    d_marker1->setValue( 0.0, 0.0 );
    d_marker1->setLineStyle( QwtPlotMarker::VLine );
    d_marker1->setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
    d_marker1->setLinePen( Qt::green, 0, Qt::DashDotLine );
    d_marker1->attach( m_plot );

    QwtPlotMarker *d_marker2 = new QwtPlotMarker();
    d_marker2->setLineStyle( QwtPlotMarker::HLine );
    d_marker2->setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
    d_marker2->setLinePen( QColor( 200, 150, 0 ), 0, Qt::DashDotLine );
    d_marker2->setSymbol( new QwtSymbol( QwtSymbol::Diamond,
                                         QColor( Qt::yellow ),
                                         QColor( Qt::green ),
                                         QSize( 8, 8 ) ) );
    d_marker2->attach( m_plot );*/

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
        m_Curves[i]->setTitle( listKeys.at(i) );
        m_Curves[i]->setPen(static_cast<Qt::GlobalColor>(i + 9));
//        m_Curves[i]->setPen(Qt::green);
        m_Curves[i]->setSamples( curves.lowerBound( listKeys.at(i) ).value(),
                                 ( ++curves.lowerBound( listKeys.at(i) ) ).value() );
        m_Curves[i]->attach(m_plot);
    }
}

void PlotterDialog::setupGUI()
{
    QVBoxLayout *timeLayout = new QVBoxLayout;
    timeLayout->addWidget(m_lcdTimeInterval);
    timeLayout->addWidget(m_msbTimeInterval);
    timeLayout->setSpacing(5);

    QGroupBox *gbTime = new QGroupBox("sec/div", this);
    gbTime->setLayout(timeLayout);

    QVBoxLayout *tempLayout = new QVBoxLayout;
    tempLayout->addWidget(m_lcdTempInterval);
    tempLayout->addWidget(m_msbTempInterval);

    tempLayout->setSpacing(5);

    QGroupBox *gbTemp = new QGroupBox("°C/div", this);
    gbTemp->setLayout(tempLayout);

    QVBoxLayout *knobsLayout = new QVBoxLayout;
    knobsLayout->addWidget(gbTime);
    knobsLayout->addWidget(gbTemp);

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
    m_plot->setAxisScale(QwtPlot::xBottom,
                         m_msbTimeInterval->value(),
                         dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())->value() * XDIVISION + m_msbTimeInterval->value());
}

void PlotterDialog::changeTempInterval()
{
    m_plot->setAxisScale(QwtPlot::yLeft,
                         -dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value() * YDIVISION/2 + m_msbTempInterval->value(),
                         dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value() * YDIVISION/2 + m_msbTempInterval->value());
}

void PlotterDialog::moveTimeInterval()
{
    m_plot->setAxisScale(QwtPlot::xBottom,
                         m_msbTimeInterval->value(),
                         dynamic_cast<QLCDNumber*>(m_lcdTimeInterval->spinWidget())->value() * XDIVISION + m_msbTimeInterval->value());
}

void PlotterDialog::moveTempInterval()
{
    m_plot->setAxisScale(QwtPlot::yLeft,
                         -dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value() * YDIVISION/2 + m_msbTempInterval->value(),
                         dynamic_cast<QLCDNumber*>(m_lcdTempInterval->spinWidget())->value() * YDIVISION/2 + m_msbTempInterval->value());
}

void PlotterDialog::setupConnections()
{
    connect(m_lcdTimeInterval, SIGNAL(valueChanged()), this, SLOT(changeTimeInterval()));
    connect(m_lcdTempInterval, SIGNAL(valueChanged()), this, SLOT(changeTempInterval()));
    connect(m_msbTimeInterval, SIGNAL(valueChanged()), this, SLOT(moveTimeInterval()));
    connect(m_msbTempInterval, SIGNAL(valueChanged()), this, SLOT(moveTempInterval()));
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
