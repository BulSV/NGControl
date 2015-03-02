#ifndef PLOTTERDIALOG_H
#define PLOTTERDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QLabel>
#include <QStatusBar>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
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
    QSpinBox *m_sbInterval;
    QStatusBar *m_sbarInfo;
    QwtPlot *m_plot;
    QVector<QwtPlotCurve*> m_Curves;

    void setupGUI();
private slots:
    void changeInterval(int interval);
};

#endif // PLOTTERDIALOG_H
