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
    void initPieChart(double value);
private:
    Ui::MainWindow *ui;
    QChart* lineChart;
    QChart* pieChart;

public slots:
    void onPushButtonClicked();
};

#endif // MAINWINDOW_H
