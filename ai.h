// ai.h
#ifndef AI_H
#define AI_H

#include "MainWindow.h"
#include "parametr.h"
#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QChartView>
#include <vector>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>

class ai : public QMainWindow
{
    Q_OBJECT

public:
    explicit ai(parametr* paramsWindow, QWidget *parent = nullptr);
    QTableWidget* getTable() const { return table; };
    void saveResultsToFile(const QString& filename);

    ~ai();

private slots:
    void trainAndPredict();

private:
    parametr* paramsWindow;
    QTableWidget* table;
    QLabel* infoLabel;
    QChartView* chartView;

    // Параметры нейросети
    std::vector<std::vector<float>> weights1;
    std::vector<float> biases1;
    std::vector<float> weights2;
    float bias2;
    float learningRate;

    // Методы нейросети
    float relu(float x);
    float predictNN(float x);
    void trainNN(const std::vector<float>& xData, const std::vector<float>& yData);

    void setupUI();
    void setupTable();
    void plotResults(const std::vector<float>& xData,
                     const std::vector<float>& yData,
                     const std::vector<float>& predictions) {
        QChart *chart = new QChart();
        chart->setTitle("Нейросетевая регрессия");
        chart->setAnimationOptions(QChart::AllAnimations);

        // Исходные данные
        QScatterSeries *scatter = new QScatterSeries();
        scatter->setName("Исходные данные");
        scatter->setMarkerSize(10.0);
        scatter->setColor(QColor(65, 105, 225));
        scatter->setBorderColor(Qt::white);

        // Предсказанные значения
        QScatterSeries *predicted = new QScatterSeries();
        predicted->setName("Предсказания");
        predicted->setMarkerSize(10.0);
        predicted->setColor(QColor(255, 105, 97));
        predicted->setBorderColor(Qt::white);

        // Линия тренда
        QLineSeries *line = new QLineSeries();
        line->setName("Аппроксимация");
        line->setColor(Qt::green);

        // Заполнение данных
        for (size_t i = 0; i < xData.size(); ++i) {
            scatter->append(xData[i], yData[i]);
            predicted->append(xData[i], predictions[i]);
            line->append(xData[i], predictions[i]);
        }

        chart->addSeries(scatter);
        chart->addSeries(predicted);
        chart->addSeries(line);

        // Настройка осей
        QValueAxis *axisX = new QValueAxis();
        axisX->setTitleText("X");
        axisX->setLabelFormat("%.2f");
        chart->addAxis(axisX, Qt::AlignBottom);
        scatter->attachAxis(axisX);
        predicted->attachAxis(axisX);
        line->attachAxis(axisX);

        QValueAxis *axisY = new QValueAxis();
        axisY->setTitleText("Y");
        axisY->setLabelFormat("%.2f");
        chart->addAxis(axisY, Qt::AlignLeft);
        scatter->attachAxis(axisY);
        predicted->attachAxis(axisY);
        line->attachAxis(axisY);

        chartView->setChart(chart);
    }
};

#endif // AI_H
