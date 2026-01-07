#include "csveditor.h"
#include <QDebug>

CSVEditor::CSVEditor(QObject *parent) : QObject(parent) {}

bool CSVEditor::loadCSV(const QString &filePath, QChar delimiter)
{
    m_data.clear();
    m_headers.clear();
    m_selectedColumns.clear();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit errorOccurred("Не удалось открыть файл: " + filePath);
        return false;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    // Читаем заголовки (первая строка)
    if (!in.atEnd()) {
        QString headerLine = in.readLine();
        m_headers = headerLine.split(delimiter);
    }

    // Читаем данные
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList fields = line.split(delimiter);
        m_data.append(fields);
    }

    file.close();
    return true;
}

bool CSVEditor::selectColumns(const QList<int> &columns)
{
    if (m_data.isEmpty()) {
        emit errorOccurred("Нет данных для обработки");
        return false;
    }

    // Проверяем индексы столбцов
    for (int col : columns) {
        if (col < 0 || col >= m_headers.size()) {
            emit errorOccurred("Неверный индекс столбца: " + QString::number(col));
            return false;
        }
    }

    m_selectedColumns = columns;
    return true;
}

bool CSVEditor::saveCSV(const QString &filePath, QChar delimiter)
{
    if (m_selectedColumns.isEmpty()) {
        emit errorOccurred("Не выбраны столбцы для сохранения");
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit errorOccurred("Не удалось создать файл: " + filePath);
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    // Записываем заголовки выбранных столбцов
    QStringList selectedHeaders;
    for (int col : m_selectedColumns) {
        selectedHeaders << m_headers[col];
    }
    out << selectedHeaders.join(delimiter) << "\n";

    // Записываем данные
    for (const QStringList &row : m_data) {
        QStringList selectedFields;
        for (int col : m_selectedColumns) {
            if (col < row.size()) {
                selectedFields << row[col];
            } else {
                selectedFields << "";
            }
        }
        out << selectedFields.join(delimiter) << "\n";
    }

    file.close();
    return true;
}

const QList<QStringList>& CSVEditor::getData() const
{
    return m_data;
}

const QStringList& CSVEditor::getHeaders() const
{
    return m_headers;
}

int CSVEditor::columnCount() const
{
    return m_headers.size();
}

int CSVEditor::rowCount() const
{
    return m_data.size();
}
