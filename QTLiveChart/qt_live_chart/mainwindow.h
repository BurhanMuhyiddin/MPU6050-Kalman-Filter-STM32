#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>
#include <queue>
#include <deque>

namespace Ui {
class MainWindow;
}

template <typename T, int MaxLen, typename Container=std::deque<T>>
class FixedQueue : public std::queue<T, Container>
{
public:
    typedef typename Container::const_iterator const_iterator;
    const_iterator begin() const { return this->c.begin(); }
    const_iterator end() const { return this->c.end(); }
    void push(const T& value)
    {
        if(this->size() == MaxLen)
            this->c.pop_front();
        std::queue<T, Container>::push(value);
    }
};

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

    typedef struct
    {
        uint8_t numPlots;
        uint8_t numSamples;
    } SerialPlotConfiguration;
    SerialPlotConfiguration *splc;

    FixedQueue<float, 50> plotData[6];
};

#endif // MAINWINDOW_H
