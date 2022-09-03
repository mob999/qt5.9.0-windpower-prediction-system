#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPieSlice>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    lineChart = new QChart();
    pieChart = new QChart();
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onPushButtonClicked);
    ui->pushButton->setStyleSheet("background-color:white");
    ui->pushButton_2->setStyleSheet("background-color:white");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete lineChart;
    delete pieChart;
}

template<typename T>
void MainWindow::initLineChart(QVector<T> arr1, QVector<T> arr2)
{
    QLineSeries* series1 = new QLineSeries();
    QLineSeries* series2 = new QLineSeries();
    int i = 0;
    for(auto& x: arr1) {
        series1->append(i++, x);
    }
    i = 0;
    for(auto& x: arr2) {
        series2->append(i++, x);
    }
    lineChart->legend()->hide();
    lineChart->addSeries(series1);
    lineChart->addSeries(series2);
    series1->setUseOpenGL(true);
    series2->setUseOpenGL(true);
    lineChart->createDefaultAxes();
    lineChart->setTitle(QStringLiteral("预测"));
}

void MainWindow::initPieChart(double value)
{
    QPieSeries* series = new QPieSeries();
    series->setHoleSize(0.45);
    series->append("", value);
    series->append("", 200);
    pieChart->legend()->hide();
    pieChart->setTitle("风速");
    pieChart->addSeries(series);
}

void MainWindow::onPushButtonClicked()
{
    ApiCaller apiCaller;
    apiCaller.sendRequest("/BPproduce");
    QJsonObject res = apiCaller.getResult();
    QVector<double> produceData = JsonProcess::toDoubleArray(res, "produceData");
    QVector<double> realData = JsonProcess::toDoubleArray(res, "realData");
    initLineChart(produceData, realData);
    ui->graphicsView->setChart(lineChart);

    initPieChart(100);
    ui->graphicsView_2->setChart(pieChart);
    ui->graphicsView_2->setRenderHint(QPainter::Antialiasing);
}
