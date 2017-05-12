#include "fpscnt.h"

fpsCnt::fpsCnt(int num, QObject *parent) : QObject(parent)
{
    if(num<=0)
        num=1;
    for(int i=0;i<num;i++)
    {
        QElapsedTimer timer;
        m_timerList.append({timer,false,0,0});
    }
}

void fpsCnt::counting(int index)
{
    if(index>m_timerList.count()-1)
        return;
    if(m_timerList.at(index).isStart==false)
    {
        m_timerList[index].timer.start();
        m_timerList[index].isStart=true;
        m_timerList[index].cnt=0;
    }
    m_timerList[index].cnt++;
    int elapsed = m_timerList[index].timer.elapsed();
    if(elapsed>1000)
    {
        m_timerList[index].timer.restart();
        m_timerList[index].fps = qreal(0.1 * int(10000.0 * (qreal(m_timerList[index].cnt) / qreal(elapsed))));
        m_timerList[index].cnt=0;
        emit fpsResult(index,m_timerList[index].fps);
    }
}
