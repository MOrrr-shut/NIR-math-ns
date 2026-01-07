#ifndef CSVEDITOR_H
#define CSVEDITOR_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>

class CSVEditor : public QObject
{
    Q_OBJECT
public:
    CSVEditor(QObject *parent = nullptr);

    bool loadCSV(const QString &filePath, QChar delimiter = ',');
    bool saveCSV(const QString &filePath, QChar delimiter = ',');
    bool selectColumns(const QList<int> &columns);

    const QList<QStringList>& getData() const;
    const QStringList& getHeaders() const;

    int columnCount() const;
    int rowCount() const;

signals:
    void errorOccurred(const QString &message);

private:
    QList<QStringList> m_data;
    QStringList m_headers;
    QList<int> m_selectedColumns;
};

#endif // CSVEDITOR_H
