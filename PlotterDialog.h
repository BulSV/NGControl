#ifndef PLOTTERDIALOG_H
#define PLOTTERDIALOG_H

#ifdef DEBUG
#include <QDebug>
#endif

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_picker.h>
#include <qwt_knob.h>
#include <QVector>
#include <QMap>
#include <QCheckBox>
#include <QTime>
#include <QComboBox>
#include <QGroupBox>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>
#include <QTimer>
#include <QLCDNumber>
#include <QSpinBox>
#include <QRadioButton>
#include <QButtonGroup>

#include "ComPort.h"
#include "IProtocol.h"
#include "NGProtocol.h"
#include "LCDSpinBox.h"
#include "LCDSampleSpinBox.h"
#include "MoveSpinBox.h"

class PlotterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PlotterDialog(const QString &title, QWidget *parent = 0);
    ~PlotterDialog();
    void setCurves(const QMap<QString, QPen> &curves,
                   const QMap<QString, QwtPlot::Axis> &axis);
signals:

public slots:
    void appendData(const QMap<QString, double> &curvesData);
    void updatePlot();
    void resetTime();
    void pauseRessume();
    void toCurrentTime();
private:
    ISpinBox *m_lcdTimeInterval;
    ISpinBox *m_lcdTempIntervalLeft;
    ISpinBox *m_lcdTempIntervalRight;
    ISpinBox *m_msbTimeInterval;
    ISpinBox *m_msbTempIntervalLeft;
    ISpinBox *m_msbTempIntervalRight;

    QCheckBox *m_cbTimeAccurate;
    QCheckBox *m_cbTempAccurateLeft;
    QCheckBox *m_cbTempAccurateRight;

    QPushButton *m_bReset;
    QPushButton *m_bPauseRessume;

    double m_TimeAccurateFactor;
    double m_TempAccurateFactorLeft;
    double m_TempAccurateFactorRight;

    QwtPlot *m_plot;
    QVector<QwtPlotCurve*> m_Curves;
    QwtPlotPicker *m_pickerLeft;
    QwtPlotPicker *m_pickerRight;

    QTime *m_currentTime;
    QMap<QString, QVector<double> > m_dataAxises;
    QVector<double> m_timeAxis;

    bool m_isReseted;
    bool m_isRessumed;

    double m_prevCurrentTime;
    double m_prevCentralTempLeft;
    double m_prevTempOffsetLeft;
    double m_prevCentralTempRight;
    double m_prevTempOffsetRight;

    QRadioButton *m_rbRelateLeft;
    QRadioButton *m_rbRelateRight;
    QButtonGroup *m_bgRelate;

    QLabel *lPort;
    QComboBox *cbPort;
    QLabel *lBaud;
    QComboBox *cbBaud;
    QPushButton *bPortStart;
    QPushButton *bPortStop;
    QCheckBox *chbSynchronize;
    QLabel *lTx;
    QLabel *lRx;

    ISpinBox *sbSetTemp;
    QLCDNumber *lcdInstalledTemp;
    QLCDNumber *lcdSensor1Termo;
    QLCDNumber *lcdSensor2Termo;

    QPushButton *bSetTemp;

    QGroupBox *gbSetTemp;

    QSerialPort *itsPort;
    ComPort *itsComPort;
    IProtocol *itsProtocol;

    QStringList itsSensorsList;

    void setupGUI();
    void setupConnections();

    // цвет индикации температуры >0 & <=0
    void setColorLCD(QLCDNumber *lcd, bool isHeat);
    // добавляет завершающие нули
    QString &addTrailingZeros(QString &str, int prec);

    // setting color LCD
    void setColorLCD(QLCDNumber *lcd, const QColor &color);

    QTimer *itsBlinkTimeTxNone;
    QTimer *itsBlinkTimeRxNone;
    QTimer *itsBlinkTimeTxColor;
    QTimer *itsBlinkTimeRxColor;
    QTimer *itsTimeToDisplay;

    void lcdStyling(QList<QLCDNumber *> &lcdList);
    void autoScroll(const double &elapsedTime);
    double roundToStep(const double &value, const double &step);
private slots:
    void changeTimeInterval();
    void moveTimeInterval();

    void changeTempIntervalLeft();
    void moveTempIntervalLeft();

    void changeTempIntervalRight();
    void moveTempIntervalRight();

    void changeTimeAccurateFactor(bool isChecked);
    void changeTempAccurateFactorLeft(bool isChecked);
    void changeTempAccurateFactorRight(bool isChecked);

    void radioButtonClicked(int id);

    void openPort();
    void closePort();
    void received(bool isReceived);
    void writeTemp();
    // мигание надписей "Rx" - при получении и "Tx" - при отправке пакета
    void colorTxNone();
    void colorRxNone();
    void colorIsTx();
    void colorIsRx();
    // display current Rx data
    void display();
    // colored set temp LCD if value < 0
    void colorSetTempLCD();
};

#endif // PLOTTERDIALOG_H
