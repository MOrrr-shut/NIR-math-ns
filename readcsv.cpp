#include "readcsv.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDebug>

readCSV::readCSV(QObject *parent) : QObject(parent) {}

QList<QStringList> readCSV::read(const QString &filePath, QChar delimiter, bool hasHeader)
{
    QList<QStringList> data;

    if (!fileExists(filePath)) {
        emit errorOccurred("Файл не существует: " + filePath);
        return data;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit errorOccurred("Не удалось открыть файл: " + filePath);
        return data;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    // Читаем все строки
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList fields = line.split(delimiter);
        data.append(fields);
    }
    file.close();

    // Отладочный вывод
    qDebug() << "Прочитано строк:" << data.size();
    if (!data.isEmpty()) {
        qDebug() << "Первые 3 строки:";
        for (int i = 0; i < qMin(3, data.size()); ++i) {
            qDebug() << data[i];
        }
    }

    // Пропускаем заголовок если нужно
    if (hasHeader && !data.isEmpty()) {
        data.removeFirst();
    }

    return data;
}

bool readCSV::fileExists(const QString &path) const
{
    return QFileInfo::exists(path);
}
