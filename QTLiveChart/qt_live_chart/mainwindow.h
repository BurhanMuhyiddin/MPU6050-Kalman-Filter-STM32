#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void configureSerialPort();
    void readSerialData();
    void check_vendor_product_id();
    void connectToSerialPort();
    void on_pushButtonPlot_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *stmBoard;
    QList<float> imuData;
    quint16 stmDiscovery_vendorID = 6790;
    quint16 stmDiscovery_productID = 29987;
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
};

#endif // MAINWINDOW_H
