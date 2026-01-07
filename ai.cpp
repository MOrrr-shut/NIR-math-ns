// ai.cpp
#include "ai.h"
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
#include <random>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <QFile>
#include <QDir>

const QString RESULTS_PATH = "E:/pr_qtcreator/NIR2/data";

ai::ai(parametr* paramsWindow, QWidget *parent)
    : QMainWindow(parent), paramsWindow(paramsWindow), table(nullptr),
    infoLabel(nullptr), chartView(nullptr), bias2(0), learningRate(0.01f)
{
    setupUI();
}

void ai::setupUI()
{
    MainWindow::setupBack(this, "НЕЙРОСЕТЕВАЯ РЕГРЕССИЯ");

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

    QLabel* paramsLabel = new QLabel("НЕЙРОСЕТЕВАЯ РЕГРЕССИЯ", contentWidget);
    paramsLabel->setAlignment(Qt::AlignCenter);
    paramsLabel->setStyleSheet("color: white; background: transparent; font-size: 20px;");
    contentLayout->addWidget(paramsLabel);

    setupTable();
    contentLayout->addWidget(table);

    QPushButton *calcBtn = new QPushButton("Обучить нейросеть");
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
    connect(calcBtn, &QPushButton::clicked, this, &ai::trainAndPredict);
    contentLayout->addWidget(calcBtn, 0, Qt::AlignLeft);

    infoLabel = new QLabel();
    infoLabel->setStyleSheet("color: white; font-size: 14px;");
    contentLayout->addWidget(infoLabel);

    chartView = new QChartView();
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(400, 300);
    contentLayout->addWidget(chartView);

    mainLayout->addWidget(contentWidget);
}

void ai::setupTable()
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

float ai::relu(float x)
{
    return std::max(0.0f, x);
}

float ai::predictNN(float x)
{
    const int hiddenSize = weights1.size();
    std::vector<float> hidden(hiddenSize);

    for (int i = 0; i < hiddenSize; ++i) {
        hidden[i] = relu(x * weights1[i][0] + biases1[i]);
    }

    float output = bias2;
    for (int i = 0; i < hiddenSize; ++i) {
        output += hidden[i] * weights2[i];
    }

    return output;
}

void ai::trainNN(const std::vector<float>& xData, const std::vector<float>& yData)
{
    const int epochs = 5000;
    const int hiddenSize = 10;

    // Инициализация весов
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> dist(0.0f, 0.1f);

    weights1.resize(hiddenSize, std::vector<float>(1));
    biases1.resize(hiddenSize);
    weights2.resize(hiddenSize);

    for (int i = 0; i < hiddenSize; ++i)
    {
        weights1[i][0] = dist(gen);
        biases1[i] = dist(gen);
        weights2[i] = dist(gen);
    }
    bias2 = dist(gen);

    learningRate = 0.01f;

    // Обучение
    for (int epoch = 0; epoch < epochs; ++epoch)
    {
        float totalError = 0.0f;

        for (size_t i = 0; i < xData.size(); ++i)
        {
            // Прямое распространение
            float x = xData[i];
            float y_true = yData[i];

            std::vector<float> hidden(hiddenSize);
            std::vector<float> hiddenAct(hiddenSize);

            for (int j = 0; j < hiddenSize; ++j)
            {
                hidden[j] = x * weights1[j][0] + biases1[j];
                hiddenAct[j] = relu(hidden[j]);
            }

            float y_pred = bias2;
            for (int j = 0; j < hiddenSize; ++j)
            {
                y_pred += hiddenAct[j] * weights2[j];
            }

            // Ошибка
            float error = y_pred - y_true;
            totalError += error * error;

            // Обратное распространение
            float delta_out = error;

            // Градиенты выходного слоя
            std::vector<float> gradW2(hiddenSize);
            for (int j = 0; j < hiddenSize; ++j)
            {
                gradW2[j] = delta_out * hiddenAct[j];
                weights2[j] -= learningRate * gradW2[j];
            }
            bias2 -= learningRate * delta_out;

            // Градиенты скрытого слоя
            for (int j = 0; j < hiddenSize; ++j)
            {
                float delta_hidden = delta_out * weights2[j] * (hidden[j] > 0 ? 1.0f : 0.0f);
                weights1[j][0] -= learningRate * delta_hidden * x;
                biases1[j] -= learningRate * delta_hidden;
            }
        }

        // Уменьшение learning rate
        if (epoch % 1000 == 0)
        {
            learningRate *= 0.5f;
        }
    }
}

void ai::trainAndPredict() {
    if (!paramsWindow || !paramsWindow->table)
    {
        QMessageBox::warning(this, "Ошибка", "Не найдено окно с параметрами");
        return;
    }

    QTableWidget* sourceTable = paramsWindow->table;
    std::vector<float> xData, yData;

    // Сбор данных
    for (int row = 0; row < sourceTable->rowCount(); ++row)
    {
        QTableWidgetItem* xItem = sourceTable->item(row, 0);
        QTableWidgetItem* yItem = sourceTable->item(row, 1);

        if (xItem && yItem && !xItem->text().isEmpty() && !yItem->text().isEmpty())
        {
            bool okX, okY;
            float x = xItem->text().toFloat(&okX);
            float y = yItem->text().toFloat(&okY);

            if (okX && okY) {
                xData.push_back(x);
                yData.push_back(y);
            }
        }
    }

    if (xData.size() < 2)
    {
        QMessageBox::warning(this, "Ошибка", "Недостаточно данных для обучения");
        return;
    }

    // Нормализация
    float x_mean = std::accumulate(xData.begin(), xData.end(), 0.0f) / xData.size();
    float y_mean = std::accumulate(yData.begin(), yData.end(), 0.0f) / yData.size();
    float x_std = std::sqrt(std::accumulate(xData.begin(), xData.end(), 0.0f,
                                            [x_mean](float sum, float x) { return sum + (x - x_mean)*(x - x_mean); }) / xData.size());
    float y_std = std::sqrt(std::accumulate(yData.begin(), yData.end(), 0.0f,
                                            [y_mean](float sum, float y) { return sum + (y - y_mean)*(y - y_mean); }) / yData.size());

    std::vector<float> xNorm(xData), yNorm(yData);
    for (size_t i = 0; i < xData.size(); ++i)
    {
        xNorm[i] = (xData[i] - x_mean) / x_std;
        yNorm[i] = (yData[i] - y_mean) / y_std;
    }

    // Обучение
    trainNN(xNorm, yNorm);

    // Предсказание
    table->setRowCount(xData.size());
    std::vector<float> predictions(xData.size());
    float mse = 0.0f;

    for (size_t i = 0; i < xData.size(); ++i)
    {
        float predNorm = predictNN(xNorm[i]);
        float predicted = predNorm * y_std + y_mean;
        predictions[i] = predicted;
        mse += (yData[i] - predicted) * (yData[i] - predicted);

        table->setItem(i, 0, new QTableWidgetItem(QString::number(yData[i])));
        table->setItem(i, 1, new QTableWidgetItem(QString::number(predicted)));
    }

    mse /= xData.size();
    float r2 = 1.0f - mse / (y_std * y_std);

    infoLabel->setText(QString("Нейросеть обучена\nMSE: %1\nR²: %2")
                           .arg(mse, 0, 'f', 4)
                           .arg(r2, 0, 'f', 4));

    plotResults(xData, yData, predictions);

    saveResultsToFile("ai_results.csv");
}

ai::~ai()
{
    qDebug() << "ai window destroyed";
}

void ai::saveResultsToFile(const QString& filename)
{
    // Жёстко прописанный путь
    const QString SAVE_PATH = "C:/NeuroRegressionData/";
    const QString AI_FILE = SAVE_PATH + "ai_results.csv";

    if (!QDir().exists(SAVE_PATH))
    {
        if (!QDir().mkpath(SAVE_PATH))
        {
            QMessageBox::critical(this, "Ошибка",
                                  QString("Не удалось создать папку:\n%1").arg(SAVE_PATH));
            return;
        }
    }

    // Открываем файл с проверкой
    QFile file(AI_FILE);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Ошибка",
                              QString("Не удалось открыть файл:\n%1\nОшибка: %2")
                                  .arg(AI_FILE)
                                  .arg(file.errorString()));
        return;
    }

    // Запись данных
    QTextStream out(&file);
    out << "X,ActualY,PredictedY\n";
    for (int row = 0; row < table->rowCount(); ++row)
    {
        out << table->item(row, 0)->text() << ","
            << table->item(row, 1)->text() << "\n";
    }
    file.close();

    // Подтверждение
    QMessageBox::information(this, "Сохранено",
                             QString("Данные сохранены в:\n%1").arg(AI_FILE));
}
