#include "PlotStorage.h"

#include <QFile>
#include <QDebug>

PlotStorage::PlotStorage(QObject *parent) :
    IPlotStorage(parent)
{
}

void PlotStorage::witePlot(const QString &fileName, QwtPlot *plot) throw(QString)
{
    qDebug() << "writing...";
    if(fileName.isNull()) {
        return;
    }

    QFile file(fileName);

    if( !file.open(QFile::WriteOnly) ) {
        throw QString("File can't be write");
    }

    QDataStream out;
    out.setVersion(QDataStream::Qt_5_2);
    out.setDevice(&file);
    out << (*plot);
    file.flush();
    file.close();
}

void PlotStorage::readPlot(const QString &fileName, QwtPlot *plot) throw(QString)
{
    qDebug() << "reading...";
    if(fileName.isNull()) {
        return;
    }

    QFile file(fileName);

    if(! file.open(QFile::ReadOnly) ) {
        throw QString("File can't be read");
    }

    QDataStream in;
    in.setVersion(QDataStream::Qt_5_2);
    in.setDevice(&file);
    try {
        in >> (*plot);
    } catch (QString &s) {
        throw s;
    }
    file.close();
    qDebug() << "exiting readPlot()...";
}
