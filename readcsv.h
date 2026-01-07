#ifndef READCSV_H
#define READCSV_H

#include <QObject>
#include <QStringList>

class readCSV : public QObject
{
    Q_OBJECT
public:
    explicit readCSV(QObject *parent = nullptr);
    QList<QStringList> read(const QString &filePath, QChar delimiter = ',', bool hasHeader = true);

signals:
    void errorOccurred(const QString &message);

private:
    bool fileExists(const QString &path) const;
};

#endif // READCSV_H
