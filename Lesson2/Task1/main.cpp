#include <QCoreApplication>
#include <QtSql>
#include <QtNetwork>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qInfo() << QHostAddress(QHostAddress::LocalHost) << Qt::endl;

    QSqlDatabase db;
    qInfo() << "База открыта:" << db.isOpen() << Qt::endl;

    return a.exec();
}
