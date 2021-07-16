// https://stackoverflow.com/questions/25679153/c-threading-pattern-for-qt-serial-port

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

    tmr = new QTimer(this);
    splc = new SerialPlotConfiguration();
    sr = new SerialRead(this);

    splc->numPlots = 6;
    splc->numSamples = 50;

    flg = false;

    connect(tmr, SIGNAL(timeout()), this, SLOT(plotSerialData()));
    connect(sr, SIGNAL(receiveData(QByteArray)), this, SLOT(onReceive(QByteArray)));
}

MainWindow::~MainWindow()
{
    disconnect(tmr, SIGNAL(timeout()), this, SLOT(plotSerialData()));
    disconnect(sr, SIGNAL(receiveData(QByteArray)), this, SLOT(onReceive(QByteArray)));

    if (tmr->isActive())
        tmr->stop();
    tmr->deleteLater();

    delete splc;
    delete sr;
    delete ui;
}

void MainWindow::on_pushButtonPlot_clicked()
{
    if (!tmr->isActive())
        tmr->start(10);
}

void MainWindow::on_pushButtonStop_clicked()
{
    if (tmr->isActive())
        tmr->stop();
}

void MainWindow::plotSerialData()
{
    VectorXd xValues;
    xValues.setLinSpaced(50, 1, 50);

    std::vector<double> xValuesStdVector(xValues.data(), xValues.data() + splc->numSamples);
    std::vector<double> yValuesAccRoll_StdVector(plotData[0].begin(), plotData[0].end());
    std::vector<double> yValuesGyrRoll_StdVector(plotData[1].begin(), plotData[1].end());
    std::vector<double> yValuesKalRoll_StdVector(plotData[2].begin(), plotData[2].end());

    QVector<double> xValuesQVector = QVector<double>::fromStdVector(xValuesStdVector);
    QVector<double> yValuesAccRoll_QVector = QVector<double>::fromStdVector(yValuesAccRoll_StdVector);
    QVector<double> yValuesGyrRoll_QVector = QVector<double>::fromStdVector(yValuesGyrRoll_StdVector);
    QVector<double> yValuesKalRoll_QVector = QVector<double>::fromStdVector(yValuesKalRoll_StdVector);

    float x_maxValue = xValues.maxCoeff();
    float x_minValue = xValues.minCoeff();

    float y_maxValue = 1.0 * 3.14159265359;
    float y_minValue = -1.0 * 3.14159265359;

    QCustomPlot* customPlot = ui->widgetGraph;

    if (!flg)
    {
        customPlot->legend->setVisible(true);
        customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

        customPlot->addGraph();
        customPlot->graph(0)->setPen(QPen(Qt::red));
        customPlot->graph(0)->setName("Acc roll angle");

        customPlot->addGraph();
        customPlot->graph(1)->setPen(QPen(Qt::green));
        customPlot->graph(1)->setName("Gyr roll angle");

        customPlot->addGraph();
        customPlot->graph(2)->setPen(QPen(Qt::blue));
        customPlot->graph(2)->setName("Kal roll angle");

        customPlot->xAxis->setLabel("time");
        customPlot->yAxis->setLabel("roll angle");

        customPlot->xAxis->setRange(x_minValue-0.1*abs(x_minValue), x_maxValue + 0.1*abs(x_maxValue));
        customPlot->yAxis->setRange(y_minValue-0.1*abs(y_minValue), y_maxValue + 0.1*abs(y_maxValue));

        flg = true;
    }

    customPlot->graph(0)->setData(xValuesQVector, yValuesAccRoll_QVector);
    customPlot->graph(1)->setData(xValuesQVector, yValuesGyrRoll_QVector);
    customPlot->graph(2)->setData(xValuesQVector, yValuesKalRoll_QVector);

    customPlot->replot();
}

void MainWindow::onReceive(QByteArray inpData)
{
    // data0 : roll_accel
    // data1 : roll_gyro
    // data2 : roll_kalman
    // data3 : pitch_accel
    // data4 : pitch_gyro
    // data5 : pitch_kalman

    const float* ptrFloat = reinterpret_cast<const float*>(inpData.constData());
    for (int i = 0; i < splc->numPlots; i++)
    {
        plotData[i].push(*ptrFloat);
        ptrFloat++;
    }
}
