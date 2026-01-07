#ifndef REGRESSIA_H
#define REGRESSIA_H

#include "MainWindow.h"
#include "parametr.h"
#include "E:\institut\work\NIR6\lin_regression.h"
#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QChartView>

class regressia : public QMainWindow
{
    Q_OBJECT

public:
    explicit regressia(parametr* paramsWindow, QWidget *parent = nullptr);
    QTableWidget* getTable() const { return table; };

    void saveResultsToFile(const QString& filename);

    ~regressia();
private slots:
    void calculateRegression();

private:
    parametr* paramsWindow;
    QTableWidget* table;  // Добавляем объявление таблицы
    QLabel* equationLabel;
    QChartView* chartView;

    void setupUI();
    void setupTable();
    void plotRegression(const std::vector<float>& xData,
                        const std::vector<float>& yData,
                        const Lin_regression& model);
};

#endif
