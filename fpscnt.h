#ifndef FPSCNT_H
#define FPSCNT_H

#include <QObject>
#include <QVector>
#include <QElapsedTimer>

typedef struct
{
    QElapsedTimer timer;
    bool isStart;
    int cnt;
    qreal fps;
}timerHandle;

class fpsCnt : public QObject
{
    Q_OBJECT
public:
    explicit fpsCnt(int num=1,QObject *parent = 0);

signals:
    void fpsResult(int index,qreal fps);
public slots:
    void counting(int index);
private:
    QVector<timerHandle> m_timerList;
};

#endif // FPSCNT_H
