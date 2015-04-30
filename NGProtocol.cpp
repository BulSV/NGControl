#include "NGProtocol.h"

#ifdef DEBUG
#include <QDebug>
#endif

#define STARTBYTE 0x55
#define STOPBYTE 0xAA
#define BYTESLENGTH 8

#define NEGATIVE 32768 // 2^15
#define OFFSET 65536 // 2^16
#define SLOPE 128

#define CPU_FACTOR 0.537
#define CPU_OFFSET 900
#define CPU_SLOPE 2.95

#define ACCURACY 0.1

#define FORMAT 'f'
#define PRECISION 2

NGProtocol::NGProtocol(ComPort *comPort, QObject *parent) :
    IProtocol(parent),
    itsComPort(comPort),
    itsPrevCPUTemp(0.0),
    itsPrevSensor1Temp(0.0),
    itsPrevSensor2Temp(0.0),
    itsWasPrevCPUTemp(false),
    itsWasPrevSensor1Temp(false),
    itsWasPrevSensor2Temp(false),
    m_resend(new QTimer(this)),
    m_numResends(3),
    m_currentResend(0)
{
    m_resend->setInterval(100);

    connect(itsComPort, SIGNAL(DataIsReaded(bool)), this, SLOT(readData(bool)));
    connect(itsComPort, SIGNAL(DataIsWrited(bool)), this, SIGNAL(DataIsWrited(bool)));
    connect(m_resend, SIGNAL(timeout()), this, SLOT(writeData()));
}

void NGProtocol::setDataToWrite(const QMultiMap<QString, QString> &data)
{
    itsWriteData = data;
}

QMultiMap<QString, QString> NGProtocol::getReadedData() const
{
    return itsReadData;
}

void NGProtocol::readData(bool isReaded)
{
    itsReadData.clear();

    if(isReaded) {
        QByteArray ba;

        ba = itsComPort->getReadData();

        itsReadData.insert(QString("TEMP"), QString::number(negativeTempCorr(wordToInt(ba.mid(1, 2)))));
        itsReadData.insert(QString("SENS1"),
                           QString::number(newSensor(wordToInt(ba.mid(3, 2))), FORMAT, PRECISION));
        itsReadData.insert(QString("SENS2"),
                           QString::number(tempCorr(tempSensors(wordToInt(ba.mid(5, 2))), SENSOR2), FORMAT, PRECISION));

        if( !itsWriteData.isEmpty() && itsReadData.value("TEMP") != itsWriteData.value("TEMP")
                && m_currentResend < m_numResends ) {
            m_resend->start();
            ++m_currentResend;
        } else {
            m_currentResend = 0;
            m_resend->stop();
        }
        emit DataIsReaded(true);
    } else {
        emit DataIsReaded(false);
    }
}

void NGProtocol::writeData()
{
    QByteArray ba;

    ba.append(STARTBYTE);
    ba.append(itsWriteData.value("CODE").toInt());
    ba.append(intToByteArray(itsWriteData.value("TEMP").toInt(), 2).at(0));
    ba.append(intToByteArray(itsWriteData.value("TEMP").toInt(), 2).at(1));
    ba.append('\0');
    ba.append('\0');
    ba.append('\0');
    ba.append(STOPBYTE);

    itsComPort->setWriteData(ba);
#ifdef DEBUG
    for(int i = 0; i < ba.size(); ++i) {
        qDebug() << "ba =" << (int)ba.at(i);
    }
#endif
    itsComPort->writeData();
}

void NGProtocol::resetProtocol()
{
    itsWasPrevCPUTemp = false;
    itsWasPrevSensor1Temp = false;
    itsWasPrevSensor2Temp = false;
}

// преобразует word в byte
int NGProtocol::wordToInt(QByteArray ba)
{
    if(ba.size() != 2)
        return -1;

    int temp = ba[0];
    if(temp < 0)
    {
        temp += 0x100; // 256;
        temp *= 0x100;
    }
    else
        temp = ba[0]*0x100; // старший байт

    int i = ba[1];
    if(i < 0)
    {
        i += 0x100; // 256;
        temp += i;
    }
    else
        temp += ba[1]; // младший байт

    return temp;
}

// определяет температуру
float NGProtocol::tempSensors(int temp)
{
    if(temp & NEGATIVE) {
        return -static_cast<float>(qAbs(temp - OFFSET))/SLOPE;
    } else {
        return static_cast<float>(temp)/SLOPE;
    }
}

// определяет температуру кристалла
float NGProtocol::tempCPU(int temp)
{
    return (static_cast<float>(temp*CPU_FACTOR - CPU_OFFSET))/CPU_SLOPE;
}

float NGProtocol::tempCorr(float temp, NGProtocol::SENSORS sensor)
{
    float prevValue = 0.0;
    bool wasPrev = false;

    switch (sensor) {
    case CPU:
        prevValue = itsPrevCPUTemp;
        wasPrev = itsWasPrevCPUTemp;
        break;
    case SENSOR1:
        prevValue = itsPrevSensor1Temp;
        wasPrev = itsWasPrevSensor1Temp;
        break;
    case SENSOR2:
        prevValue = itsPrevSensor2Temp;
        wasPrev = itsWasPrevSensor2Temp;
        break;
    default:
        prevValue = itsPrevCPUTemp;
        wasPrev = itsWasPrevCPUTemp;
        break;
    }

    if(wasPrev) {
        prevValue = prevValue*(1 - ACCURACY) + temp*ACCURACY;
    } else {
        prevValue = temp;
    }

    switch (sensor) {
    case CPU:
        itsPrevCPUTemp = prevValue;
        itsWasPrevCPUTemp = true;
        break;
    case SENSOR1:
        itsPrevSensor1Temp = prevValue;
        itsWasPrevSensor1Temp = true;
        break;
    case SENSOR2:
        itsPrevSensor2Temp = prevValue;
        itsWasPrevSensor2Temp = true;
        break;
    default:
        itsPrevCPUTemp = prevValue;
        itsWasPrevCPUTemp = true;
        break;
    }

    return prevValue;
}

QString NGProtocol::sensorToString(NGProtocol::SENSORS sensor)
{
    switch (sensor) {
    case CPU:
        return "CPU";
        break;
    case SENSOR1:
        return "SENS1";
        break;
    case SENSOR2:
        return "SENS2";
        break;
    default:
        return "CPU";
        break;
    }
}

QByteArray NGProtocol::intToByteArray(const int &value, const int &numBytes)
{
    QByteArray ba;

    for(int i = numBytes - 1; i > - 1; --i) {
        ba.append((value >> 8*i) & 0xFF);
    }

    return ba;
}

float NGProtocol::negativeTempCorr(int temp)
{
    if(temp & NEGATIVE) {
        return -static_cast<float>(qAbs(temp - OFFSET));
    } else {
        return static_cast<float>(temp);
    }
}

float NGProtocol::newSensor(int temp)
{
    float t = 0.01 * ( (-1.5)*40781 + temp*0.0001 * ( 32791 + temp*0.00001 * ( (-2)*36016 + temp*0.00001 * ( 4*24926 + (-2)*0.00001*28446*temp ) ) ) );
    return t;
}
