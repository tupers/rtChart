#include "rtchart.h"

/**
 * @brief RTChart::RTChart
 * @param name
 * @param parent
 */
RTChart::RTChart(QString name,QWidget *parent) : QWidget(parent)
{
    m_ChartName=name;
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

    //    saveButton = createButton("save");
    //    connect(saveButton,&QPushButton::clicked,this,&RTChart::saveData);
    //    m_hCtrlLayout->addWidget(saveButton);
    //    m_hCtrlLayout->setAlignment(saveButton,Qt::AlignRight);

    //    m_hZoomInButton = createButton("x0.5");
    //    connect(m_hZoomInButton,&QPushButton::clicked,[=](){if(displayNum<16)return;displayNum/=2;chartView->chart()->axisX()->setRange(0,displayNum);});
    //    m_hCtrlLayout->addWidget(m_hZoomInButton);
    //    m_hCtrlLayout->setAlignment(m_hZoomInButton,Qt::AlignRight);

    //    m_hZoomOutButton = createButton("x2");
    //    connect(m_hZoomOutButton,&QPushButton::clicked,[=](){if(displayNum>(storedNum/2))return;displayNum*=2;chartView->chart()->axisX()->setRange(0,displayNum);});
    //    m_hCtrlLayout->addWidget(m_hZoomOutButton);
    //    m_hCtrlLayout->setAlignment(m_hZoomOutButton,Qt::AlignRight);

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




    infoLabel = createLabel("no data");
    infoLabel->setMinimumWidth(130);
    infoLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    m_hSubLayout->addWidget(infoLabel);
    m_hSubLayout->setAlignment(infoLabel,Qt::AlignRight|Qt::AlignTop);

    //init data
    data.reserve(storedNum);
    dataPoint.reserve(displayNum);
    for(int i =0;i<displayNum;i++)
        dataPoint.append(QPointF(i,0));


    //init fps counting
    m_fpsString="fps: 0";
    m_dataString="val: 0";

    m_fpsThread = new QThread();
    m_fpsCnt = new fpsCnt();
    m_fpsCnt->moveToThread(m_fpsThread);
    connect(this,&RTChart::updateFps,m_fpsCnt,&fpsCnt::counting);
    connect(m_fpsCnt,&fpsCnt::fpsResult,this,[=](qreal fps){m_fpsString=QString("fps: %1").arg(fps);});
    m_fpsThread->start();
}

RTChart::~RTChart()
{
    m_fpsThread->deleteLater();
    m_fpsCnt->deleteLater();
}

/**
 * @brief RTChart::updateDisplayData
 */
void RTChart::updateDisplayData()
{
    int dataCount = data.count();
    if(dataCount<displayNum)
        for(int i = 0;i<dataCount;i++)
            dataPoint[displayNum-dataCount+i]=QPointF(displayNum-dataCount+i,data.at(i));
    else
        for(int i=0;i<displayNum;i++)
            dataPoint[i]=QPointF(i,data.at(dataCount-displayNum+i));
    QLineSeries* series = (QLineSeries*)chartView->chart()->series()[0];
    if(m_bIsDisplay)
        series->replace(dataPoint);
}

/**
 * @brief RTChart::createChart
 * @return
 */
QChartView* RTChart::createChart()
{
    QLineSeries* series = new QLineSeries();
    series->setUseOpenGL(true);
    QChart* chart = new QChart();
    chart->addSeries(series);
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

/**
 * @brief RTChart::updateData
 * @param indata
 */
void RTChart::updateData(float indata)
{
    emit updateFps();

    if(data.count()>=storedNum)
        data.removeFirst();
    data.append(indata);

    m_dataString=QString("val: %1").arg(indata);
    infoLabel->setText(m_fpsString+" "+m_dataString);
    //    if(m_bIsDisplay)
    //        infoLabel->setText(fpsCnt+" "+val);
    //    else
    //        infoLabel->setText(fpsCnt+"\n"+val);
    curFrequency++;
    if(curFrequency>=frequency)
    {
        updateDisplayData();
        curFrequency=0;
    }
}

/**
 * @brief RTChart::displayCtrl
 */
void RTChart::displayCtrl()
{
    if(isDisplay())
    {
        QLineSeries* series = (QLineSeries*)chartView->chart()->series()[0];
        series->clear();
        m_hSubLayout->setAlignment(infoLabel,Qt::AlignHCenter|Qt::AlignVCenter);
        QFont font = infoLabel->font();
        font.setPointSize(15);
        infoLabel->setFont(font);
        chartView->setVisible(false);
    }
    else
    {
        m_hSubLayout->setAlignment(infoLabel,Qt::AlignRight|Qt::AlignTop);
        QFont font = infoLabel->font();
        font.setPointSize(9);
        infoLabel->setFont(font);
        chartView->setVisible(true);
    }

    setDisplay(!isDisplay());
}
