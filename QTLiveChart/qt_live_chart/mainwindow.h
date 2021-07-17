#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>
#include <queue>
#include <deque>

#include "serialread.h"

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
    void on_pushButtonPlot_clicked();
    void on_pushButtonStop_clicked();

public slots:
    void plotSerialData();
    void onReceive(QByteArray inpData);

private:
    Ui::MainWindow *ui;
    QTimer *tmr;
    SerialRead *sr;

    bool flg;
    bool isConnected;

    FixedQueue<float, 50> plotData[6];

    typedef struct
    {
        uint8_t numPlots;
        uint8_t numSamples;
    } SerialPlotConfiguration;
    SerialPlotConfiguration *splc;
};

#endif // MAINWINDOW_H
