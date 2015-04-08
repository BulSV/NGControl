#ifndef IPLOTSTORAGE_H
#define IPLOTSTORAGE_H

#include <QObject>
#include <qwt_plot.h>

class IPlotStorage : public QObject
{
    Q_OBJECT
public:
    explicit IPlotStorage(QObject *parent = 0);
    virtual ~IPlotStorage() {}
signals:
    void plotWritten(bool);
    void plotReaded(bool);
public slots:
    virtual void witePlot(const QString &fileName, QwtPlot *plot) = 0;
    virtual QwtPlot *readPlot(const QString &fileName) = 0;
};

#endif // IPLOTSTORAGE_H
