#ifndef PLOTSTORAGE_H
#define PLOTSTORAGE_H

#include "IPlotStorage.h"
#include <QDataStream>
#include <qwt_plot_curve.h>

class PlotStorage : public IPlotStorage
{
    Q_OBJECT
public:
    explicit PlotStorage(QObject *parent = 0);
    virtual ~PlotStorage() {}
public slots:
    virtual void witePlot(const QString &fileName, QwtPlot *plot);
    virtual void readPlot(const QString &fileName, QwtPlot *plot);
};

inline QDataStream &operator<<(QDataStream &out, const QwtPlot &plot)
{
    qDebug() << "out <<";
    for(int i = 0; i < plot.itemList(QwtPlotItem::Rtti_PlotCurve).size(); ++i) {
        out << static_cast<qint32>(dynamic_cast<QwtPlotCurve *>(plot.itemList(QwtPlotItem::Rtti_PlotCurve).value(i))->data()->size());
        for(int j = 0; j < dynamic_cast<QwtPlotCurve *>(plot.itemList(QwtPlotItem::Rtti_PlotCurve).value(i))->data()->size(); ++j) {
            out << dynamic_cast<QwtPlotCurve *>(plot.itemList(QwtPlotItem::Rtti_PlotCurve).value(i))->sample(j);
        }
    }

    return out;
}

inline QDataStream &operator>>(QDataStream &in, QwtPlot &plot)
{
    qDebug() << "in >>";
    QwtPlotCurve curve;

    qint32 pointSize;

    for(int i = 0; i < 3; ++i) {
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
#endif // PLOTSTORAGE_H
