#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>



class MainWindow: public QMainWindow
{

    Q_OBJECT

public:

    static void setupBack(QWidget *widget, QString NameSetWindowTitle);
    static void setupMenu(QMainWindow* parent, QWidget* centralWidget);
    static void setupButton(QWidget* widget, QVBoxLayout* centralWidget);
};
#endif // MAINWINDOW_H
