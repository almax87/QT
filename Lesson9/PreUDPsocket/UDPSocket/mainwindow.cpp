#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    udpWorker = new UDPworker(this);
    udpWorker->InitTimeSocket();  // Для временных меток
    udpWorker->InitTextSocket();  // Для текстовых сообщений

    connect(udpWorker, &UDPworker::sig_sendTimeToGUI, this, &MainWindow::DisplayTime);
    connect(udpWorker, &UDPworker::sig_sendTextToGUI, this, &MainWindow::DisplayText);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [&]{
        QDateTime dateTime = QDateTime::currentDateTime();
        QByteArray dataToSend;
        QDataStream outStr(&dataToSend, QIODevice::WriteOnly);
        outStr << dateTime;
        udpWorker->SendTimeDatagram(dataToSend);
    });
}

void MainWindow::on_pb_sendText_clicked()
{
    QString text = ui->le_textInput->text();
    if(!text.isEmpty()) {
        udpWorker->SendTextDatagram(text);
        ui->le_textInput->clear();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pb_start_clicked()
{
    timer->start(TIMER_DELAY);
}

void MainWindow::DisplayTime(QDateTime data)
{
    counterPck++;
    if(counterPck % 20 == 0){
        ui->te_result->clear();
    }
    ui->te_result->append("Текущее время: " + data.toString() + ". "
                                                                "Принято пакетов " + QString::number(counterPck));
}

void MainWindow::on_pb_stop_clicked()
{
    timer->stop();
}

void MainWindow::DisplayText(QString text, QHostAddress senderAddress, quint16 senderPort)
{
    int size = text.toUtf8().size();
    QString message = QString("Принято сообщение от %1:%2, размер: %3 байт\nТекст: %4")
                          .arg(senderAddress.toString())
                          .arg(senderPort)
                          .arg(size)
                          .arg(text);

    ui->te_result->append(message);
}
