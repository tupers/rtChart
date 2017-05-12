#include "rtchart.h"

/**
 * @brief RTChart::RTChart
 * @param name
 * @param num
 * @param parent
 */
RTChart::RTChart(QString name,int num,QWidget *parent) : QWidget(parent)
{
    m_ChartName=name;

    //init data
    m_sourceList.reserve(num);
    for(int i=0;i<num;i++)
    {
        QVector<float> data;
        data.reserve(storedNum);
        QVector<QPointF> display;
        for(int j=0;j<displayNum;j++)
            display.append(QPointF(j,0));
        m_sourceList.append({data,display,0,NULL,"fps: 0","val: 0"});
    }

    //init ui
    mainLayout = new QVBoxLayout(this);

    multiLayout = new QStackedLayout;
    multiLayout->setStackingMode(QStackedLayout::StackAll);
    mainLayout->addLayout(multiLayout,0);

    chartView=createChart();
    multiLayout->addWidget(chartView);

    m_hSubWidget = new QWidget;
    m_hSubLayout = new QVBoxLayout;
    m_hSubLayout->setMargin(0);
    m_hSubLayout->setSpacing(0);
    m_hSubWidget->setLayout(m_hSubLayout);
    multiLayout->addWidget(m_hSubWidget);
    multiLayout->setCurrentIndex(multiLayout->count()-1);

    m_hCtrlLayout=new QHBoxLayout;
    m_hCtrlLayout->setMargin(0);
    m_hCtrlLayout->setSpacing(5);
    m_hCtrlLayout->addSpacerItem(new QSpacerItem(1,1,QSizePolicy::Expanding));
    m_hSubLayout->addLayout(m_hCtrlLayout);

    m_hPauseButton = createButton("pause");
    connect(m_hPauseButton,&QPushButton::clicked,[=](){ if(m_hPauseButton->text()=="pause")
            m_hPauseButton->setText("continue");
        else
            m_hPauseButton->setText("pause");
        setDisplay(!isDisplay());});
    m_hCtrlLayout->addWidget(m_hPauseButton);
    m_hCtrlLayout->setAlignment(m_hPauseButton,Qt::AlignRight);

    displayButton = createButton("display");
    connect(displayButton,&QPushButton::clicked,this,&RTChart::displayCtrl);
    m_hCtrlLayout->addWidget(displayButton);
    m_hCtrlLayout->setAlignment(displayButton,Qt::AlignRight);

    for(int i=0;i<num;i++)
    {
        QLineSeries* series = (QLineSeries*)chartView->chart()->series()[i];
        m_sourceList[i].info = createLabel("no data");
        m_sourceList[i].info->setMinimumWidth(130);
        m_sourceList[i].info->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
        m_sourceList[i].info->setStyleSheet(QString("color:rgb(%1,%2,%3)").arg(series->color().red()).arg(series->color().green()).arg(series->color().blue()));
        m_hSubLayout->addWidget(m_sourceList[i].info);
        m_hSubLayout->setAlignment(m_sourceList[i].info,Qt::AlignRight|Qt::AlignTop);
    }

    m_hSubSpacer = new QSpacerItem(1,1,QSizePolicy::Preferred,QSizePolicy::Expanding);
    m_hSubLayout->addSpacerItem(m_hSubSpacer);

    //init fps counting

    m_fpsThread = new QThread();
    m_fpsCnt = new fpsCnt(num);
    m_fpsCnt->moveToThread(m_fpsThread);
    connect(this,&RTChart::updateFps,m_fpsCnt,&fpsCnt::counting);
    connect(m_fpsCnt,&fpsCnt::fpsResult,this,[=](int index,qreal fps){m_sourceList[index].fpsString=QString("fps: %1").arg(fps);});
    m_fpsThread->start();
}

RTChart::~RTChart()
{
    if(m_hSubSpacer!=NULL&&!m_bIsDisplay)
    {
        delete m_hSubSpacer;
        m_hSubSpacer=NULL;
    }
    m_fpsThread->deleteLater();
    m_fpsCnt->deleteLater();
}

/**
 * @brief RTChart::updateDisplayData
 * @param index
 */
void RTChart::updateDisplayData(int index)
{
    int dataCount = m_sourceList.at(index).data.count();
    if(dataCount<displayNum)
        for(int j = 0;j<dataCount;j++)
            m_sourceList[index].display[displayNum-dataCount+j]=QPointF(displayNum-dataCount+j,m_sourceList.at(index).data.at(j));
    else
        for(int j=0;j<displayNum;j++)
            m_sourceList[index].display[j]=QPointF(j,m_sourceList.at(index).data.at(dataCount-displayNum+j));
    QLineSeries* series = (QLineSeries*)chartView->chart()->series()[index];
    if(m_bIsDisplay)
        series->replace(m_sourceList.at(index).display);
}

/**
 * @brief RTChart::createChart
 * @return
 */
QChartView* RTChart::createChart()
{
    QChart* chart = new QChart();
    for(int i=0;i<m_sourceList.count();i++)
    {
        QLineSeries* series = new QLineSeries();
        series->setUseOpenGL(true);
        chart->addSeries(series);
    }
    chart->createDefaultAxes();
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::NoAnimation);
    chart->setTheme(QChart::ChartThemeDark);
    chart->axisX()->setRange(0,displayNum);
    chart->axisX()->setVisible(false);
    QValueAxis* axisY = (QValueAxis*)chart->axisY();
    axisY->setTickCount(3);
    chart->setMargins(QMargins(5,5,5,5));
    chart->setBackgroundVisible(false);
    QChartView* view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->setStyleSheet("background-color:rgba(0,0,0,0)");

    return view;
}

/**
 * @brief RTChart::createButton
 * @return
 */
QPushButton *RTChart::createButton(QString name)
{
    QPushButton* button = new QPushButton(name,this);
    return button;
}

/**
 * @brief RTChart::createLabel
 * @param name
 * @return
 */
QLabel *RTChart::createLabel(QString name)
{
    QLabel* label = new QLabel(name,this);
    label->setStyleSheet("color:white");

    return label;
}

/**
 * @brief RTChart::setDataRange
 * @param min
 * @param max
 */
void RTChart::setDataRange(qreal min, qreal max)
{
    if(min>max)
        return;
    QChart* chart = chartView->chart();
    chart->axisY()->setRange(min,max);
}

void RTChart::clear()
{
    for(int i=0;i<m_sourceList.count();i++)
    {
        QLineSeries* series = (QLineSeries*)chartView->chart()->series()[i];
        series->clear();
    }
}

/**
 * @brief RTChart::updateData
 * @param indata
 * @param index
 */
void RTChart::updateData(float indata, int index)
{
    if(index>m_sourceList.count()-1)
        return;
    emit updateFps(index);

    if(m_sourceList[index].data.count()>=storedNum)
        m_sourceList[index].data.removeFirst();
    m_sourceList[index].data.append(indata);

    m_sourceList[index].dataString=QString("val: %1").arg(indata);
    m_sourceList[index].info->setText(m_sourceList[index].fpsString+" "+m_sourceList[index].dataString);

    m_sourceList[index].curFrequency++;
    if(m_sourceList[index].curFrequency>=frequency)
    {
        updateDisplayData(index);
        m_sourceList[index].curFrequency=0;
    }
}

/**
 * @brief RTChart::displayCtrl
 */
void RTChart::displayCtrl()
{
    if(isDisplay())
    {
        for(int i=0;i<m_sourceList.count();i++)
        {
            QLineSeries* series = (QLineSeries*)chartView->chart()->series()[i];
            series->clear();
            m_hSubLayout->setAlignment(m_sourceList[i].info,Qt::AlignHCenter|Qt::AlignVCenter);
            QFont font = m_sourceList[i].info->font();
            font.setPointSize(15);
            m_sourceList[i].info->setFont(font);
        }
        chartView->setVisible(false);
        m_hSubLayout->removeItem(m_hSubSpacer);
    }
    else
    {
        for(int i=0;i<m_sourceList.count();i++)
        {
            m_hSubLayout->setAlignment(m_sourceList[i].info,Qt::AlignRight|Qt::AlignTop);
            QFont font = m_sourceList[i].info->font();
            font.setPointSize(9);
            m_sourceList[i].info->setFont(font);
        }
        chartView->setVisible(true);
        m_hSubLayout->addSpacerItem(m_hSubSpacer);
    }
    setDisplay(!isDisplay());
}
