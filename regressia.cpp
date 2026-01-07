#include "regressia.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QFile>
#include <QDir>

regressia::regressia(parametr* paramsWindow, QWidget *parent)
    : QMainWindow(parent), paramsWindow(paramsWindow), table(nullptr)  // Инициализируем table
{
    setupUI();
}

void regressia::setupUI()
{
    // Оригинальный стиль окна
    MainWindow::setupBack(this, "РЕГРЕССИЯ");

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

    QLabel* paramsLabel = new QLabel("ЛИН РЕГРЕССИЯ", contentWidget);
    paramsLabel->setAlignment(Qt::AlignCenter);
    paramsLabel->setStyleSheet("color: white; background: transparent; font-size: 20px;");
    contentLayout->addWidget(paramsLabel);

    // Таблица с более точными значениями
    setupTable();
    contentLayout->addWidget(table);

    // Кнопка расчета
    QPushButton *calcBtn = new QPushButton("Рассчитать регрессию");
    calcBtn->setStyleSheet(
        "QPushButton {"
        "   background: #5B7FAF;"
        "   color: white;"
        "   border-radius: 10px;"
        "   padding: 8px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background: #6B8FBF;"
        "}"
        );
    connect(calcBtn, &QPushButton::clicked, this, &regressia::calculateRegression);
    contentLayout->addWidget(calcBtn, 0, Qt::AlignLeft);

    // Поле для уравнения
    equationLabel = new QLabel();
    equationLabel->setStyleSheet("color: white; font-size: 14px;");
    contentLayout->addWidget(equationLabel);

    // График
    chartView = new QChartView();
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(400, 300);
    contentLayout->addWidget(chartView);

    mainLayout->addWidget(contentWidget);
}

void regressia::setupTable()
{
    table = new QTableWidget(0, 2);
    table->setHorizontalHeaderLabels({"Фактические Y", "Предсказанные Y"});
    table->setStyleSheet(
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
        "}"
        );
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void regressia::calculateRegression()
{
    if (!paramsWindow || !paramsWindow->table) {
        QMessageBox::warning(this, "Ошибка", "Не найдено окно с параметрами");
        return;
    }

    QTableWidget* sourceTable = paramsWindow->table;
    std::vector<float> xData, yData;

    for (int row = 0; row < sourceTable->rowCount(); ++row) {
        QTableWidgetItem* xItem = sourceTable->item(row, 0);
        QTableWidgetItem* yItem = sourceTable->item(row, 1);

        if (xItem && yItem) {
            bool okX, okY;
            float x = xItem->text().toFloat(&okX);
            float y = yItem->text().toFloat(&okY);

            if (okX && okY) {
                xData.push_back(x);
                yData.push_back(y);
            }
        }
    }

    if (xData.size() < 2) {
        QMessageBox::warning(this, "Ошибка", "Недостаточно данных для расчета");
        return;
    }

    Lin_regression model(yData, xData);

    table->setRowCount(xData.size());
    for (size_t i = 0; i < xData.size(); ++i) {
        float predicted = model.predict(xData[i]);
        table->setItem(i, 0, new QTableWidgetItem(QString::number(yData[i])));
        table->setItem(i, 1, new QTableWidgetItem(QString::number(predicted)));
    }

    QString equation = QString("Уравнение: y = %1x + %2")
                           .arg(model.formula_K(), 0, 'f', 8)
                           .arg(model.formula_B(), 0, 'f', 8);
    equationLabel->setText(equation);

    plotRegression(xData, yData, model);

    saveResultsToFile("regression_results.csv");
}

void regressia::plotRegression(const std::vector<float>& xData,
                               const std::vector<float>& yData,
                               const Lin_regression& model)
{
    QChart *chart = new QChart();
    chart->setTitle("Линейная регрессия");
    chart->setAnimationOptions(QChart::AllAnimations);

    QScatterSeries *scatter = new QScatterSeries();
    scatter->setName("Исходные данные");
    scatter->setMarkerSize(10.0);
    scatter->setColor(QColor(65, 105, 225));
    scatter->setBorderColor(Qt::white);

    QLineSeries *line = new QLineSeries();
    line->setName("Линия регрессии");
    line->setColor(Qt::red);

    float minX = xData.empty() ? 0 : xData[0];
    float maxX = xData.empty() ? 0 : xData[0];

    for (size_t i = 0; i < xData.size(); ++i) {
        float x = xData[i];
        float y = yData[i];

        if (x < minX) minX = x;
        if (x > maxX) maxX = x;

        scatter->append(x, y);
    }

    line->append(minX, model.predict(minX));
    line->append(maxX, model.predict(maxX));

    chart->addSeries(scatter);
    chart->addSeries(line);

    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("X");
    axisX->setLabelFormat("%.2f");
    chart->addAxis(axisX, Qt::AlignBottom);
    scatter->attachAxis(axisX);
    line->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Y");
    axisY->setLabelFormat("%.2f");
    chart->addAxis(axisY, Qt::AlignLeft);
    scatter->attachAxis(axisY);
    line->attachAxis(axisY);

    chartView->setChart(chart);
}

void regressia::saveResultsToFile(const QString& filename) {
    const QString SAVE_PATH = "C:/NeuroRegressionData/";
    const QString REGRESSION_FILE = SAVE_PATH + "regression_results.csv";

    if (!QDir().mkpath(SAVE_PATH)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать папку для результатов");
        return;
    }

    QFile file(REGRESSION_FILE);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка",
                              QString("Ошибка при сохранении:\n%1").arg(file.errorString()));
        return;
    }

    QTextStream out(&file);
    out << "X,ActualY,PredictedY\n";
    for (int row = 0; row < table->rowCount(); ++row) {
        out << paramsWindow->table->item(row, 0)->text() << ","
            << table->item(row, 0)->text() << ","
            << table->item(row, 1)->text() << "\n";
    }
    file.close();

    // Показываем где сохранено
    QMessageBox::information(this, "Успех",
                             QString("Результаты регрессии сохранены в:\n%1").arg(REGRESSION_FILE));
}

regressia::~regressia() {
    qDebug() << "regressia window destroyed";
}
