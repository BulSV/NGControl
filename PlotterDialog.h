#ifndef PLOTTERDIALOG_H
#define PLOTTERDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QStatusBar>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_picker.h>
#include <qwt_knob.h>
#include <QVector>
#include <QMap>
#include <QCheckBox>
#include <QTime>
#include "LCDSampleSpinBox.h"
#include "MoveSpinBox.h"

class PlotterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PlotterDialog(const QString &title, QWidget *parent = 0);
    void setCurves(const QMap<QString, QPen> &curves);
signals:

public slots:
    void appendData(const QMap<QString, double> &curvesData);
    void updatePlot();
    void resetTime();
    void pauseRessume();
    void toCurrentTime();
private:
    ISpinBox *m_lcdTimeInterval;
    ISpinBox *m_lcdTempInterval;
    ISpinBox *m_msbTimeInterval;
    ISpinBox *m_msbTempInterval;

    QCheckBox *m_cbTimeAccurate;
    QCheckBox *m_cbTempAccurate;

    QPushButton *m_bReset;
    QPushButton *m_bPauseRessume;
    QPushButton *m_bCurrent;

    double m_TimeAccurateFactor;
    double m_TempAccurateFactor;

//    QLabel *m_lStatusBar;
    QStatusBar *m_sbarInfo;
    QwtPlot *m_plot;
    QVector<QwtPlotCurve*> m_Curves;
    QwtPlotPicker *m_picker;

    QTime *m_currentTime;
    QVector<QVector<double> > m_dataAxises;
    QVector<double> m_timeAxis;

    bool m_isReseted;
    bool m_isRessumed;

    double m_prevCurrentTime;
    double m_prevCentralTemp;

    void setupGUI();
    void setupConnections();
    void lcdStyling(QList<QLCDNumber *> &lcdList);
    void autoScroll(const double &elapsedTime);
    double roundToStep(const double &value, const double &step);
private slots:
    void changeTimeInterval();
    void changeTempInterval();
    void moveTimeInterval();
    void moveTempInterval();

    void changeTimeAccurateFactor(bool isChecked);
    void changeTempAccurateFactor(bool isChecked);

    void currentPosText();
};

#endif // PLOTTERDIALOG_H
