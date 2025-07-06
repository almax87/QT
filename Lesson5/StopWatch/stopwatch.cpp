#include "stopwatch.h"

Stopwatch::Stopwatch(QObject *parent) : QObject(parent)
{
    timer = new QTimer(this);
    interval = 100;
    timer->setInterval(interval);
    connect(timer, &QTimer::timeout, this, &Stopwatch::updateTime);
    reset();
}

Stopwatch::~Stopwatch()
{
    delete timer;
}

void Stopwatch::start()
{
    if (!isActive) {
        if (pausedTime.isNull()) {
            // Первый запуск
            startTime = QTime::currentTime();
            lastLapMs = 0;
        } else {
            // Продолжение после паузы
            int pausedDuration = pausedTime.msecsTo(QTime::currentTime());
            startTime = startTime.addMSecs(-pausedDuration);
        }
        isActive = true;
        timer->start();
        emit timeUpdated();
    }
}

void Stopwatch::stop()
{
    if (isActive) {
        timer->stop();
        pausedTime = QTime::currentTime();
        isActive = false;
        emit timeUpdated();
    }
}

void Stopwatch::reset()
{
    timer->stop();
    startTime = QTime();
    pausedTime = QTime();
    lapsCount = 0;
    lastLapMs = 0;
    isActive = false;
    emit timeUpdated();
}

QString Stopwatch::lap()
{
    if (isActive) {
        int currentMs = startTime.msecsTo(QTime::currentTime());
        int lapTimeMs = currentMs - lastLapMs;
        lastLapMs = currentMs;

        lapsCount++;
        return QString("Круг %1, время: %2 сек")
            .arg(lapsCount)
            .arg(lapTimeMs / 1000.0, 0, 'f', 2);
    }
    return QString();
}


bool Stopwatch::isRunning() const
{
    return isActive;
}

QString Stopwatch::getTime() const
{
    int elapsedMs = 0;
    if (isActive) {
        elapsedMs = startTime.msecsTo(QTime::currentTime());
    } else if (!pausedTime.isNull()) {
        elapsedMs = startTime.msecsTo(pausedTime);
    }

    QTime time = QTime(0, 0, 0, 0).addMSecs(elapsedMs);
    return QString("%1:%2:%3.%4")
        .arg(time.hour(), 2, 10, QLatin1Char('0'))
        .arg(time.minute(), 2, 10, QLatin1Char('0'))
        .arg(time.second(), 2, 10, QLatin1Char('0'))
        .arg(time.msec() / 100, 1, 10, QLatin1Char('0'));
}

void Stopwatch::updateTime()
{
    emit timeUpdated();
}
