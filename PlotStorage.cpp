#include "PlotStorage.h"

#include <QFile>
#include <QDebug>

PlotStorage::PlotStorage(QObject *parent) :
    IPlotStorage(parent)
{
}

void PlotStorage::witePlot(const QString &fileName, QwtPlot *plot)
{
    qDebug() << "writing...";
    QFile file(fileName);
    file.open(QFile::WriteOnly);
    QDataStream out;
    out.setVersion(QDataStream::Qt_5_2);
    out.setDevice(&file);
    out << (*plot);
    file.flush();
    file.close();
}

void PlotStorage::readPlot(const QString &fileName, QwtPlot *plot)
{
    qDebug() << "reading...";
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    QDataStream in;
    in.setVersion(QDataStream::Qt_5_2);
    in.setDevice(&file);
    in >> (*plot);
    file.close();
    qDebug() << "exiting readPlot()...";
}
