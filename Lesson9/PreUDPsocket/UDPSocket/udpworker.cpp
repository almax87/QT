#include "udpworker.h"

UDPworker::UDPworker(QObject *parent) : QObject(parent)
{
}

void UDPworker::InitSocket()
{
    serviceUdpSocket = new QUdpSocket(this);
    serviceUdpSocket->bind(QHostAddress::LocalHost, BIND_PORT);
    connect(serviceUdpSocket, &QUdpSocket::readyRead, this, &UDPworker::readPendingDatagrams);
}

void UDPworker::ReadDatagram(QNetworkDatagram datagram)
{
    QByteArray data = datagram.data();

    // Проверяем, содержит ли датаграмма QDateTime (старый функционал)
    QDataStream inStr(&data, QIODevice::ReadOnly);
    QDateTime dateTime;
    inStr >> dateTime;

    if (dateTime.isValid()) {
        emit sig_sendTimeToGUI(dateTime);
    } else {
        // Если это не QDateTime, то считаем это текстовым сообщением
        QString text = QString::fromUtf8(data);
        emit sig_sendTextToGUI(text, datagram.senderAddress(), datagram.senderPort());
    }
}

void UDPworker::SendDatagram(QByteArray data)
{
    serviceUdpSocket->writeDatagram(data, QHostAddress::LocalHost, BIND_PORT);
}

void UDPworker::SendTextDatagram(QString text)
{
    QByteArray data = text.toUtf8();
    serviceUdpSocket->writeDatagram(data, QHostAddress::LocalHost, BIND_PORT);
}

void UDPworker::readPendingDatagrams(void)
{
    while(serviceUdpSocket->hasPendingDatagrams()){
        QNetworkDatagram datagram = serviceUdpSocket->receiveDatagram();
        ReadDatagram(datagram);
    }
}
