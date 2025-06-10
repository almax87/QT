#include "stopwatch.h"

Stopwatch::Stopwatch(QObject* parent) : QObject{parent}
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
    if (!timer->isActive())
    {
        elapsedTimer.start();
        lapTimer.start();
        timer->start();

        emit timeUpdated();
    }
}

void Stopwatch::stop()
{
    if(timer->isActive())
    {
        timer->stop();
        totalElapsed += elapsedTimer.elapsed();

        emit timeUpdated();
    }
}

void Stopwatch::lap()
{
    if(timer->isActive())
    {
        lapsCount++;
        QString lapInfo = QString("Круг '%1', время: '%2' сек").arg(lapsCount).arg(lapTimer.elapsed());
        lapTimer.restart();
        emit lapUpdated(lapInfo);
    }
}

void Stopwatch::reset()
{
    timer->stop();
    totalElapsed = 0;
    displayTime = QTime(0, 0, 0, 0);
    lapsCount = 0;
    emit timeUpdated();
}

bool Stopwatch::isRunning() const
{
    return timer->isActive();
}

QString Stopwatch::getTime() const
{
    qint64 elapsed = totalElapsed;
    if (timer->isActive())
    {
        elapsed += elapsedTimer.elapsed();
    }

    QTime time = QTime(0, 0, 0, 0).addMSecs(elapsed);
    return QString("%1:%2:%3:%4")
        .arg(time.hour(), 2, 10, QLatin1Char('0'))
        .arg(time.minute(), 2, 10, QLatin1Char('0'))
        .arg(time.second(), 2, 10, QLatin1Char('0'))
        .arg(time.msec()/100, 1, 10, QLatin1Char('0'));
}

QString Stopwatch::getLapTime()
{
    return QString::number(lapTimer.elapsed()/1000.0, 'f', 2);
}

void Stopwatch::updateTime()
{
    emit timeUpdated();
}






































