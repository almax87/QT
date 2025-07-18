#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QList>
#include <QDate>

struct Airport {
    QString code;
    QString name;
};

struct Flight {
    QString flightNumber;
    QDateTime time;
    QString airportName;
};

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    bool connectToDatabase();
    bool isConnected() const;
    QString lastError() const;

    QList<Airport> getAirports();
    QList<Flight> getArrivals(const QString &airportCode, const QDate &date);
    QList<Flight> getDepartures(const QString &airportCode, const QDate &date);
    QMap<QDate, int> getYearlyStatistics(const QString &airportCode);
    QMap<QDate, int> getMonthlyStatistics(const QString &airportCode, int month, int year);

signals:
    void connectionStatusChanged(bool connected);

private:
    QSqlDatabase m_db;
    QString m_lastError;
};

#endif // DATABASEMANAGER_H
