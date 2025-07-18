#include "MainWindow.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Настройка подключения к PostgreSQL
    if (!QSqlDatabase::isDriverAvailable("QPSQL")) {
        QMessageBox::critical(nullptr, "Ошибка", "Драйвер PostgreSQL не доступен!");
        return -1;
    }

    MainWindow w;
    w.show();

    return a.exec();
}
