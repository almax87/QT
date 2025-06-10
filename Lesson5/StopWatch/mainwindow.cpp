#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    stopwatch = new Stopwatch(this);

    ui->pb_start_stop->setText("Старт");
    ui->pb_lap->setText("Круг");
    ui->pb_reset->setText("Сброс");
    ui->pb_lap->setEnabled(false);
    ui->tl_time->setText("00:00:00:00");

    connect(stopwatch, &Stopwatch::timeUpdated, this, &MainWindow::updateTimeDisplay);
    connect(stopwatch, &Stopwatch::lapUpdated, this, &MainWindow::addLap);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete stopwatch;
}

void MainWindow::on_pb_start_stop_clicked()
{
    if (stopwatch->isRunning())
    {
        stopwatch->stop();
        ui->pb_start_stop->setText("Старт");
        ui->pb_lap->setEnabled(false);
    }
    else
    {
        stopwatch->start();
        ui->pb_start_stop->setText("Стоп");
        ui->pb_lap->setEnabled(true);
    }
}

void MainWindow::on_pb_lap_clicked()
{
    stopwatch->lap();
}

void MainWindow::on_pb_reset_clicked()
{
    stopwatch->reset();
    ui->tb_lap_info->clear();
    ui->pb_start_stop->setText("Старт");
    ui->pb_lap->setEnabled(false);
}

void MainWindow::updateTimeDisplay()
{
    ui->tl_time->setText(stopwatch->getTime());
}

void MainWindow::addLap(const QString &lapInfo)
{
    ui->tb_lap_info->append(lapInfo);
}

