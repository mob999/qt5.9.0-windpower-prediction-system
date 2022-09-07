#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "apicaller.h"
#include "jsonprocess.h"
#include <QChartView>
#include <QLineSeries>
#include <QSharedPointer>
#include <QVector>
#include <QPieSeries>
namespace Ui {

class MainWindow;
}
QT_CHARTS_USE_NAMESPACE
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

    template<typename T>
    void initLineChart(QVector<T> arr1, QVector<T> arr2);

    template<typename T>
    void initLineChart2(QVector<T> arr1, QVector<T> arr2, QVector<T> arr3);

    template<typename T>
    void initLineChart2(QVector<T> arr);

    template<typename T>
    void initLineChart3(QVector<T> arr1, QVector<T> arr2, QVector<T> arr3);

    void initPieChart(double value);

    void initPieChart2(double value);

    void initPieChartPro(double value);

    void initPieChart2Pro(double value);

    void timerEvent(QTimerEvent *event) override;

    void windDataDisplay();

    void propowerDisplay();

    void initPropowerChart(QVector<double> arr1, QVector<double> arr2);

private:
    Ui::MainWindow *ui;
    QChart* lineChart;
    QChart* lineChart2;
    QChart* lineChart3;
    QChart* pieChart;
    QChart* pieChart2;
    QChart* propowerChart;
    QChart* pieChartPro;
    QChart* pieChart2Pro;
    int timerId;

public slots:
    void onPushButtonClicked();
    void onWindDataButtonClicked();
    void onTestButtonClicked();
    void onPcsBtnClicked();
};

#endif // MAINWINDOW_H
