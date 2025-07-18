#include "StatisticsDialog.h"
#include "ui_StatisticsDialog.h"
#include <QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <QMessageBox>

StatisticsDialog::StatisticsDialog(DatabaseManager *dbManager, const QString &airportCode, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StatisticsDialog)
    , m_dbManager(dbManager)
    , m_airportCode(airportCode)
{
    ui->setupUi(this);

    // Находим название аэропорта
    QList<Airport> airports = m_dbManager->getAirports();
    for (const Airport &airport : airports) {
        if (airport.code == airportCode) {
            m_airportName = airport.name;
            break;
        }
    }

    setWindowTitle("Статистика загруженности: " + m_airportName);

    // Настройка выпадающего списка месяцев
    QStringList months = {"Январь", "Февраль", "Март", "Апрель", "Май", "Июнь",
                          "Июль", "Август", "Сентябрь", "Октябрь", "Ноябрь", "Декабрь"};
    ui->monthCombo->addItems(months);
    ui->monthCombo->setCurrentIndex(0);

    connect(ui->monthCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StatisticsDialog::onMonthChanged);

    setupYearlyChart();
    setupMonthlyChart(1); // Январь
}

StatisticsDialog::~StatisticsDialog()
{
    delete ui;
}

void StatisticsDialog::onMonthChanged(int index)
{
    setupMonthlyChart(index + 1); // +1 потому что месяцы с 1 до 12
}

void StatisticsDialog::setupYearlyChart()
{
    QMap<QDate, int> yearlyStats = m_dbManager->getYearlyStatistics(m_airportCode);

    QBarSeries *series = new QBarSeries();
    QBarSet *set = new QBarSet("Количество рейсов");

    QStringList categories;
    for (const QDate &date : yearlyStats.keys()) {
        *set << yearlyStats[date];
        categories << date.toString("MMM yyyy");
    }

    series->append(set);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Загруженность аэропорта за год");

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QLayoutItem *oldItem;
    while ((oldItem = ui->yearlyChartLayout->takeAt(0)) != nullptr) {
        delete oldItem->widget();
        delete oldItem;
    }

    ui->yearlyChartLayout->addWidget(chartView);
}

void StatisticsDialog::setupMonthlyChart(int month)
{
    QMap<QDate, int> monthlyStats = m_dbManager->getMonthlyStatistics(m_airportCode, month, 2017);

    QLineSeries *series = new QLineSeries();
    series->setName("Количество рейсов");

    for (const QDate &date : monthlyStats.keys()) {
        series->append(date.day(), monthlyStats[date]);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(QString("Загруженность аэропорта за %1").arg(ui->monthCombo->currentText()));

    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("День месяца");
    axisX->setLabelFormat("%d");
    axisX->setRange(1, 31);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Количество рейсов");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QLayoutItem *oldItem;
    while ((oldItem = ui->monthlyChartLayout->takeAt(0)) != nullptr) {
        delete oldItem->widget();
        delete oldItem;
    }

    ui->monthlyChartLayout->addWidget(chartView);
}
