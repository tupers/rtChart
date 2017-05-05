#ifndef RTCHART_H
#define RTCHART_H

#include <QObject>
#include <QVector>
#include <QPoint>
#include <QtCharts>
#include <QList>

#define DISPLAY_NUM 100
#define STORED_NUM  500
#define DEFAULT_FREQUENCY   1

class RTChart : public QObject
{
    Q_OBJECT
public:
    explicit RTChart(QObject *parent = 0);
    void setFrequency(int fq){frequency=fq;}
    void updateDisplayData();
    void initChart();
    QChartView* getView(){return chartView;}
    void setYRange(qreal min,qreal max){chart->axisY()->setRange(min,max);}
signals:

public slots:
    void updateData(float indata);
private:
    QVector<float> data;
    QVector<QPointF> dataPoint;
    int frequency=DEFAULT_FREQUENCY;
    int curFrequency=0;
    QLineSeries* series=NULL;
    QChart* chart=NULL;
    QChartView* chartView=NULL;
    int size=0;
};

#endif // RTCHART_H
