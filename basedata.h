#ifndef BASEDATA_H
#define BASEDATA_H
#include "readcsv.h"

#include <QMainWindow>
#include <QVBoxLayout>
#include <QList>

class basedata : public QMainWindow
{
public:

    basedata(QWidget *parent = nullptr);
    readCSV* getCsvReader() const { return m_csvReader; };
    QList<QStringList> getLoadedData() const { return loadedData; };
    void setLoadedData(const QList<QStringList>& data) { loadedData = data; }

private:

    readCSV *m_csvReader;
    QList<QStringList> loadedData;
    void loadData(QVBoxLayout *contentLayout);

};

#endif // BASEDATA_H
