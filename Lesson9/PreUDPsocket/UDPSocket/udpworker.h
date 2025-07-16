#ifndef UDPWORKER_H
#define UDPWORKER_H

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QDateTime>

#define TIME_PORT 12345
#define TEXT_PORT 12346

class UDPworker : public QObject
{
    Q_OBJECT
public:
    explicit UDPworker(QObject *parent = nullptr);
    void InitTimeSocket();
    void InitTextSocket();
    void SendTimeDatagram(QByteArray data);
    void SendTextDatagram(QString text);

private slots:
    void readTimeDatagrams();
    void readTextDatagrams();

private:
    QUdpSocket* timeSocket;
    QUdpSocket* textSocket;
    void processTimeDatagram(QNetworkDatagram datagram);
    void processTextDatagram(QNetworkDatagram datagram);

signals:
    void sig_sendTimeToGUI(QDateTime data);
    void sig_sendTextToGUI(QString text, QHostAddress senderAddress, quint16 senderPort);
};

#endif // UDPWORKER_H
