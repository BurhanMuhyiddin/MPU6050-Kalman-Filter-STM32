#include "serialread.h"

SerialRead::SerialRead(QObject *parent) : QObject(parent)
{
    stmBoard = new QSerialPort(this);
    spc = new SerialPortConfiguration();
    isBoardAvailable = false;

    initPort();
}

SerialRead::~SerialRead()
{
    if(stmBoard->isOpen())
        stmBoard->close();

    delete spc;
}

void SerialRead::readSerialData()
{
//    qDebug() << "New data...\n";
    QByteArray inputData = stmBoard->readAll();
    emit receiveData(inputData);
}

void SerialRead::initPort()
{
    //    qDebug() << "Number of available ports: " << QSerialPortInfo::availablePorts().length();
    //    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
    //        qDebug() << "Has vendor ID: " << serialPortInfo.hasVendorIdentifier();
    //        if (serialPortInfo.hasVendorIdentifier())
    //            qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier();
    //        qDebug() << "Has product ID: " << serialPortInfo.hasProductIdentifier();
    //        if (serialPortInfo.hasProductIdentifier())
    //            qDebug() << "Product ID: " << serialPortInfo.productIdentifier();
    //    }
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier())
        {
            if(serialPortInfo.vendorIdentifier() == stmDiscovery_vendorID && serialPortInfo.productIdentifier() == stmDiscovery_productID)
            {
                isBoardAvailable = true;
                spc->portName = serialPortInfo.portName();
            }
        }
    }

    spc->baudRate       = QSerialPort::Baud115200;
    spc->dataBits       = QSerialPort::Data8;
    spc->operatingMode  = QIODevice::ReadWrite;
    spc->parity         = QSerialPort::NoParity;
    spc->stopBit        = QSerialPort::OneStop;
    spc->flowControl    = QSerialPort::NoFlowControl;

    if(isBoardAvailable)
    {
        qDebug() << "Found stmDiscovery port...\n";
        try {
            qDebug() << "Trying to configure and connect the port...\n";
            stmBoard->setPortName(spc->portName);
            qDebug() << Q_FUNC_INFO << stmBoard->open(spc->operatingMode);
            qDebug() << Q_FUNC_INFO << stmBoard->setBaudRate(spc->baudRate);
            qDebug() << Q_FUNC_INFO << stmBoard->setDataBits(spc->dataBits);
            qDebug() << Q_FUNC_INFO << stmBoard->setFlowControl(spc->flowControl);
            qDebug() << Q_FUNC_INFO << stmBoard->setParity(spc->parity);
            qDebug() << Q_FUNC_INFO << stmBoard->setStopBits(spc->stopBit);
            QObject::connect(stmBoard, SIGNAL(readyRead()), this, SLOT(readSerialData()));
            if(stmBoard->isOpen())
                qDebug() << "Port is open...\n";
            else
                qDebug() << "Port is not open...\n";
        } catch (...) {
            qDebug() << "Couldn't connect to the port!\n";
        }
    }
    else
    {
        qDebug() << "Couldn't find stmDiscovery port!\n";
    }
}


