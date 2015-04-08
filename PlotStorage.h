#ifndef PLOTSTORAGE_H
#define PLOTSTORAGE_H

#include "IPlotStorage.h"
#include <QDataStream>
class PlotStorage : public IPlotStorage
{
    Q_OBJECT
public:
    explicit PlotStorage(QObject *parent = 0);
    virtual ~PlotStorage() {}
    friend QDataStream &operator<<(QDataStream &out, const QwtPlot &plot);
    friend QDataStream &operator>>(QDataStream &in, QwtPlot &plot);
public slots:
    virtual void witePlot(const QString &fileName, QwtPlot *plot);
    virtual QwtPlot *readPlot(const QString &fileName);
};

#endif // PLOTSTORAGE_H
