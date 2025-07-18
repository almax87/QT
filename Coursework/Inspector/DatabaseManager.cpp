#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QPSQL");
    m_db.setHostName("981757-ca08998.tmweb.ru");
    m_db.setPort(5432);
    m_db.setDatabaseName("demo");
    m_db.setUserName("netology_usr_cpp");
    m_db.setPassword("CppNeto3");
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::connectToDatabase()
{
    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        Q_EMIT connectionStatusChanged(false);
        return false;
    }

    Q_EMIT connectionStatusChanged(true);
    return true;
}

bool DatabaseManager::isConnected() const
{
    return m_db.isOpen();
}

QString DatabaseManager::lastError() const
{
    return m_lastError;
}

QList<Airport> DatabaseManager::getAirports()
{
    QList<Airport> airports;

    QSqlQuery query("SELECT airport_name->>'ru' as \"airportName\", airport_code FROM bookings.airports_data");

    while (query.next()) {
        Airport airport;
        airport.name = query.value("airportName").toString();
        airport.code = query.value("airport_code").toString();
        airports.append(airport);
    }

    return airports;
}

QList<Flight> DatabaseManager::getArrivals(const QString &airportCode, const QDate &date)
{
    QList<Flight> flights;

    QSqlQuery query;
    query.prepare("SELECT flight_no, scheduled_arrival, ad.airport_name->>'ru' as \"Name\" "
                  "FROM bookings.flights f "
                  "JOIN bookings.airports_data ad on ad.airport_code = f.departure_airport "
                  "WHERE f.arrival_airport = :airportCode AND DATE(scheduled_arrival) = :date");
    query.bindValue(":airportCode", airportCode);
    query.bindValue(":date", date);

    if (query.exec()) {
        while (query.next()) {
            Flight flight;
            flight.flightNumber = query.value("flight_no").toString();
            flight.time = query.value("scheduled_arrival").toDateTime();
            flight.airportName = query.value("Name").toString();
            flights.append(flight);
        }
    } else {
        m_lastError = query.lastError().text();
    }

    return flights;
}

QList<Flight> DatabaseManager::getDepartures(const QString &airportCode, const QDate &date)
{
    QList<Flight> flights;

    QSqlQuery query;
    query.prepare("SELECT flight_no, scheduled_departure, ad.airport_name->>'ru' as \"Name\" "
                  "FROM bookings.flights f "
                  "JOIN bookings.airports_data ad on ad.airport_code = f.arrival_airport "
                  "WHERE f.departure_airport = :airportCode AND DATE(scheduled_departure) = :date");
    query.bindValue(":airportCode", airportCode);
    query.bindValue(":date", date);

    if (query.exec()) {
        while (query.next()) {
            Flight flight;
            flight.flightNumber = query.value("flight_no").toString();
            flight.time = query.value("scheduled_departure").toDateTime();
            flight.airportName = query.value("Name").toString();
            flights.append(flight);
        }
    } else {
        m_lastError = query.lastError().text();
    }

    return flights;
}

QMap<QDate, int> DatabaseManager::getYearlyStatistics(const QString &airportCode)
{
    QMap<QDate, int> statistics;

    QSqlQuery query;
    query.prepare("SELECT count(flight_no), date_trunc('month', scheduled_departure) as \"Month\" "
                  "FROM bookings.flights f "
                  "WHERE (scheduled_departure::date > date('2016-08-31') AND "
                  "scheduled_departure::date <= date('2017-08-31')) AND "
                  "(departure_airport = :airportCode OR arrival_airport = :airportCode) "
                  "GROUP BY \"Month\"");
    query.bindValue(":airportCode", airportCode);

    if (query.exec()) {
        while (query.next()) {
            QDate date = query.value("Month").toDate();
            int count = query.value(0).toInt();
            statistics[date] = count;
        }
    } else {
        m_lastError = query.lastError().text();
    }

    return statistics;
}

QMap<QDate, int> DatabaseManager::getMonthlyStatistics(const QString &airportCode, int month, int year)
{
    QMap<QDate, int> statistics;

    QDate startDate(year, month, 1);
    QDate endDate = startDate.addMonths(1);

    QSqlQuery query;
    query.prepare("SELECT count(flight_no), date_trunc('day', scheduled_departure) as \"Day\" "
                  "FROM bookings.flights f "
                  "WHERE (scheduled_departure::date >= :startDate AND "
                  "scheduled_departure::date < :endDate) AND "
                  "(departure_airport = :airportCode OR arrival_airport = :airportCode) "
                  "GROUP BY \"Day\"");
    query.bindValue(":airportCode", airportCode);
    query.bindValue(":startDate", startDate);
    query.bindValue(":endDate", endDate);

    if (query.exec()) {
        while (query.next()) {
            QDate date = query.value("Day").toDate();
            int count = query.value(0).toInt();
            statistics[date] = count;
        }
    } else {
        m_lastError = query.lastError().text();
    }

    return statistics;
}
