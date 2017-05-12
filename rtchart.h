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
#include <QSpacerItem>

#include "fpscnt.h"

#define DISPLAY_NUM         100
#define STORED_NUM          500
#define DEFAULT_FREQUENCY   1

typedef struct
{
    QVector<float> data;
    QVector<QPointF> display;
    int curFrequency;
    QLabel* info;
    QString fpsString;
    QString dataString;
}dataHandle;

class RTChart : public QWidget
{
    Q_OBJECT
public:
    explicit RTChart(QString name="",int num=1,QWidget *parent = 0);
    ~RTChart();
    void setFrequency(int fq){frequency=fq>1?fq:1;}
    void updateDisplayData(int index);
    void setDataRange(qreal min,qreal max);
    void setDisplay(bool opt){m_bIsDisplay=opt;}
    bool isDisplay(){return m_bIsDisplay;}
    void clear();

signals:
    void updateFps(int index);
public slots:
    void updateData(float indata,int index);
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
    QSpacerItem* m_hSubSpacer=NULL;
    QWidget* m_hSubWidget;
    QPushButton* saveButton;
    QPushButton* displayButton;
    QPushButton* m_hPauseButton;
    QPushButton* m_hZoomInButton;
    QPushButton* m_hZoomOutButton;
    QChartView* chartView=NULL;
    QVector<dataHandle> m_sourceList;
    int frequency=DEFAULT_FREQUENCY;
    bool m_bIsDisplay=true;
    int displayNum=DISPLAY_NUM;
    int storedNum=STORED_NUM;
    QString m_ChartName;
    fpsCnt* m_fpsCnt;
    QThread* m_fpsThread;
};

#endif // RTCHART_H
