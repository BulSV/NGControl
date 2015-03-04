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
#include "LCDSampleSpinBox.h"

class PlotterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PlotterDialog(const QString &title, QWidget *parent = 0);
    void setCurves(const QMultiMap<QString, QVector<double> > &curves);
signals:

public slots:
private:
    ISpinBox *m_lcdTimeInterval;
    ISpinBox *m_lcdInstalledTempInterval;
    ISpinBox *m_lcdSensor1TempInterval;
    ISpinBox *m_lcdSensor2TempInterval;

    QStatusBar *m_sbarInfo;
    QwtPlot *m_plot;
    QVector<QwtPlotCurve*> m_Curves;

    void setupGUI();
    void setupConnections();
    void lcdStyling(QList<QLCDNumber *> &lcdList);
    void setColorLCD(QLCDNumber *lcd, bool isHeat);
    QString &addTrailingZeros(QString &str, int prec);
private slots:
    void changeTimeInterval();
    void changeTempInterval();
};

#endif // PLOTTERDIALOG_H
