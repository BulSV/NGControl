#include "PlotterDialog.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QStringList>
#include <qwt_plot_canvas.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>

PlotterDialog::PlotterDialog(const QString &title, QWidget *parent) :
    QDialog(parent),
    m_sbInterval(new QSpinBox(this)),
    m_sbarInfo(new QStatusBar(this)),
    m_plot(new QwtPlot(this))
{
    setWindowTitle(title);
    m_sbInterval->setRange(10, 600);
    m_sbInterval->setSingleStep(5);
    m_sbInterval->setSuffix(" sec");
    m_sbInterval->setValue(60);

    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setBorderRadius(5);
    canvas->setFrameShadow(QwtPlotCanvas::Raised);
    m_plot->setCanvas(canvas);
    m_plot->setCanvasBackground(QBrush(QColor(Qt::black)));

    // legend
    QwtLegend *legend = new QwtLegend;
    m_plot->insertLegend( legend, QwtPlot::BottomLegend );

    // grid
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableXMin( true );
    grid->enableYMin( true );
    grid->setMajorPen( Qt::white, 0 );
    grid->setMinorPen( Qt::white, 0, Qt::DotLine );
    grid->attach( m_plot );

    // axes
    m_plot->setAxisTitle( QwtPlot::xBottom, "Time, sec" );
    m_plot->setAxisScale(QwtPlot::xBottom, 0, m_sbInterval->value());
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
        for(int j = 0; j < m_sbInterval->value(); ++j) {
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

    connect(m_sbInterval, SIGNAL(valueChanged(int)), this, SLOT(changeInterval(int)));
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
    QFrame *hline = new QFrame(this);
    hline->setFrameShape(QFrame::HLine);
    hline->setFrameShadow(QFrame::Sunken);

    QHBoxLayout *hTop = new QHBoxLayout;
    hTop->addWidget(new QLabel("Time interval:"));
    hTop->addWidget(m_sbInterval);
    hTop->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
    hTop->setSpacing(5);

    QGridLayout *grid = new QGridLayout;
    grid->addItem(hTop, 0, 0, 1, 3);
    grid->addWidget(hline, 1, 0, 1, 3);
    grid->addWidget(m_plot, 2, 0, 1, 3);
    grid->addWidget(m_sbarInfo, 3, 0, 1, 3);
    grid->setSpacing(5);

    setLayout(grid);
}

void PlotterDialog::changeInterval(int interval)
{
    m_plot->setAxisScale(QwtPlot::xBottom, 0, interval);
}
