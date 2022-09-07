#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPieSlice>
#include <QTimer>
#include <QtCharts>
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
    pieChartPro   = new QChart();
    pieChart2Pro  = new QChart();
    propowerChart = new QChart();
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onPushButtonClicked);
    connect(ui->windDataButton, &QPushButton::clicked, this, &MainWindow::onWindDataButtonClicked);
    connect(ui->test2button, &QPushButton::clicked, this, &MainWindow::onTestButtonClicked);
    connect(ui->pcsBtn, &QPushButton::clicked, this, &MainWindow::onPcsBtnClicked);
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

void MainWindow::windDataDisplay()
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

void MainWindow::initPropowerChart(QVector<double> arr1, QVector<double> arr2)
{
    propowerChart->removeAllSeries();
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
    propowerChart->addSeries(series1);
    propowerChart->addSeries(series2);
    propowerChart->setTitle("propower");
}

void MainWindow::propowerDisplay()
{
    ApiCaller apiCaller;
    apiCaller.sendRequest("/response");
    int cnt = apiCaller.getResult()["cnt"].toInt();
    apiCaller.sendRequest("/powerpro");
    QJsonObject propower = apiCaller.getResult();
    QVector<double> propowerData, realpowerData;
    if(cnt >= 17) {
        propowerData  = JsonProcess::toDoubleArray(propower, "propowerData" );
        realpowerData = JsonProcess::toDoubleArray(propower, "realpowerData");
    } else {
        propowerData  = JsonProcess::toDoubleArray(propower, "propowerData");
    }
    initPropowerChart(propowerData, realpowerData);
    ui->powerproView->setChart(propowerChart);

}

void MainWindow::timerEvent(QTimerEvent *event)
{
    windDataDisplay();
    propowerDisplay();
}

void MainWindow::onWindDataButtonClicked()
{
    timerId = startTimer(1000);
}

void MainWindow::onTestButtonClicked()
{
    QTimer* timer = new QTimer(this);
    timer->start(1000);
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
        QVector<double> proPowerData, stbPowerData, fnPowerData;
        if(status == "yes") {
            proPowerData = JsonProcess::toDoubleArray(res, "propowerData");
            stbPowerData = JsonProcess::toDoubleArray(res, "stbpowerData");
            fnPowerData  = JsonProcess::toDoubleArray(res, "fnpowerData");
            ui->idealPingyiText->setText("理想平抑："+QString::number(stbPowerData.back()));
            ui->realPingyiText->setText("实际平抑："+QString::number(fnPowerData.back()));
            ui->nullPingyiText->setText("未平抑："+QString::number(proPowerData.back()));
        } else {
            proPowerData = JsonProcess::toDoubleArray(res, "propowerData");
            ui->nullPingyiText->setText("未平抑："+QString::number(proPowerData.back()));
        }
        initLineChart2(proPowerData, stbPowerData, fnPowerData);
        ui->pingyiView->setChart(lineChart2);
        if(status == "yes"){
            apiCaller2.sendRequest("/Volatility");
            res = apiCaller2.getResult();
            qDebug() << res;
            QVector<double> vol1Data = JsonProcess::toDoubleArray(res, "vol1Data"),
                            vol2Data = JsonProcess::toDoubleArray(res, "vol2Data"),
                            vol3Data = JsonProcess::toDoubleArray(res, "vol3Data");
            initLineChart3(vol1Data, vol2Data, vol3Data);
            ui->bodongView->setChart(lineChart3);
            ui->realBodongText->setText("实际波动："+QString::number(vol3Data.back()));
        }
        apiCaller.sendRequest("/winddatapro");
        double speed = apiCaller.getResult()["speedDatapro"].toDouble(),
               dir   = apiCaller.getResult()["dirDatapro"].toDouble();
        qDebug() << speed <<  " " << dir;
        initPieChartPro(speed);
        initPieChart2Pro(dir);
        ui->speedView2->setChart(pieChartPro);
        ui->dirView2->setChart(pieChart2Pro);
        ui->speed_2->setText(QString::number(speed)+"m/s");
        ui->dir_2->setText(QString::number(dir)+"°");
    });
}

void MainWindow::initPieChartPro(double value)
{
    QPieSeries* series = new QPieSeries();
    series->setHoleSize(0.45);
    series->append("", value);
    series->append("", 5);
    pieChartPro->legend()->hide();
    pieChartPro->setTitle("风速");
    pieChartPro->addSeries(series);
}

void MainWindow::initPieChart2Pro(double value)
{
    QPieSeries* series = new QPieSeries();
    series->setHoleSize(0.45);
    series->append("", value);
    series->append("", 90);
    pieChart2Pro->legend()->hide();
    pieChart2Pro->setTitle("风向");
    pieChart2Pro->addSeries(series);
}


template<typename T>
void MainWindow::initLineChart2(QVector<T> arr1, QVector<T> arr2, QVector<T> arr3)
{
    lineChart2->removeAllSeries();
    lineChart2->removeAxis( lineChart2->axisX());
    lineChart2->removeAxis( lineChart2->axisY());
    QLineSeries* series1 = new QLineSeries();
    QLineSeries* series2 = new QLineSeries();
    QLineSeries* series3 = new QLineSeries();
    series1->setName("提前预测功率曲线");   //添加折线名字
    series2->setName("理想平抑功率曲线");
    series3->setName("实际平抑功率曲线");
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
    lineChart2->addSeries(series1);
    lineChart2->addSeries(series2);
    lineChart2->addSeries(series3);
    series1->setUseOpenGL(true);
    series2->setUseOpenGL(true);
    series3->setUseOpenGL(true);
    QValueAxis *axisX = new QValueAxis;
    axisX->setTitleText("时间 min");//X轴
    QValueAxis *axisY = new QValueAxis; //Y 轴
    axisY->setRange(0, 1500);
    axisY->setTitleText("功率 KW");
    lineChart2->setAxisX(axisX, series1);
    lineChart2->setAxisY(axisY, series1);
    lineChart2->setAxisX(axisX, series2);
    lineChart2->setAxisY(axisY, series2);
    lineChart2->setAxisX(axisX, series3);
    lineChart2->setAxisY(axisY, series3);
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
    series->setName("提前预测功率曲线");   //添加折线名字
    QValueAxis *axisX = new QValueAxis;
    axisX->setTitleText("时间 min");//X轴
    QValueAxis *axisY = new QValueAxis; //Y 轴
    axisY->setRange(0, 1500);
    axisY->setTitleText("功率 KW");
    series->setUseOpenGL(true);
    lineChart2->setAxisX(axisX, series);
    lineChart2->setAxisY(axisY, series);
    lineChart2->addSeries(series);
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

void MainWindow::onPcsBtnClicked()
{
    system("test.exe");
}
