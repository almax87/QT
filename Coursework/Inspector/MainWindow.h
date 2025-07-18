#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>
#include "DatabaseManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConnectionStatusChanged(bool connected);
    void onAirportsLoaded();
    void onLoadFlightsClicked();
    void onShowStatisticsClicked();
    void onDateChanged(const QDate &date);
    void onArrivalsToggled(bool checked);
    void onDeparturesToggled(bool checked);

private:
    void setupConnections();
    void updateFlightsTable();
    void enableControls(bool enable);

    Ui::MainWindow *ui;
    DatabaseManager *m_dbManager;
    QList<Airport> m_airports;
    QString m_currentAirportCode;
    QDate m_currentDate;
    bool m_showArrivals = true;
    bool m_showDepartures = false;
};

#endif // MAINWINDOW_H
