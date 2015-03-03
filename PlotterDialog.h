#ifndef PLOTTERDIALOG_H
#define PLOTTERDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QStatusBar>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_knob.h>
#include <QVector>
#include <QMultiMap>
#include <QLCDNumber>

class PlotterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PlotterDialog(const QString &title, QWidget *parent = 0);
    void setCurves(const QMultiMap<QString, QVector<double> > &curves);
signals:

public slots:
private:
    QLCDNumber *m_lcdTimeInterval;
    QwtKnob *m_knobTimeInterval;

    QLCDNumber *m_lcdInstalledTempInterval;
    QwtKnob *m_knobInstalledTempInterval;

    QLCDNumber *m_lcdSensor1TempInterval;
    QwtKnob *m_knobSensor1TempInterval;

    QLCDNumber *m_lcdSensor2TempInterval;
    QwtKnob *m_knobSensor2TempInterval;

    QStatusBar *m_sbarInfo;
    QwtPlot *m_plot;
    QVector<QwtPlotCurve*> m_Curves;

    void setupGUI();
    void setupConnections();
    void knobStyling(QList<QwtKnob *> &knobList);
    void lcdStyling(QList<QLCDNumber *> &lcdList);
private slots:
    void changeTimeInterval(double interval);
};

#endif // PLOTTERDIALOG_H
