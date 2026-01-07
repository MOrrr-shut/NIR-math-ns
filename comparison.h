#ifndef COMPARISON_H
#define COMPARISON_H

#include <QMainWindow>
#include <QTableWidget>
#include <QChartView>
#include <QVBoxLayout>

class Comparison : public QMainWindow
{
    Q_OBJECT

public:
    Comparison(QWidget *parent = nullptr);

    QVector<QPair<double, double>> readCSV(const QString& filename);

private:
    void setupUI();
    void setupTable();
    void loadAndCompareData();
    void plotComparison();

    QTableWidget* comparisonTable;
    QChartView* chartView;
};

#endif // COMPARISON_H
