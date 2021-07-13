/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QCustomPlot *widgetGraph;
    QRadioButton *radioButtonQuadratic;
    QRadioButton *radioButtonCubic;
    QPushButton *pushButtonPlot;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QButtonGroup *buttonGroupRadio;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(613, 458);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        widgetGraph = new QCustomPlot(centralWidget);
        widgetGraph->setObjectName(QStringLiteral("widgetGraph"));
        widgetGraph->setGeometry(QRect(170, 100, 431, 291));
        radioButtonQuadratic = new QRadioButton(centralWidget);
        buttonGroupRadio = new QButtonGroup(MainWindow);
        buttonGroupRadio->setObjectName(QStringLiteral("buttonGroupRadio"));
        buttonGroupRadio->addButton(radioButtonQuadratic);
        radioButtonQuadratic->setObjectName(QStringLiteral("radioButtonQuadratic"));
        radioButtonQuadratic->setGeometry(QRect(20, 110, 112, 23));
        radioButtonCubic = new QRadioButton(centralWidget);
        buttonGroupRadio->addButton(radioButtonCubic);
        radioButtonCubic->setObjectName(QStringLiteral("radioButtonCubic"));
        radioButtonCubic->setGeometry(QRect(20, 150, 112, 23));
        pushButtonPlot = new QPushButton(centralWidget);
        pushButtonPlot->setObjectName(QStringLiteral("pushButtonPlot"));
        pushButtonPlot->setGeometry(QRect(20, 290, 89, 25));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 613, 22));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        radioButtonQuadratic->setText(QApplication::translate("MainWindow", "quadratic", Q_NULLPTR));
        radioButtonCubic->setText(QApplication::translate("MainWindow", "cubic", Q_NULLPTR));
        pushButtonPlot->setText(QApplication::translate("MainWindow", "Plot", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
