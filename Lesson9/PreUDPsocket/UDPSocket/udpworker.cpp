#include "udpworker.h"

UDPworker::UDPworker(QObject *parent) : QObject(parent),
    timeSocket(nullptr),
    textSocket(nullptr)
{
}

void UDPworker::InitTimeSocket()
{
    timeSocket = new QUdpSocket(this);
    timeSocket->bind(QHostAddress::LocalHost, TIME_PORT);
    connect(timeSocket, &QUdpSocket::readyRead, this, &UDPworker::readTimeDatagrams);
}

void UDPworker::InitTextSocket()
{
    textSocket = new QUdpSocket(this);
    textSocket->bind(QHostAddress::LocalHost, TEXT_PORT);
    connect(textSocket, &QUdpSocket::readyRead, this, &UDPworker::readTextDatagrams);
}

void UDPworker::SendTimeDatagram(QByteArray data)
{
    if(timeSocket)
        timeSocket->writeDatagram(data, QHostAddress::LocalHost, TIME_PORT);
}

void UDPworker::SendTextDatagram(QString text)
{
    if(textSocket) {
        QByteArray data = text.toUtf8();
        textSocket->writeDatagram(data, QHostAddress::LocalHost, TEXT_PORT);
    }
}

void UDPworker::readTimeDatagrams()
{
    while(timeSocket->hasPendingDatagrams()){
        QNetworkDatagram datagram = timeSocket->receiveDatagram();
        processTimeDatagram(datagram);
    }
}

void UDPworker::readTextDatagrams()
{
    while(textSocket->hasPendingDatagrams()){
        QNetworkDatagram datagram = textSocket->receiveDatagram();
        processTextDatagram(datagram);
    }
}

void UDPworker::processTimeDatagram(QNetworkDatagram datagram)
{
    QByteArray data = datagram.data();
    QDataStream inStr(&data, QIODevice::ReadOnly);
    QDateTime dateTime;
    inStr >> dateTime;

    if(dateTime.isValid()) {
        emit sig_sendTimeToGUI(dateTime);
    }
}

void UDPworker::processTextDatagram(QNetworkDatagram datagram)
{
    QString text = QString::fromUtf8(datagram.data());
    emit sig_sendTextToGUI(text, datagram.senderAddress(), datagram.senderPort());
}
