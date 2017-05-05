#include "rtchart.h"

RTChart::RTChart(QObject *parent) : QObject(parent)
{
    //dataPoint.resize(DISPLAY_NUM);
    data.reserve(STORED_NUM);
    dataPoint.reserve(DISPLAY_NUM);
    for(int i=0;i<DISPLAY_NUM;i++)
        dataPoint.append(QPointF(i,0));
    initChart();
}

void RTChart::updateDisplayData()
{
    int dataCount = data.count();
    if(dataCount<DISPLAY_NUM)
        for(int i = 0;i<dataCount;i++)
            dataPoint[DISPLAY_NUM-dataCount+i].setY(data.at(i));
    else
        for(int i=0;i<DISPLAY_NUM;i++)
            dataPoint[i].setY(data.at(dataCount-DISPLAY_NUM+i));
    series->replace(dataPoint);
}

void RTChart::initChart()
{
    series = new QLineSeries();
    series->setUseOpenGL(true);
    chart = new QChart();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::NoAnimation);
    chart->setTheme(QChart::ChartThemeDark);
    chart->axisX()->setRange(0,DISPLAY_NUM);
    chart->axisX()->setVisible(false);
    QValueAxis* axisY = (QValueAxis*)chart->axisY();
    axisY->setTickCount(3);
    chart->setMargins(QMargins(5,5,5,5));
    chart->setBackgroundVisible(false);
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet("background-color:rgba(0,0,0,0)");

}

void RTChart::updateData(float indata)
{
//    qDebug()<<indata;
    if(data.count()>=STORED_NUM)
        data.removeFirst();
    data.append(indata);
    curFrequency++;
    if(curFrequency>=frequency)
    {
        updateDisplayData();
        curFrequency=0;
    }

}
