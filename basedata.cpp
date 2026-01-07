#include "basedata.h"
#include "MainWindow.h"
#include <QLabel>
#include <QTableWidget>
#include <QMessageBox>
#include <QHeaderView>
#include <QFileInfo>

basedata::basedata(QWidget *parent)
    : QMainWindow(parent), m_csvReader(new readCSV(this))  // Инициализируем m_csvReader
{
    // СТИЛЬ
    MainWindow::setupBack(this, "БАЗА ДАННЫХ");

    // ПОЛУЧАЕМ ЛАУТ И ВИДЖЕТ
    QWidget *centralWidget = this->centralWidget();
    QHBoxLayout *mainLayout = qobject_cast<QHBoxLayout*>(centralWidget->layout());
    if (!mainLayout) {
        mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->setContentsMargins(0, 0, 0, 0);
    }

    // ОКНО ДЛЯ ВИДЖЕТОВ
    QWidget *contentWidget = new QWidget();
    contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    contentWidget->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #CAD8E8, stop:1 #6889AE);"
        "border-top-left-radius: 15px;"
        "border-bottom-left-radius: 15px;"
        "border: none;"
        );

    // ЛАУТ ДЛЯ КОНТЕНТА
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(20, 9, 20, 20);
    contentLayout->setAlignment(Qt::AlignTop);

    // ОГЛАВЛЕНИЕ
    QLabel* paramsLabel = new QLabel("ИСХОДНАЯ БД", contentWidget);
    paramsLabel->setAlignment(Qt::AlignCenter);
    paramsLabel->setStyleSheet("color: white; background: transparent; font-size: 20px;");
    contentLayout->addWidget(paramsLabel);
    contentLayout->setSpacing(5);

    mainLayout->addWidget(contentWidget);

    // Подключаем обработчик ошибок
    connect(m_csvReader, &readCSV::errorOccurred, [this](const QString &error) {
        QMessageBox::critical(this, "Ошибка CSV", error);
    });

    loadData(contentLayout);
}

void basedata::loadData(QVBoxLayout *contentLayout)
{
    if (!contentLayout) {
        qCritical() << "Передан неверный layout";
        return;
    }

    // Проверяем существование файла
    QString filePath = "E:/pr_qtcreator/NIR2/Bitcoin_history_data.csv";
    if (!QFileInfo::exists(filePath)) {
        QLabel *errorLabel = new QLabel("Файл не найден: " + filePath);
        errorLabel->setStyleSheet("color: red; font-size: 16px;");
        contentLayout->addWidget(errorLabel);
        return;
    }

    // Загружаем данные
    QList<QStringList> csvData = m_csvReader->read(filePath, ',', true);

    // Отладочный вывод
    qDebug() << "Загружено строк данных:" << csvData.size();
    if (!csvData.isEmpty()) {
        qDebug() << "Первая строка данных:" << csvData.first();
    }

    if (csvData.isEmpty()) {
        QLabel *errorLabel = new QLabel("Файл пуст или не содержит данных");
        errorLabel->setStyleSheet("color: red; font-size: 16px;");
        contentLayout->addWidget(errorLabel);
        return;
    }

    // Создаем таблицу
    QTableWidget *table = new QTableWidget();
    table->setColumnCount(csvData.first().size());
    table->setRowCount(csvData.size());
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
        "QTableView::item:hover {"
        "   background-color: #F0F7FF;"
        "}"
        "QScrollBar:vertical {"
        "   background: #F5F9FF;"
        "   width: 10px;"
        "   border-radius: 4px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: #D6E7F5;"
        "   border-radius: 4px;"
        "   min-height: 30px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   background: none;"
        "}"
        );

    // Устанавливаем заголовки (если нужно)
    QStringList headers = {"Date", "Open", "High", "Low", "Close", "Volume"};
    table->setHorizontalHeaderLabels(headers);

    // Заполняем таблицу
    for (int row = 0; row < csvData.size(); ++row) {
        for (int col = 0; col < csvData[row].size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(csvData[row][col]);
            table->setItem(row, col, item);
        }
    }

    // Настройки таблицы
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    contentLayout->addWidget(table);
}
