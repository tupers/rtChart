#ifndef RTCHART_H
#define RTCHART_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVector>
#include <QPoint>
#include <QtCharts>
#include <QList>
#include <QStackedLayout>
#include <QDebug>
#include <QThread>

#include "fpscnt.h"

#define DISPLAY_NUM         100
#define STORED_NUM          500
#define DEFAULT_FREQUENCY   1

class RTChart : public QWidget
{
    Q_OBJECT
public:
    explicit RTChart(QString name,QWidget *parent = 0);
    ~RTChart();
    void setFrequency(int fq){frequency=fq>1?fq:1;}
    void updateDisplayData();
    void setDataRange(qreal min,qreal max);
    void setDisplay(bool opt){m_bIsDisplay=opt;}
    bool isDisplay(){return m_bIsDisplay;}

signals:
    void updateFps();
public slots:
    void updateData(float indata);
    void displayCtrl();
protected:
    QChartView* createChart();
    QPushButton* createButton(QString name="");
    QLabel* createLabel(QString name="");
private:
    QVBoxLayout* mainLayout;
    QHBoxLayout* m_hCtrlLayout;
    QStackedLayout* multiLayout;
    QVBoxLayout* m_hSubLayout;
    QWidget* m_hSubWidget;
    QLabel* infoLabel;
    QPushButton* saveButton;
    QPushButton* displayButton;
    QPushButton* m_hPauseButton;
    QPushButton* m_hZoomInButton;
    QPushButton* m_hZoomOutButton;
    QChartView* chartView=NULL;
    QVector<float> data;
    QVector<QPointF> dataPoint;
    qint64 m_lLastTime=0;
    int frequency=DEFAULT_FREQUENCY;
    int curFrequency=0;
    bool m_bIsDisplay=true;
    int displayNum=DISPLAY_NUM;
    int storedNum=STORED_NUM;
    QString m_ChartName;
    QString m_fpsString;
    QString m_dataString;
    fpsCnt* m_fpsCnt;
    QThread* m_fpsThread;
};

#endif // RTCHART_H
