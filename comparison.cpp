#include "comparison.h"
#include "mainwindow.h"
#include "regressia.h"
#include "ai.h"
#include <QFile>
#include <QLabel>
#include <QTextStream>
#include <QMessageBox>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
#include <QHeaderView>

Comparison::Comparison(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    loadAndCompareData();
}

void Comparison::setupUI()
{
    // Оригинальный стиль окна
    MainWindow::setupBack(this, "СРАВНЕНИЕ МОДЕЛЕЙ");

    QWidget *centralWidget = this->centralWidget();
    QHBoxLayout *mainLayout = qobject_cast<QHBoxLayout*>(centralWidget->layout());
    if (!mainLayout) {
        mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->setContentsMargins(0, 0, 0, 0);
    }

    QWidget *contentWidget = new QWidget();
    contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    contentWidget->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #CAD8E8, stop:1 #6889AE);"
        "border-top-left-radius: 15px;"
        "border-bottom-left-radius: 15px;"
        "border: none;"
        );

    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(20, 9, 20, 20);
    contentLayout->setSpacing(15);

    QLabel* paramsLabel = new QLabel("СРАВНЕНИЕ МОДЕЛЕЙ", contentWidget);
    paramsLabel->setAlignment(Qt::AlignCenter);
    paramsLabel->setStyleSheet("color: white; background: transparent; font-size: 20px;");
    contentLayout->addWidget(paramsLabel);

    setupTable();
    contentLayout->addWidget(comparisonTable);

    chartView = new QChartView();
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setRubberBand(QChartView::RectangleRubberBand);
    chartView->setMinimumSize(400, 300);
    contentLayout->addWidget(chartView);

    mainLayout->addWidget(contentWidget);
}

void Comparison::setupTable()
{
    comparisonTable = new QTableWidget(0, 4);
    comparisonTable->setHorizontalHeaderLabels({
        "X",
        "Фактические Y",
        "Линейная регрессия",
        "Нейросеть"
    });
    comparisonTable->setStyleSheet(
        "QTableView {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #D6E7F5;"
        "   border-radius: 8px;"
        "   gridline-color: #E1EFF9;"
        "   font: 10pt 'Segoe UI';"
        "   color: #3A5A78;"
        "}"
        "QHeaderView::section {"
        "   background-color: #E1EFF9;"
        "   color: #3A5A78;"
        "   padding: 10px;"
        "}");
    comparisonTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void Comparison::loadAndCompareData() {
    // Проверка существования файлов в текущей директории
    if (!QFile::exists("ai_results.csv") || !QFile::exists("regression_results.csv")) {
        QMessageBox::critical(this, "Ошибка",
                              "Файлы с результатами не найдены!\n"
                              "Сначала выполните расчёты в обоих модулях.");
        return;
    }

    // Чтение данных с обработкой ошибок
    auto aiData = readCSV("ai_results.csv");
    auto regressData = readCSV("regression_results.csv");

    // Проверка, что данные не пустые
    if (aiData.isEmpty() || regressData.isEmpty()) {
        QMessageBox::warning(this, "Ошибка",
                             "Не удалось прочитать данные для сравнения.\n"
                             "Проверьте целостность файлов результатов.");
        return;
    }

    // Очистка таблицы перед заполнением
    comparisonTable->setRowCount(0);

    // Заполнение таблицы с проверкой индексов
    int rowsToCompare = qMin(aiData.size(), regressData.size());
    for (int i = 0; i < rowsToCompare; ++i) {
        int newRow = comparisonTable->rowCount();
        comparisonTable->insertRow(newRow);

        // Добавляем данные с проверкой
        auto addSafe = [this](int row, int col, double value) {
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(value));
            comparisonTable->setItem(row, col, item);
        };

        addSafe(newRow, 0, regressData[i].first);   // X
        addSafe(newRow, 1, regressData[i].second);  // ActualY из регрессии
        addSafe(newRow, 2, aiData[i].second);       // PredictedY из ИИ
    }

    plotComparison();
}

void Comparison::plotComparison()
{
    if (comparisonTable->rowCount() == 0) {
        qDebug() << "Нет данных для построения графика";
        return;
    }

    try {
        QChart *chart = new QChart();
        chart->setTitle("Сравнение моделей");
        chart->setAnimationOptions(QChart::AllAnimations);
        chart->setBackgroundBrush(QBrush(QColor("#F0F8FF")));

        // Исходные данные
        QScatterSeries *actualSeries = new QScatterSeries();
        actualSeries->setName("Фактические значения");
        actualSeries->setMarkerSize(10.0);
        actualSeries->setColor(QColor(65, 105, 225)); // RoyalBlue
        actualSeries->setBorderColor(Qt::white);

        // Линейная регрессия
        QLineSeries *regressSeries = new QLineSeries();
        regressSeries->setName("Линейная регрессия");
        regressSeries->setColor(Qt::red);
        regressSeries->setPen(QPen(Qt::red, 2));

        // Нейросеть
        QLineSeries *neuralSeries = new QLineSeries();
        neuralSeries->setName("Нейросеть");
        neuralSeries->setColor(Qt::green);
        neuralSeries->setPen(QPen(Qt::green, 2));

        // Заполнение данными
        for (int row = 0; row < comparisonTable->rowCount(); ++row) {
            double x = comparisonTable->item(row, 0)->text().toDouble();
            double actual = comparisonTable->item(row, 1)->text().toDouble();
            double regress = comparisonTable->item(row, 2)->text().toDouble();
            double neural = comparisonTable->item(row, 3)->text().toDouble();

            actualSeries->append(x, actual);
            regressSeries->append(x, regress);
            neuralSeries->append(x, neural);
        }

        chart->addSeries(actualSeries);
        chart->addSeries(regressSeries);
        chart->addSeries(neuralSeries);

        // Настройка осей
        QValueAxis *axisX = new QValueAxis();
        axisX->setTitleText("X");
        axisX->setTitleBrush(QBrush(Qt::white));
        axisX->setLabelsBrush(QBrush(Qt::white));
        chart->addAxis(axisX, Qt::AlignBottom);
        actualSeries->attachAxis(axisX);
        regressSeries->attachAxis(axisX);
        neuralSeries->attachAxis(axisX);

        QValueAxis *axisY = new QValueAxis();
        axisY->setTitleText("Y");
        axisY->setTitleBrush(QBrush(Qt::white));
        axisY->setLabelsBrush(QBrush(Qt::white));
        chart->addAxis(axisY, Qt::AlignLeft);
        actualSeries->attachAxis(axisY);
        regressSeries->attachAxis(axisY);
        neuralSeries->attachAxis(axisY);

        chart->setBackgroundVisible(false);
        chartView->setChart(chart);
    }
    catch (const std::exception& e) {
        qCritical() << "Ошибка при построении графика:" << e.what();
        QMessageBox::critical(this, "Ошибка", "Не удалось построить график сравнения");
    }
}

QVector<QPair<double, double>> Comparison::readCSV(const QString& filename)
{
    QVector<QPair<double, double>> data;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл:" << filename;
        QMessageBox::warning(this, "Ошибка",
                             QString("Ошибка при открытии файла:\n%1\n%2")
                                 .arg(filename)
                                 .arg(file.errorString()));
        return data;
    }

    QTextStream in(&file);
    QString header = in.readLine(); // Пропускаем заголовок

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList values = line.split(",");
        if (values.size() < 3) {
            qDebug() << "Некорректная строка в файле:" << line;
            continue;
        }

        bool ok1, ok2;
        double x = values[0].toDouble(&ok1);
        double y = values[2].toDouble(&ok2); // Берем PredictedY (3-й столбец)

        if (!ok1 || !ok2) {
            qDebug() << "Ошибка преобразования данных в строке:" << line;
            continue;
        }

        data.append(qMakePair(x, y));
    }

    file.close();
    return data;
}
