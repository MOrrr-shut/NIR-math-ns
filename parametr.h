#ifndef PARAMETR_H
#define PARAMETR_H

#include <QMainWindow>
#include <QTableWidget>
#include <QtCharts/QChartView>
#include "basedata.h"

class parametr : public QMainWindow
{
    Q_OBJECT
public:
    parametr(basedata* dataSource = nullptr, QWidget *parent = nullptr);

    int lastCol1 = 2;
    int lastCol2 = 5;

private slots:
    void onLoadClicked();

protected:
    void showEvent(QShowEvent *event) override;

public:
    void setupUI();
    void setupChart();
    void setupTable();
    void setupInputWidgets();
    void loadDataFromCSV();
    void fillTable(int colNum1, int colNum2);
    void fillTableAndChart(int colNum1, int colNum2);
    QString getColumnName(int colNum);

    QTableWidget *table;
    QLineEdit *input1;
    QLineEdit *input2;
    basedata *dataSource;
    QString csvFilePath = "E:/pr_qtcreator/NIR2/Bitcoin_history_data.csv";

    QChartView *chartView;
};

#endif // PARAMETR_H
