#include "fpscnt.h"

fpsCnt::fpsCnt(QObject *parent) : QObject(parent)
{

}

void fpsCnt::counting()
{
    if(m_bIsStart==false)
    {
        m_fpsTimer.start();
        m_bIsStart=true;
        m_nCount=0;
    }
    m_nCount++;
    int elapsed = m_fpsTimer.elapsed();
    if(elapsed>1000)
    {
        m_fpsTimer.restart();
        m_fps = qreal(0.1 * int(10000.0 * (qreal(m_nCount) / qreal(elapsed))));
        m_nCount=0;
        emit fpsResult(m_fps);
    }
}
