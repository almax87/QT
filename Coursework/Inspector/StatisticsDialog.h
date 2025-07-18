#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include "DatabaseManager.h"

namespace Ui {
class StatisticsDialog;
}

class StatisticsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatisticsDialog(DatabaseManager *dbManager, const QString &airportCode, QWidget *parent = nullptr);
    ~StatisticsDialog();

private slots:
    void onMonthChanged(int index);

private:
    void setupYearlyChart();
    void setupMonthlyChart(int month);

    Ui::StatisticsDialog *ui;
    DatabaseManager *m_dbManager;
    QString m_airportCode;
    QString m_airportName;
};

#endif // STATISTICSDIALOG_H
