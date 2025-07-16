#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    client = new TCPclient(this);
    //Доступность полей по умолчанию
    ui->le_data->setEnabled(false);
    ui->pb_request->setEnabled(false);
    ui->lb_connectStatus->setText("Отключено");
    ui->lb_connectStatus->setStyleSheet("color: red");


    //При отключении меняем надписи и доступность полей.
    connect(client, &TCPclient::sig_Disconnected, this, [&]{
        ui->lb_connectStatus->setText("Отключено");
        ui->lb_connectStatus->setStyleSheet("color: red");
        ui->pb_connect->setText("Подключиться");
        ui->le_data->setEnabled(false);
        ui->pb_request->setEnabled(false);
        ui->spB_port->setEnabled(true);
        ui->spB_ip1->setEnabled(true);
        ui->spB_ip2->setEnabled(true);
        ui->spB_ip3->setEnabled(true);
        ui->spB_ip4->setEnabled(true);
    });

    // Соединяем сигналы со слотами
    connect(client, &TCPclient::sig_sendTime, this, &MainWindow::DisplayTime);
    connect(client, &TCPclient::sig_sendStat, this, &MainWindow::DisplayStat);
    connect(client, &TCPclient::sig_connectStatus, this, &MainWindow::DisplayConnectStatus);
     connect(client, &TCPclient::sig_sendFreeSize, this, &MainWindow::DisplayFreeSpace);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete client;  // Добавляем освобождение памяти для client
}

// ... остальные методы MainWindow остаются без изменений ...

void MainWindow::DisplayTime(QDateTime time)
{
    ui->tb_result->append("Текущие время и дата на сервере: " + time.toString("dd MMM yyyy hh:mm:ss"));
}

void MainWindow::DisplayStat(StatServer stat)
{
    ui->tb_result->append("Статистика сервера:");
    ui->tb_result->append("Принято байт: " + QString::number(stat.incBytes));
    ui->tb_result->append("Отправлено байт: " + QString::number(stat.sendBytes));
    ui->tb_result->append("Принято пакетов: " + QString::number(stat.revPck));
    ui->tb_result->append("Отправлено пакетов: " + QString::number(stat.sendPck));
    ui->tb_result->append("Время работы сервера (сек): " + QString::number(stat.workTime));
    ui->tb_result->append("Количество подключенных клиентов: " + QString::number(stat.clients));
}

void MainWindow::DisplayFreeSpace(uint32_t freeSpace)
{
    ui->tb_result->append("Свободное место на сервере: " + QString::number(freeSpace) + " байт");
}

void MainWindow::on_cb_request_currentIndexChanged(int index)
{
    // Разблокируем поле ввода данных только для "Отправить данные" (индекс 3)
    ui->le_data->setEnabled(index == 3);
}

void MainWindow::SetDataReply(QString replyString)
{
    ui->tb_result->append("Ответ сервера: " + replyString);
}

void MainWindow::DisplayConnectStatus(uint16_t status)
{
    if(status == ERR_CONNECT_TO_HOST){
        ui->tb_result->append("Ошибка подключения к порту: " + QString::number(ui->spB_port->value()));
        ui->lb_connectStatus->setText("Ошибка подключения");
        ui->lb_connectStatus->setStyleSheet("color: red");
    }
    else{
        ui->lb_connectStatus->setText("Подключено");
        ui->lb_connectStatus->setStyleSheet("color: green");
        ui->pb_connect->setText("Отключиться");
        ui->spB_port->setEnabled(false);
        ui->pb_request->setEnabled(true);
        ui->spB_ip1->setEnabled(false);
        ui->spB_ip2->setEnabled(false);
        ui->spB_ip3->setEnabled(false);
        ui->spB_ip4->setEnabled(false);
    }
}

void MainWindow::DisplaySuccess(uint16_t typeMess)
{
    switch (typeMess) {
    case CLEAR_DATA:
        ui->tb_result->append("Успешно: память сервера очищена");
        break;
    case SET_DATA:
        ui->tb_result->append("Успешно: данные отправлены на сервер");
        break;
    default:
        ui->tb_result->append("Операция выполнена успешно (тип: " + QString::number(typeMess) + ")");
        break;
    }
}

void MainWindow::DisplayError(uint16_t error)
{
    switch (error) {
    case ERR_NO_FREE_SPACE:
        ui->tb_result->append("Ошибка: недостаточно свободного места на сервере");
        break;
    case ERR_NO_FUNCT:
        ui->tb_result->append("Ошибка: запрошенная функция не реализована на сервере");
        break;
    case ERR_CONNECT_TO_HOST:
        ui->tb_result->append("Ошибка подключения к серверу");
        break;
    default:
        ui->tb_result->append("Неизвестная ошибка: " + QString::number(error));
        break;
    }
}

void MainWindow::on_pb_connect_clicked()
{
    if(ui->pb_connect->text() == "Подключиться"){
        uint16_t port = ui->spB_port->value();

        QString ip = ui->spB_ip4->text() + "." +
                     ui->spB_ip3->text() + "." +
                     ui->spB_ip2->text() + "." +
                     ui->spB_ip1->text();

        client->ConnectToHost(QHostAddress(ip), port);
    }
    else{
        client->DisconnectFromHost();
    }
}

void MainWindow::on_pb_request_clicked()
{
    ServiceHeader header;
    header.id = ID;
    header.status = STATUS_SUCCES;
    header.len = 0;

    switch (ui->cb_request->currentIndex()){
    //Получить время
    case 0:
        header.idData = GET_TIME;
        break;
    //Получить свободное место
    case 1:
        header.idData = GET_SIZE;
        break;
    //Получить статистику
    case 2:
        header.idData = GET_STAT;
        break;
    //Отправить данные
    case 3:
        header.idData = SET_DATA;
        header.len = ui->le_data->text().toUtf8().size();
        client->SendData(header, ui->le_data->text());
        return;
    //Очистить память на сервере
    case 4:
        header.idData = CLEAR_DATA;
        break;
    default:
        ui->tb_result->append("Такой запрос не реализован в текущей версии");
        return;
    }


    client->SendRequest(header);
}
