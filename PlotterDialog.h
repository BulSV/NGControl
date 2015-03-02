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

class PlotterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PlotterDialog(const QString &title, QWidget *parent = 0);
    void setCurves(const QMultiMap<QString, QVector<double> > &curves);
signals:

public slots:
private:
    QLabel *m_lTimeInterval;
    QwtKnob *m_sdTimeInterval;

    QLabel *m_lInstalledTempInterval;
    QwtKnob *m_sdInstalledTempInterval;

    QLabel *m_lSensor1TempInterval;
    QwtKnob *m_sdSensor1TempInterval;

    QLabel *m_lSensor2TempInterval;
    QwtKnob *m_sdSensor2TempInterval;

    QStatusBar *m_sbarInfo;
    QwtPlot *m_plot;
    QVector<QwtPlotCurve*> m_Curves;

    void setupGUI();
    void knobStyling(QList<QwtKnob *> &knobList);
private slots:
    void changeTimeInterval(double interval);
};

#endif // PLOTTERDIALOG_H
