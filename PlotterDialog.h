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
#include "LCDSpinBox.h"

class PlotterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PlotterDialog(const QString &title, QWidget *parent = 0);
    void setCurves(const QMultiMap<QString, QVector<double> > &curves);
signals:

public slots:
private:
    LCDSpinBox *m_lcdTimeInterval;
    LCDSpinBox *m_lcdInstalledTempInterval;
    LCDSpinBox *m_lcdSensor1TempInterval;
    LCDSpinBox *m_lcdSensor2TempInterval;

    QStatusBar *m_sbarInfo;
    QwtPlot *m_plot;
    QVector<QwtPlotCurve*> m_Curves;

    void setupGUI();
    void setupConnections();
    void lcdStyling(QList<QLCDNumber *> &lcdList);
    void setColorLCD(QLCDNumber *lcd, bool isHeat);
    QString &addTrailingZeros(QString &str, int prec);
private slots:
    void changeTimeInterval(double interval);
};

#endif // PLOTTERDIALOG_H
