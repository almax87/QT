#include "tcpclient.h"

// В tcpclient.cpp, в начале файла (после #include)
QDataStream &operator>>(QDataStream &out, StatServer &data) {
    out >> data.incBytes;
    out >> data.sendBytes;
    out >> data.revPck;
    out >> data.sendPck;
    out >> data.workTime;
    out >> data.clients;
    return out;
}

QDataStream &operator<<(QDataStream &in, StatServer &data) {
    in << data.incBytes;
    in << data.sendBytes;
    in << data.revPck;
    in << data.sendPck;
    in << data.workTime;
    in << data.clients;
    return in;
}

QDataStream &operator >>(QDataStream &out, ServiceHeader &data){
    out >> data.id;
    out >> data.idData;
    out >> data.status;
    out >> data.len;
    return out;
};

QDataStream &operator <<(QDataStream &in, ServiceHeader &data){
    in << data.id;
    in << data.idData;
    in << data.status;
    in << data.len;
    return in;
};

TCPclient::TCPclient(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &TCPclient::ReadyReed);
    connect(socket, &QTcpSocket::disconnected, this, [&]{
        emit sig_Disconnected();
    });
}

void TCPclient::SendRequest(ServiceHeader head)
{
    if(socket->state() == QAbstractSocket::ConnectedState){
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out << head;
        socket->write(data);
    }
}

void TCPclient::ConnectToHost(QHostAddress host, uint16_t port)
{
    socket->connectToHost(host, port);
    if(socket->waitForConnected(1000)){
        emit sig_connectStatus(STATUS_SUCCES);
    } else {
        emit sig_connectStatus(ERR_CONNECT_TO_HOST);
    }
}

void TCPclient::DisconnectFromHost()
{
    socket->disconnectFromHost();
    if(socket->state() != QAbstractSocket::UnconnectedState){
        socket->waitForDisconnected(1000);
    }
}

void TCPclient::ReadyReed()
{
    QDataStream incStream(socket);

    if(incStream.status() != QDataStream::Ok){
        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText("Ошибка открытия входящего потока для чтения данных!");
        msg.exec();
        return;
    }

    // Читаем до конца потока
    while(incStream.atEnd() == false){
        // Если мы обработали предыдущий пакет, мы скинули значение idData в 0
        if(servHeader.idData == 0){
            // Проверяем количество полученных байт
            if(socket->bytesAvailable() < sizeof(ServiceHeader)){
                return;
            }
            else{
                // Читаем заголовок
                incStream >> servHeader;
                // Проверяем на корректность данных
                if(servHeader.id != ID){
                    uint16_t hdr = 0;
                    while(incStream.atEnd() == false){
                        incStream >> hdr;
                        if(hdr == ID){
                            incStream >> servHeader.idData;
                            incStream >> servHeader.status;
                            incStream >> servHeader.len;
                            break;
                        }
                    }
                }
            }
        }
        // Если получены не все данные, ждем новую посылку
        if(socket->bytesAvailable() < servHeader.len){
            return;
        }
        else{
            // Обработка данных
            ProcessingData(servHeader, incStream);
            servHeader.idData = 0;
            servHeader.status = 0;
            servHeader.len = 0;
        }
    }
}

void TCPclient::SendData(ServiceHeader header, QString str)
{
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);

        // Сначала записываем заголовок
        out << header;

        // Затем добавляем строку данных
        out << str;

        // Отправляем данные
        socket->write(data);
    }
}

void TCPclient::ProcessingData(ServiceHeader header, QDataStream &stream)
{
    switch (header.idData){
    case GET_TIME: {
        QDateTime time;
        stream >> time;
        emit sig_sendTime(time);
        break;
    }
    case GET_SIZE: {
        uint32_t freeSpace;
        stream >> freeSpace;
        emit sig_sendFreeSize(freeSpace);
        break;
    }
    case GET_STAT: {
        StatServer stat;
        stream >> stat;
        emit sig_sendStat(stat);
        break;
    }
    case SET_DATA: {
        QString reply;
        stream >> reply;
        emit sig_SendReplyForSetData(reply);
        break;
    }
    case CLEAR_DATA: {
        emit sig_Success(CLEAR_DATA);
        break;
    }
    default:
        emit sig_Error(ERR_NO_FUNCT);
        break;
    }
}
