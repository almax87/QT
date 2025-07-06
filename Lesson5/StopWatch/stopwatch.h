#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QString>

class Stopwatch : public QObject
{
    Q_OBJECT
public:
    explicit Stopwatch(QObject *parent = nullptr);
    ~Stopwatch();

    void start();
    void stop();
    void reset();
    QString lap();  // Теперь возвращает QString

    bool isRunning() const;
    QString getTime() const;

signals:
    void timeUpdated();  // Оставляем только этот сигнал

private:
    QTimer *timer;
    QTime startTime;
    QTime pausedTime;
    int lapsCount;
    int interval;
    int lastLapMs;
    bool isActive;

    void updateTime();
};

#endif // STOPWATCH_H
