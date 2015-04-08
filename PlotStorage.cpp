#include "PlotStorage.h"

#include <QFile>
#include <QDataStream>
#include <qwt_plot_curve.h>
//#include <QFileDialog>
#include <QDebug>

QDataStream &operator<<(QDataStream &out, const QwtPlot &plot)
{
    qDebug() << "out <<";
    for(int i = 0; i < plot.itemList(QwtPlotItem::Rtti_PlotCurve).size(); ++i) {
        for(int j = 0; j < dynamic_cast<QwtPlotCurve *>(plot.itemList(QwtPlotItem::Rtti_PlotCurve).value(i))->data()->size(); ++j) {
            out << dynamic_cast<QwtPlotCurve *>(plot.itemList(QwtPlotItem::Rtti_PlotCurve).value(i))->sample(j);
        }
    }

    return out;
}

QDataStream &operator>>(QDataStream &in, QwtPlot &plot)
{
    qDebug() << "in >>";
    QwtPlotCurve curve;

    qint32 pointSize;

    while( !in.atEnd()) {
        qDebug() << "in.atEnd():" << in.atEnd();
        QVector<QPointF> v;

        in >> pointSize;
        qDebug() << "pointSize:" << pointSize;
        for(int j = 0; j < pointSize; ++j) {
            QPointF p;
            in >> p;
            v.push_back(p);
        }
        curve.setSamples(v);
        curve.attach(&plot);
    }

    return in;
}

PlotStorage::PlotStorage(QObject *parent) :
    IPlotStorage(parent)
{
}

void PlotStorage::witePlot(const QString &fileName, QwtPlot *plot)
{
//    QFileDialog::getOpenFileName(0, "Open Plot", ".", "Plot Files (*.ngh)");
    qDebug() << "writing...";
    QFile file(fileName);
    file.open(QFile::WriteOnly);
    QDataStream out;
    out.setDevice(&file);
    out << (*plot);
    file.close();
}

QwtPlot *PlotStorage::readPlot(const QString &fileName)
{
    qDebug() << "reading...";
    QwtPlot *plot = new QwtPlot();
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    QDataStream in;
    in.setDevice(&file);
    in >> (*plot);
    file.close();
    qDebug() << "plot->itemList(QwtPlotItem::Rtti_PlotCurve).size():"
                << plot->itemList(QwtPlotItem::Rtti_PlotCurve).size();
    for(int i = 0; i < plot->itemList(QwtPlotItem::Rtti_PlotCurve).size(); ++i) {
        qDebug() << dynamic_cast<QwtPlotCurve *>(plot->itemList(QwtPlotItem::Rtti_PlotCurve).value(i))->data()->sample(0);
    }

    return plot;
}
