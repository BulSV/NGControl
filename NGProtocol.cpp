#include "NGProtocol.h"

#define STARTBYTE 0x55
#define STOPBYTE 0xAA
#define BYTESLENTH 8

#define NEGATIVE 32768 // 2^15
#define OFFSET 65536 // 2^16
#define SLOPE 128

#define CPU_FACTOR 0.537
#define CPU_OFFSET 900
#define CPU_SLOPE 2.95

#define ACCURACY 0.02

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
    itsWasPrevSensor2Temp(false)
{
    connect(itsComPort, SIGNAL(DataIsReaded(bool)), this, SLOT(readData(bool)));
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

        for(int i = 1, sensor = static_cast<int>(NGProtocol::CPU); i < BYTESLENTH - 1; i += 2, ++sensor) {
            if(sensor != static_cast<int>(NGProtocol::CPU)) {
                itsReadData.insert(sensorToString(static_cast<NGProtocol::SENSORS>(sensor)),
                                   QString::number(tempCorr(tempSensors(wordToInt(ba.mid(i, 2))),
                                                            static_cast<NGProtocol::SENSORS>(sensor)),
                                                   FORMAT, PRECISION));
            } else {
                itsReadData.insert(sensorToString(static_cast<NGProtocol::SENSORS>(sensor)),
                                   QString::number(tempCorr(tempCPU(wordToInt(ba.mid(i, 2))),
                                                            NGProtocol::CPU),
                                                   FORMAT, PRECISION));
            }
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
    ba.append(itsWriteData.value("CODE"));
    ba.append(intToByteArray(itsWriteData.value("TEMP").toInt(), 2).at(0));
    ba.append(intToByteArray(itsWriteData.value("TEMP").toInt(), 2).at(1));
    ba.append('\0');
    ba.append('\0');
    ba.append('\0');
    ba.append(STOPBYTE);

    itsComPort->setWriteData(ba);
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
        return "SENSOR1";
        break;
    case SENSOR2:
        return "SENSOR2";
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
