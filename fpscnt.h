#ifndef FPSCNT_H
#define FPSCNT_H

#include <QObject>
#include <QElapsedTimer>

class fpsCnt : public QObject
{
    Q_OBJECT
public:
    explicit fpsCnt(QObject *parent = 0);

signals:
    void fpsResult(qreal fps);
public slots:
    void counting();
private:
    QElapsedTimer m_fpsTimer;
    bool m_bIsStart=false;
    int m_nCount=0;
    qreal m_fps;
};

#endif // FPSCNT_H
