#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "StatisticsDialog.h"
#include <QMessageBox>
#include <QTimer>
#include <QDate>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_dbManager(new DatabaseManager(this))
{
    ui->setupUi(this);

    // Настройка интерфейса
    ui->dateEdit->setDate(QDate(2016, 8, 15));
    ui->dateEdit->setMinimumDate(QDate(2016, 8, 15));
    ui->dateEdit->setMaximumDate(QDate(2017, 9, 14));

    ui->arrivalsRadio->setChecked(true);
    ui->departuresRadio->setChecked(false);

    setupConnections();

    // Первоначальное подключение к БД
    if (!m_dbManager->connectToDatabase()) {
        QTimer::singleShot(5000, this, [this]() { m_dbManager->connectToDatabase(); });
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupConnections()
{
    connect(m_dbManager, &DatabaseManager::connectionStatusChanged,
            this, &MainWindow::onConnectionStatusChanged);

    connect(ui->loadButton, &QPushButton::clicked,
            this, &MainWindow::onLoadFlightsClicked);
    connect(ui->statisticsButton, &QPushButton::clicked,
            this, &MainWindow::onShowStatisticsClicked);
    connect(ui->dateEdit, &QDateEdit::dateChanged,
            this, &MainWindow::onDateChanged);
    connect(ui->arrivalsRadio, &QRadioButton::toggled,
            this, &MainWindow::onArrivalsToggled);
    connect(ui->departuresRadio, &QRadioButton::toggled,
            this, &MainWindow::onDeparturesToggled);
}

void MainWindow::onConnectionStatusChanged(bool connected)
{
    ui->statusLabel->setText(connected ? "Подключено" : "Отключено");

    if (connected) {
        QList<Airport> airports = m_dbManager->getAirports();
        m_airports = airports;

        ui->airportCombo->clear();
        for (const Airport &airport : airports) {
            ui->airportCombo->addItem(airport.name, airport.code);
        }

        if (!airports.isEmpty()) {
            m_currentAirportCode = airports.first().code;
            enableControls(true);
        }
    } else {
        enableControls(false);
        QTimer::singleShot(5000, this, [this]() { m_dbManager->connectToDatabase(); });
    }
}

void MainWindow::onLoadFlightsClicked()
{
    updateFlightsTable();
}

void MainWindow::onShowStatisticsClicked()
{
    if (m_currentAirportCode.isEmpty()) return;

    StatisticsDialog *dialog = new StatisticsDialog(m_dbManager, m_currentAirportCode, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->exec();
}

void MainWindow::onDateChanged(const QDate &date)
{
    m_currentDate = date;
}

void MainWindow::onArrivalsToggled(bool checked)
{
    m_showArrivals = checked;
}

void MainWindow::onDeparturesToggled(bool checked)
{
    m_showDepartures = checked;
}

void MainWindow::updateFlightsTable()
{
    ui->flightsTable->clearContents();
    ui->flightsTable->setRowCount(0);

    if (m_currentAirportCode.isEmpty()) return;

    if (m_showArrivals) {
        QList<Flight> arrivals = m_dbManager->getArrivals(m_currentAirportCode, m_currentDate);

        ui->flightsTable->setColumnCount(3);
        ui->flightsTable->setHorizontalHeaderLabels({"Номер рейса", "Время прилета", "Аэропорт отправления"});

        for (const Flight &flight : arrivals) {
            int row = ui->flightsTable->rowCount();
            ui->flightsTable->insertRow(row);

            ui->flightsTable->setItem(row, 0, new QTableWidgetItem(flight.flightNumber));
            ui->flightsTable->setItem(row, 1, new QTableWidgetItem(flight.time.toString("hh:mm")));
            ui->flightsTable->setItem(row, 2, new QTableWidgetItem(flight.airportName));
        }
    }

    if (m_showDepartures) {
        QList<Flight> departures = m_dbManager->getDepartures(m_currentAirportCode, m_currentDate);

        ui->flightsTable->setColumnCount(3);
        ui->flightsTable->setHorizontalHeaderLabels({"Номер рейса", "Время вылета", "Аэропорт назначения"});

        for (const Flight &flight : departures) {
            int row = ui->flightsTable->rowCount();
            ui->flightsTable->insertRow(row);

            ui->flightsTable->setItem(row, 0, new QTableWidgetItem(flight.flightNumber));
            ui->flightsTable->setItem(row, 1, new QTableWidgetItem(flight.time.toString("hh:mm")));
            ui->flightsTable->setItem(row, 2, new QTableWidgetItem(flight.airportName));
        }
    }
}

void MainWindow::enableControls(bool enable)
{
    ui->airportCombo->setEnabled(enable);
    ui->dateEdit->setEnabled(enable);
    ui->arrivalsRadio->setEnabled(enable);
    ui->departuresRadio->setEnabled(enable);
    ui->loadButton->setEnabled(enable);
    ui->statisticsButton->setEnabled(enable);
}

void MainWindow::onAirportsLoaded()
{
    // Пример реализации:
    if (m_dbManager) {
        QList<Airport> airports = m_dbManager->getAirports();
        ui->airportCombo->clear();

        for (const Airport &airport : airports) {
            ui->airportCombo->addItem(airport.name, airport.code);
        }

        if (!airports.isEmpty()) {
            m_currentAirportCode = airports.first().code;
            enableControls(true);
        }
    }
}
