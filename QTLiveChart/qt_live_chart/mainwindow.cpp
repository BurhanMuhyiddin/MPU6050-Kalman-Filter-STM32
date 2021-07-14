#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <Eigen/Dense>
#include <math.h>
#include <QDebug>
#include <QMessageBox>

using namespace Eigen;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->buttonGroupRadio->setId(ui->radioButtonQuadratic, 1);
    ui->buttonGroupRadio->setId(ui->radioButtonCubic, 2);

    ui->radioButtonQuadratic->setChecked(true);

    spc = new SerialPortConfiguration;
    splc = new SerialPlotConfiguration;
    stmBoard = new QSerialPort(this);
    isBoardAvailable = false;

    splc->numPlots = 6;
    splc->numSamples = 50;

    check_vendor_product_id();
    connectToSerialPort();
}

MainWindow::~MainWindow()
{
    if(stmBoard->isOpen())
        stmBoard->close();

    delete spc;
    delete splc;
    delete stmBoard;

    delete ui;
}

void MainWindow::on_pushButtonPlot_clicked()
{
//    int functionType = ui->buttonGroupRadio->checkedId();

    VectorXd xValues;
    xValues.setLinSpaced(50, 1, 50);

    std::vector<double> xValuesStdVector(xValues.data(), xValues.data() + splc->numSamples);
    std::vector<double> yValuesStdVector(plotData[0].begin(), plotData[0].end());

    QVector<double> xValuesQVector = QVector<double>::fromStdVector(xValuesStdVector);
    QVector<double> yValuesQVector = QVector<double>::fromStdVector(yValuesStdVector);

    float x_maxValue = xValues.maxCoeff();
    float x_minValue = xValues.minCoeff();

    float y_maxValue = 1.0 * 3.14159265359;
    float y_minValue = -1.0 * 3.14159265359;

    QCustomPlot* customPlot = ui->widgetGraph;

    customPlot->addGraph();
    customPlot->graph(0)->setData(xValuesQVector, yValuesQVector);

    customPlot->xAxis->setLabel("time");
    customPlot->yAxis->setLabel("accel roll");

    customPlot->xAxis->setRange(x_minValue-0.1*abs(x_minValue), x_maxValue + 0.1*abs(x_maxValue));
    customPlot->yAxis->setRange(y_minValue-0.1*abs(y_minValue), y_maxValue + 0.1*abs(y_maxValue));
    customPlot->replot();
}

void MainWindow::check_vendor_product_id()
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
}

void MainWindow::configureSerialPort()
{
    spc->baudRate       = QSerialPort::Baud115200;
    spc->dataBits       = QSerialPort::Data8;
    spc->operatingMode  = QIODevice::ReadWrite;
    spc->parity         = QSerialPort::NoParity;
    spc->stopBit        = QSerialPort::OneStop;
    spc->flowControl    = QSerialPort::NoFlowControl;
}

void MainWindow::connectToSerialPort()
{
    if(isBoardAvailable)
    {
        qDebug() << "Found stmDiscovery port...\n";
        try {
            qDebug() << "Trying to configure and connect the port...\n";
            configureSerialPort();

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
            QMessageBox::warning(this, "Connection Error", "Couldn't connect to the board!");
        }
    }
    else
    {
        qDebug() << "Couldn't find stmDiscovery port!\n";
        QMessageBox::warning(this, "Port error", "Couldn't find the board!");
    }
}

void MainWindow::readSerialData()
{
    // data0 : roll_accel
    // data1 : roll_gyro
    // data2 : roll_kalman
    // data3 : pitch_accel
    // data4 : pitch_gyro
    // data5 : pitch_kalman
    const QByteArray data = stmBoard->readAll();
    const float* ptrFloat = reinterpret_cast<const float*>(data.constData());
//    int numSamples = data.size() / 4;
    for (int i = 0; i < splc->numPlots; i++)
    {
//        qDebug() << *ptrFloat;
//        imuData.append(*ptrFloat);
//        qDebug() << "Reading data...\n";
        plotData[i].push(*ptrFloat);
        ptrFloat++;
    }
//    qDebug() << "Finished reading...\n";
//    qDebug() << "----------------------------\n";
}
