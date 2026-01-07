#include "parametr.h"
#include "MainWindow.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileInfo>
#include <QDebug>
#include <QMenu>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>

parametr::parametr(basedata* dataSource, QWidget *parent)
    : QMainWindow(parent), dataSource(dataSource)
{
    setupUI();

    input1->setText(QString::number(lastCol1));
    input2->setText(QString::number(lastCol2));

    loadDataFromCSV();
    fillTableAndChart(lastCol1, lastCol2);
}

void parametr::setupUI()
{
    MainWindow::setupBack(this, "ПАРАМЕТРЫ");

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

    QLabel* paramsLabel = new QLabel("ПАРАМЕТРЫ", contentWidget);
    paramsLabel->setAlignment(Qt::AlignCenter);
    paramsLabel->setStyleSheet("color: white; background: transparent; font-size: 20px;");
    contentLayout->addWidget(paramsLabel);

    // Горизонтальный разделитель для таблицы и графика
    QSplitter *splitter = new QSplitter(Qt::Horizontal, contentWidget);

    // Настройка таблицы (левая часть)
    setupTable();
    splitter->addWidget(table);

    // Настройка графика (правая часть)
    setupChart();
    splitter->addWidget(chartView);

    // Установка начального соотношения размеров (1:2)
    QList<int> sizes;
    sizes << width() / 3 << width() * 2 / 3;
    splitter->setSizes(sizes);

    contentLayout->addWidget(splitter, 1); // Добавляем разделитель в layout

    // Поля ввода
    QLabel* inputLabel = new QLabel("Входные столбцы", contentWidget);
    inputLabel->setStyleSheet("color: white; background: transparent; font-size: 16px;");
    contentLayout->addWidget(inputLabel);

    setupInputWidgets();
    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(input1);
    inputLayout->addWidget(input2);
    contentLayout->addLayout(inputLayout);

    QPushButton *loadButton = new QPushButton("Load", contentWidget);
    loadButton->setFixedWidth(100);
    loadButton->setStyleSheet(
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
    connect(loadButton, &QPushButton::clicked, this, &parametr::onLoadClicked);
    contentLayout->addWidget(loadButton, 0, Qt::AlignLeft);

    mainLayout->addWidget(contentWidget);
}
void parametr::setupTable()
{
    table = new QTableWidget(0, 2, this);
    table->setStyleSheet(
        "QTableView {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #D6E7F5;"
        "   border-radius: 8px;"
        "   gridline-color: #E1EFF9;"
        "   font: 10pt 'Segoe UI';"
        "   color: #3A5A78;"
        "}"
        "QTableView::item {"
        "   padding: 8px;"
        "   border-bottom: 1px solid #E1EFF9;"
        "}"
        "QTableView::item:selected {"
        "   background-color: #A8D0F5;"
        "   color: #2C4A6E;"
        "}"
        "QHeaderView::section {"
        "   background-color: #E1EFF9;"
        "   color: #3A5A78;"
        "   padding: 10px;"
        "   border: none;"
        "   border-bottom: 2px solid #D6E7F5;"
        "   font-weight: 500;"
        "}"
        );
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void parametr::setupInputWidgets()
{
    input1 = new QLineEdit(this);
    input2 = new QLineEdit(this);

    QString lineEditStyle =
        "QLineEdit {"
        "   background: #4682B4;"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "   min-width: 100px;"
        "   border: 1px solid #a0a0a0;"
        "}";

    input1->setStyleSheet(lineEditStyle);
    input2->setStyleSheet(lineEditStyle);

    input1->setPlaceholderText("Номер столбца (1-6)");
    input2->setPlaceholderText("Номер столбца (1-6)");
}

void parametr::loadDataFromCSV()
{
    if (!QFileInfo::exists(csvFilePath)) {
        QMessageBox::critical(this, "Ошибка", "Файл данных не найден: " + csvFilePath);
        return;
    }

    if (!dataSource) {
        dataSource = new basedata(this);
    }

    QList<QStringList> csvData = dataSource->getCsvReader()->read(csvFilePath, ',', true);
    if (csvData.isEmpty()) {
        QMessageBox::information(this, "Информация", "Файл не содержит данных");
        return;
    }

    dataSource->setLoadedData(csvData);
}

void parametr::fillTable(int colNum1, int colNum2)
{
    if (!table || !dataSource) return;

    QList<QStringList> csvData = dataSource->getLoadedData();
    if (csvData.isEmpty()) return;

    table->clearContents();
    table->setRowCount(csvData.size());

    for (int row = 0; row < csvData.size(); ++row) {
        const QStringList &rowData = csvData[row];

        if (rowData.size() >= colNum1) {
            QTableWidgetItem *item1 = new QTableWidgetItem(rowData[colNum1-1]);
            table->setItem(row, 0, item1);
        }

        if (rowData.size() >= colNum2) {
            QTableWidgetItem *item2 = new QTableWidgetItem(rowData[colNum2-1]);
            table->setItem(row, 1, item2);
        }
    }

    QStringList tableHeaders;
    tableHeaders << getColumnName(colNum1) << getColumnName(colNum2);
    table->setHorizontalHeaderLabels(tableHeaders);
}

void parametr::onLoadClicked()
{
    // Проверка ввода
    bool ok1, ok2;
    lastCol1 = input1->text().toInt(&ok1);
    lastCol2 = input2->text().toInt(&ok2);

    if(!ok1 || !ok2 || lastCol1 < 1 || lastCol1 > 6 || lastCol2 < 1 || lastCol2 > 6) {
        QMessageBox::warning(this, "Ошибка",
                             "Введите номера столбцов от 1 до 6\n"
                             "1-Дата 2-Открытие 3-Макс\n"
                             "4-Мин 5-Закрытие 6-Объем");
        return;
    }

    loadDataFromCSV();
    fillTableAndChart(lastCol1, lastCol2);
}

QString parametr::getColumnName(int colNum)
{
    switch(colNum) {
    case 1: return "Дата";
    case 2: return "Открытие";
    case 3: return "Макс";
    case 4: return "Мин";
    case 5: return "Закрытие";
    case 6: return "Объем";
    default: return QString("Столбец %1").arg(colNum);
    }
}

void parametr::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

    // Восстанавливаем последние выбранные столбцы
    input1->setText(QString::number(lastCol1));
    input2->setText(QString::number(lastCol2));

    // Обновляем таблицу
    if (dataSource && !dataSource->getLoadedData().isEmpty()) {
        fillTable(lastCol1, lastCol2);
    }
}

void parametr::setupChart()
{
    chartView = new QChartView(this);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(400, 300);

    // Настройка интерактивности
    chartView->setRubberBand(QChartView::RectangleRubberBand);
    chartView->setInteractive(true);
    chartView->setDragMode(QGraphicsView::ScrollHandDrag);

    // Контекстное меню только для сброса масштаба
    chartView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(chartView, &QChartView::customContextMenuRequested, [this](const QPoint &pos) {
        QMenu menu;
        menu.addAction("Сбросить масштаб", [this]() {
            if (chartView->chart()) {
                chartView->chart()->zoomReset();
            }
        });
        menu.exec(chartView->mapToGlobal(pos));
    });
}

void parametr::fillTableAndChart(int colNum1, int colNum2)
{
    if (!table || !dataSource || !chartView) return;

    QList<QStringList> csvData = dataSource->getLoadedData();
    if (csvData.isEmpty()) return;

    // Очищаем таблицу
    table->clearContents();
    table->setRowCount(csvData.size());

    // Создаем точечную серию
    QScatterSeries *series = new QScatterSeries();
    series->setName(QString("%1 vs %2").arg(getColumnName(colNum1)).arg(getColumnName(colNum2)));
    series->setMarkerSize(10.0);
    series->setColor(QColor(65, 105, 225)); // Цвет точек
    series->setBorderColor(Qt::white);
    series->setMarkerShape(QScatterSeries::MarkerShapeCircle);

    // Заполняем данные
    for (int row = 0; row < csvData.size(); ++row) {
        const QStringList &rowData = csvData[row];

        // Таблица
        if (rowData.size() >= colNum1) {
            table->setItem(row, 0, new QTableWidgetItem(rowData[colNum1-1]));
        }
        if (rowData.size() >= colNum2) {
            table->setItem(row, 1, new QTableWidgetItem(rowData[colNum2-1]));
        }

        // График
        if (rowData.size() >= qMax(colNum1, colNum2)) {
            bool ok1, ok2;
            double x = rowData[colNum1-1].toDouble(&ok1);
            double y = rowData[colNum2-1].toDouble(&ok2);
            if (ok1 && ok2) series->append(x, y);
        }
    }

    // Настраиваем таблицу
    table->setHorizontalHeaderLabels({
        getColumnName(colNum1),
        getColumnName(colNum2)
    });

    // Настраиваем график
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->legend()->setVisible(true);
    chart->setTitle(QString("Зависимость: %1 от %2").arg(getColumnName(colNum2)).arg(getColumnName(colNum1)));
    chart->setAnimationOptions(QChart::AllAnimations);

    // Оси
    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText(getColumnName(colNum1));
    axisX->setLabelFormat("%.2f");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText(getColumnName(colNum2));
    axisY->setLabelFormat("%.2f");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chartView->setChart(chart);
}
