#ifndef PLOTSTORAGE_H
#define PLOTSTORAGE_H

#include "IPlotStorage.h"
#include <QDataStream>
#include <qwt_plot_curve.h>
#include <QByteArray>
#include <qwt_plot_marker.h>
#include <QString>
#include <QFont>
#include <QPen>
#include <QDebug>

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
    QByteArray fileFormat;
    fileFormat.append(QString("NGPH").toUtf8());
    out << fileFormat;

    qint32 numCurves = plot.itemList(QwtPlotItem::Rtti_PlotCurve).size();
    out << numCurves;

    for(int i = 0; i < numCurves; ++i) {
        out << static_cast<qint32>( dynamic_cast<QwtPlotCurve *>( plot.itemList(QwtPlotItem::Rtti_PlotCurve).value(i) )->data()->size() );
        for(size_t j = 0; j < dynamic_cast<QwtPlotCurve *>( plot.itemList(QwtPlotItem::Rtti_PlotCurve).value(i) )->data()->size(); ++j) {
            out << dynamic_cast<QwtPlotCurve *>( plot.itemList(QwtPlotItem::Rtti_PlotCurve).value(i) )->sample(j);
        }
    }

    qint32 numMarkers = plot.itemList(QwtPlotItem::Rtti_PlotMarker).size();
    qDebug() << "Number of markers:" << numMarkers;
    out << numMarkers;

    for(int i = 0; i < numMarkers; ++i) {
        QByteArray text = dynamic_cast<QwtPlotMarker *>( plot.itemList(QwtPlotItem::Rtti_PlotMarker).value(i) )->label().text().toUtf8();
        out << static_cast<qint32>( text.size() );
        out << text;
        out << dynamic_cast<QwtPlotMarker *>( plot.itemList(QwtPlotItem::Rtti_PlotMarker).value(i) )->label().color();
        out << dynamic_cast<QwtPlotMarker *>( plot.itemList(QwtPlotItem::Rtti_PlotMarker).value(i) )->label().font();
        out << static_cast<qint32>( dynamic_cast<QwtPlotMarker *>( plot.itemList(QwtPlotItem::Rtti_PlotMarker).value(i) )->yAxis() );
        out << dynamic_cast<QwtPlotMarker *>( plot.itemList(QwtPlotItem::Rtti_PlotMarker).value(i) )->value();
    }

    return out;
}

inline QDataStream &operator>>(QDataStream &in, QwtPlot &plot)
{
    qDebug() << "in >>";

    QByteArray fileFormat;
    qint32 numCurves;
    qint32 pointSize;
    QVector<QwtPlotCurve *> curves;
    qint32 numMarkers;
    QVector<QwtPlotMarker *> markers;
    QVector<QPointF> markersCoordinates;
    QVector<QwtText> markersText;
    QVector<QwtPlot::Axis> markersXAxis;
    QVector<QwtPlot::Axis> markersYAxis;

    in >> fileFormat;
    if( fileFormat != QString("NGPH").toUtf8() ) {
        throw QString("Incorrect file format"); // TODO write code for this option
    }

    // curves
    in >> numCurves;
    for(int i = 0; i < numCurves; ++i) {
        QwtPlotCurve *curve = new QwtPlotCurve;
        QVector<QPointF> curvesPoints;
        QwtPlotCurve *originCurveStyles = dynamic_cast<QwtPlotCurve *>( plot.itemList(QwtPlotItem::Rtti_PlotCurve).value(i) );

        in >> pointSize;
        qDebug() << "pointSize:" << pointSize;
        for(int j = 0; j < pointSize; ++j) {
            QPointF p;
            in >> p;
            curvesPoints.push_back(p);
        }

        curve->setSamples(curvesPoints);
        curve->setRenderHint( QwtPlotItem::RenderAntialiased );
        curve->setLegendAttribute( QwtPlotCurve::LegendShowLine );
        curve->setXAxis( QwtPlot::xBottom );
        curve->setYAxis( originCurveStyles->yAxis() );
        curve->setPen( originCurveStyles->pen() );
        curve->setTitle( originCurveStyles->title() );
        curves.append(curve);
    }
    qDebug() << "detaching curves in >>";
    plot.detachItems(QwtPlotItem::Rtti_PlotCurve);
    qDebug() << "detached curves in >>";
    for(int i = 0; i < curves.size(); ++i) {
        qDebug() << "attaching curves in >>";
        curves[i]->attach(&plot);
    }

    // markers
    in >> numMarkers;
    qDebug() << "Number of markers:" << numMarkers;
    for(int i = 0; i < numMarkers; ++i) {
        QwtPlotMarker *marker = new QwtPlotMarker;
        QwtPlotMarker *originMarkerStyles = dynamic_cast<QwtPlotMarker *>( plot.itemList(QwtPlotItem::Rtti_PlotMarker).value(i) );

        qint32 textBytes;
        in >> textBytes;

        QString textStr;
        for(int j = 0; j < textBytes; ++j) {
            char *c = new char;
            in.readRawData(c, textBytes);
            textStr.append(c);
            delete c;
            c = 0;
        }
        qDebug() << "Marker text:" << textStr;

        QColor color;
        in >> color;

        QFont font;
        in >> font;

        QwtPlot::Axis yAxis;
        qint32 intAxis;
        in >> intAxis;
        yAxis = static_cast<QwtPlot::Axis>( intAxis );

        QPointF coordinate;
        in >> coordinate;
        markersCoordinates.push_back(coordinate);

        QwtText text;
        text.setText(textStr);
        text.setColor(color);
        text.setFont(font);
        text.setBackgroundBrush( originMarkerStyles->label().backgroundBrush() );
        text.setBorderPen( originMarkerStyles->label().borderPen() );
        text.setBorderRadius( originMarkerStyles->label().borderRadius() );

        markersText.push_back(text);
        markersXAxis.push_back( static_cast<QwtPlot::Axis>( originMarkerStyles->xAxis() ) );
        markersYAxis.push_back( static_cast<QwtPlot::Axis>( yAxis ) );
//        marker->setLabel( text );
//        marker->setXAxis( originMarkerStyles->xAxis() );
//        marker->setYAxis( axis );
//        marker->setValue( coordinate );
        markers.append(marker);
    }

    qDebug() << "detaching markers in >>";
    plot.detachItems(QwtPlotItem::Rtti_PlotMarker);
    qDebug() << "detached markers in >>";
    for(int i = 0; i < markers.size(); ++i) {
        qDebug() << "attaching markers in >>";
        markers[i]->attach(&plot);
        markers[i]->setLabel( markersText.at(i) );
        markers[i]->setAxes( markersXAxis.at(i), markersYAxis.at(i) );
        markers[i]->setValue( markersCoordinates.at(i) );
    }

    return in;
}
#endif // PLOTSTORAGE_H
