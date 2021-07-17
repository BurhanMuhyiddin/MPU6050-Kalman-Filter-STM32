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
    isConnected = false;

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
        tmr->start(5);
}

void MainWindow::on_pushButtonStop_clicked()
{
    if (tmr->isActive())
        tmr->stop();
}

void MainWindow::plotSerialData()
{
    if (isConnected)
    {
        VectorXd xValues;
        xValues.setLinSpaced(50, 1, 50);

        std::vector<double> xValuesStdVector(xValues.data(), xValues.data() + splc->numSamples);
        std::vector<double> yValuesAccRoll_StdVector(plotData[0].begin(), plotData[0].end());
        std::vector<double> yValuesGyrRoll_StdVector(plotData[1].begin(), plotData[1].end());
        std::vector<double> yValuesKalRoll_StdVector(plotData[2].begin(), plotData[2].end());
        std::vector<double> yValuesAccPitch_StdVector(plotData[3].begin(), plotData[3].end());
        std::vector<double> yValuesGyrPitch_StdVector(plotData[4].begin(), plotData[4].end());
        std::vector<double> yValuesKalPitch_StdVector(plotData[5].begin(), plotData[5].end());

        QVector<double> xValuesQVector = QVector<double>::fromStdVector(xValuesStdVector);
        QVector<double> yValuesAccRoll_QVector = QVector<double>::fromStdVector(yValuesAccRoll_StdVector);
        QVector<double> yValuesGyrRoll_QVector = QVector<double>::fromStdVector(yValuesGyrRoll_StdVector);
        QVector<double> yValuesKalRoll_QVector = QVector<double>::fromStdVector(yValuesKalRoll_StdVector);
        QVector<double> yValuesAccPitch_QVector = QVector<double>::fromStdVector(yValuesAccPitch_StdVector);
        QVector<double> yValuesGyrPitch_QVector = QVector<double>::fromStdVector(yValuesGyrPitch_StdVector);
        QVector<double> yValuesKalPitch_QVector = QVector<double>::fromStdVector(yValuesKalPitch_StdVector);

        float x_maxValue = xValues.maxCoeff();
        float x_minValue = xValues.minCoeff();

        float y_maxValue = 1.0 * 3.14159265359;
        float y_minValue = -1.0 * 3.14159265359;

        QCustomPlot* customPlotRoll = ui->widgetGraphRoll;
        QCustomPlot* customPlotPitch = ui->widgetGraphPitch;

        if (!flg)
        {
            customPlotRoll->legend->setVisible(true);
            customPlotRoll->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);
            customPlotPitch->legend->setVisible(true);
            customPlotPitch->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

            customPlotRoll->addGraph();
            customPlotRoll->graph(0)->setPen(QPen(Qt::red));
            customPlotRoll->graph(0)->setName("Acc roll angle");
            customPlotPitch->addGraph();
            customPlotPitch->graph(0)->setPen(QPen(Qt::red));
            customPlotPitch->graph(0)->setName("Acc pitch angle");

            customPlotRoll->addGraph();
            customPlotRoll->graph(1)->setPen(QPen(Qt::green));
            customPlotRoll->graph(1)->setName("Gyr roll angle");
            customPlotPitch->addGraph();
            customPlotPitch->graph(1)->setPen(QPen(Qt::green));
            customPlotPitch->graph(1)->setName("Gyr pitch angle");

            customPlotRoll->addGraph();
            customPlotRoll->graph(2)->setPen(QPen(Qt::blue));
            customPlotRoll->graph(2)->setName("Kal roll angle");
            customPlotPitch->addGraph();
            customPlotPitch->graph(2)->setPen(QPen(Qt::blue));
            customPlotPitch->graph(2)->setName("Kal pitch angle");

            customPlotRoll->xAxis->setLabel("time");
            customPlotRoll->yAxis->setLabel("roll angle");
            customPlotPitch->xAxis->setLabel("time");
            customPlotPitch->yAxis->setLabel("pitch angle");

            customPlotRoll->xAxis->setRange(x_minValue-0.1*abs(x_minValue), x_maxValue + 0.1*abs(x_maxValue));
            customPlotRoll->yAxis->setRange(y_minValue-0.1*abs(y_minValue), y_maxValue + 0.1*abs(y_maxValue));
            customPlotPitch->xAxis->setRange(x_minValue-0.1*abs(x_minValue), x_maxValue + 0.1*abs(x_maxValue));
            customPlotPitch->yAxis->setRange(y_minValue-0.1*abs(y_minValue), y_maxValue + 0.1*abs(y_maxValue));

            flg = true;
        }

        customPlotRoll->graph(0)->setData(xValuesQVector, yValuesAccRoll_QVector);
        customPlotRoll->graph(1)->setData(xValuesQVector, yValuesGyrRoll_QVector);
        customPlotRoll->graph(2)->setData(xValuesQVector, yValuesKalRoll_QVector);

        customPlotPitch->graph(0)->setData(xValuesQVector, yValuesAccPitch_QVector);
        customPlotPitch->graph(1)->setData(xValuesQVector, yValuesGyrPitch_QVector);
        customPlotPitch->graph(2)->setData(xValuesQVector, yValuesKalPitch_QVector);

        customPlotRoll->replot();
        customPlotPitch->replot();
    }
}

void MainWindow::onReceive(QByteArray inpData)
{
    // data0 : roll_accel
    // data1 : roll_gyro
    // data2 : roll_kalman
    // data3 : pitch_accel
    // data4 : pitch_gyro
    // data5 : pitch_kalman

    if (!flg)
        isConnected = true;

//    qDebug() << inpData.size();
    const float* ptrFloat = reinterpret_cast<const float*>(inpData.constData());
    for (int i = 0; i < splc->numPlots; i++)
    {
//        qDebug() << *ptrFloat;
        plotData[i].push(*ptrFloat);
        ptrFloat++;
    }
//    qDebug() << "---------------------------";
}
