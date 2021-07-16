#ifndef SERIALREAD_H
#define SERIALREAD_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>
#include <QDebug>
#include <QThread>

class SerialRead : public QObject
{
    Q_OBJECT
public:
    explicit SerialRead(QObject *parent = nullptr);
    ~SerialRead();

signals:
    void receiveData(QByteArray inpData);

public slots:
    void readSerialData();

private:
    QSerialPort *stmBoard;
    QThread *serialPortThread;
    const quint16 stmDiscovery_vendorID = 6790;
    const quint16 stmDiscovery_productID = 29987;
    bool isBoardAvailable;

    typedef struct
    {
        QString portName;
        QIODevice::OpenMode operatingMode;
        QSerialPort::BaudRate baudRate;
        QSerialPort::DataBits dataBits;
        QSerialPort::Parity parity;
        QSerialPort::StopBits stopBit;
        QSerialPort::FlowControl flowControl;
    } SerialPortConfiguration;
    SerialPortConfiguration *spc;

    void initPort();
};

#endif // SERIALREAD_H
