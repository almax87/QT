#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QTime>
#include <QTimer>
#include <QElapsedTimer>


class Stopwatch : public QObject
{
    Q_OBJECT
public:
    explicit Stopwatch(QObject* parent = nullptr);
    ~Stopwatch();

    void start();
    void stop();
    void reset();
    void lap();

    bool isRunning() const;
    QString getTime() const;
    QString getLapTime();

signals:
    void timeUpdated();
    void lapUpdated(const QString &lapInfo);

private:
    QTimer* timer;
    QElapsedTimer elapsedTimer;
    QTime displayTime;
    QElapsedTimer lapTimer;
    int lapsCount;
    int interval;
    qint64 totalElapsed;

    void updateTime();
};

#endif // STOPWATCH_H
