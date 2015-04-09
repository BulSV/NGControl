#include "PlotStorage.h"

#include <QFile>
//#include <QFileDialog>
#include <QDebug>

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

void PlotStorage::readPlot(const QString &fileName, QwtPlot *plot)
{
    qDebug() << "reading...";
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    QDataStream in;
    in.setDevice(&file);
    in >> (*plot);
    file.close();
    qDebug() << "plot->itemList(QwtPlotItem::Rtti_PlotCurve).size():"
                << plot->itemList(QwtPlotItem::Rtti_PlotCurve).size();
    for(int i = 0; i < plot->itemList(QwtPlotItem::Rtti_PlotCurve).size(); ++i) {
        for(int j = 0; j < dynamic_cast<QwtPlotCurve *>(plot->itemList(QwtPlotItem::Rtti_PlotCurve).value(i))->data()->size(); ++j)
            qDebug() << dynamic_cast<QwtPlotCurve *>(plot->itemList(QwtPlotItem::Rtti_PlotCurve).value(i))->data()->sample(j);
    }
}
