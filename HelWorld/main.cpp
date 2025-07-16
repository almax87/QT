
#include <QDebug>
#include <QApplication>
#include <QLocale>
#include <QMessageBox>


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    QMessageBox msg;

    msg.setText("Hello GraphicWorld!!!");
    msg.setIcon(QMessageBox::Information);
    msg.setWindowTitle("First app");
    msg.show();

    return a.exec();
}
