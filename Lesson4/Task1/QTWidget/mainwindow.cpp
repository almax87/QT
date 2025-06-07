#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->radioButton->setText("Button1");
    ui->radioButton_2->setText("Button2");

    for (auto i = 0; i < 10; i++)
    {
        ui->comboBox->addItem(QString::fromStdString("Item" + std::to_string(i)));
    }

    ui->pushButton->setText("Нажми меня");
    ui->pushButton->setCheckable(true);

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(10);
    ui->progressBar->setValue(0);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    int currentValue = ui->progressBar->value();
    currentValue++;

    if (currentValue > ui->progressBar->maximum())
        currentValue = 0;

    ui->progressBar->setValue(currentValue);
}



