#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPieSlice>
#include <QTimer>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    lineChart  = new QChart();
    lineChart2 = new QChart();
    lineChart3 = new QChart();
    pieChart   = new QChart();
    pieChart2  = new QChart();
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onPushButtonClicked);
    connect(ui->windDataButton, &QPushButton::clicked, this, &MainWindow::onWindDataButtonClicked);
    connect(ui->test2button, &QPushButton::clicked, this, &MainWindow::onTestButtonClicked);
    ui->pushButton->setStyleSheet("background-color:white");
    ui->windDataButton->setStyleSheet("background-color:white");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete lineChart;
    delete pieChart;
}

// BP曲线
template<typename T>
void MainWindow::initLineChart(QVector<T> arr1, QVector<T> arr2)
{
    lineChart->removeAllSeries();
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
    pieChart->removeAllSeries();
    QPieSeries* series = new QPieSeries();
    series->setHoleSize(0.45);
    series->append("", value);
    series->append("", 5);
    pieChart->legend()->hide();
    pieChart->setTitle("风速");
    pieChart->addSeries(series);
}

void MainWindow::initPieChart2(double value)
{
    pieChart2->removeAllSeries();
    QPieSeries* series = new QPieSeries();
    series->setHoleSize(0.45);
    series->append("", value);
    series->append("", 90);
    pieChart2->legend()->hide();
    pieChart2->setTitle("风向");
    pieChart2->addSeries(series);
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

}

void MainWindow::timerEvent(QTimerEvent *event)
{
    ApiCaller apiCaller;
    apiCaller.sendRequest("/winddata");
    QJsonObject windData = apiCaller.getResult();
    double dirData = JsonProcess::getValue(windData, "dirData"),
           speedData = JsonProcess::getValue(windData, "speedData");
    initPieChart(speedData);
    initPieChart2(dirData);
    ui->graphicsView_2->setChart(pieChart);
    ui->graphicsView_2->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView_9->setChart(pieChart2);
    ui->graphicsView_9->setRenderHint(QPainter::Antialiasing);
    ui->speed->setText("风速："+QString::number(speedData)+"m/s");
    ui->dir->setText("风向："+QString::number(dirData)+"°");
}

void MainWindow::onWindDataButtonClicked()
{
    timerId = startTimer(5000);
}

void MainWindow::onTestButtonClicked()
{
    QTimer* timer = new QTimer(this);
    timer->start(2000);
    connect(timer, &QTimer::timeout,[&]{
        ApiCaller apiCaller;
        ApiCaller apiCaller2;
        apiCaller.sendRequest("/response");
        int responseCnt = apiCaller.getResult()["cnt"].toInt();
        qDebug() << responseCnt;
        apiCaller.sendRequest("/main");
        apiCaller.sendRequest("/stablizate");
        QJsonObject res = apiCaller.getResult();
        QString status = res["status"].toString();
        if(status == "yes") {
            QVector<double> proPowerData = JsonProcess::toDoubleArray(res, "propowerData"),
                            stbPowerData = JsonProcess::toDoubleArray(res, "stbpowerData"),
                            fnPowerData  = JsonProcess::toDoubleArray(res, "fnpowerData");
            initLineChart2(proPowerData, stbPowerData, fnPowerData);
            ui->pingyiView->setChart(lineChart2);
        } else {
            QVector<double> proPowerData = JsonProcess::toDoubleArray(res, "propowerData");
            initLineChart2(proPowerData);
            ui->pingyiView->setChart(lineChart2);
        }
        if(status == "yes"){
            apiCaller2.sendRequest("/Volatility");
            res = apiCaller2.getResult();
            qDebug() << res;
            QVector<double> vol1Data = JsonProcess::toDoubleArray(res, "vol1Data"),
                            vol2Data = JsonProcess::toDoubleArray(res, "vol2Data"),
                            vol3Data = JsonProcess::toDoubleArray(res, "vol3Data");
            initLineChart3(vol1Data, vol2Data, vol3Data);
            ui->bodongView->setChart(lineChart3);
        }
    });
}

template<typename T>
void MainWindow::initLineChart2(QVector<T> arr1, QVector<T> arr2, QVector<T> arr3)
{
    lineChart2->removeAllSeries();
    QLineSeries* series1 = new QLineSeries();
    QLineSeries* series2 = new QLineSeries();
    QLineSeries* series3 = new QLineSeries();
    int i = 0;
    for(auto& x: arr1) {
        series1->append(i++, x);
    }
    i = 0;
    for(auto& x: arr2) {
        series2->append(i++, x);
    }
    int len = arr3.size();
    for(int i = 15; i < len; i++) {
        series3->append(i-15, arr3[i]);
    }
    lineChart2->legend()->hide();
    lineChart2->addSeries(series1);
    lineChart2->addSeries(series2);
    lineChart2->addSeries(series3);
    series1->setUseOpenGL(true);
    series2->setUseOpenGL(true);
    series3->setUseOpenGL(true);
    lineChart2->createDefaultAxes();
    lineChart2->setTitle(QStringLiteral("平抑效果对比"));
}

template<typename T>
void MainWindow::initLineChart2(QVector<T> arr)
{
    lineChart2->removeAllSeries();
    QLineSeries* series = new QLineSeries();
    int i = 0;
    for(auto& x: arr) {
        series->append(i++, x);
    }
    lineChart2->legend()->hide();
    lineChart2->addSeries(series);
    series->setUseOpenGL(true);
    lineChart2->createDefaultAxes();
    lineChart2->setTitle(QStringLiteral("平抑效果对比"));
}

template<typename T>
void MainWindow::initLineChart3(QVector<T> arr1, QVector<T> arr2, QVector<T> arr3)
{
    lineChart3->removeAllSeries();
    QLineSeries* series1 = new QLineSeries();
    QLineSeries* series2 = new QLineSeries();
    QLineSeries* series3 = new QLineSeries();
    int i = 0;
    for(auto& x: arr1) {
        series1->append(i++, x);
    }
    i = 0;
    for(auto& x: arr2) {
        series2->append(i++, x);
    }
    i = 0;
    for(auto& x: arr3) {
        series3->append(i++, x);
    }
    lineChart3->legend()->hide();
    lineChart3->addSeries(series1);
    lineChart3->addSeries(series2);
    lineChart3->addSeries(series3);
    series1->setUseOpenGL(true);
    series2->setUseOpenGL(true);
    series3->setUseOpenGL(true);
    lineChart3->createDefaultAxes();
    lineChart3->setTitle(QStringLiteral("波动率"));
}
