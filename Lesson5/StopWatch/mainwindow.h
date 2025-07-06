#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "stopwatch.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pb_start_stop_clicked();
    void on_pb_lap_clicked();
    void on_pb_reset_clicked();
    void updateTimeDisplay();
    void addLap(const QString &lapInfo);

private:
    Ui::MainWindow *ui;
    Stopwatch* stopwatch;
};
#endif // MAINWINDOW_H
