#include "mainwindow.h"
#include "basedata.h"
#include "parametr.h"
#include "regressia.h"
#include "ai.h"
#include "comparison.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QVector>
#include <QPushButton>
#include <QFile>
#include <QMessageBox>

#define WIDTH 1000
#define HEIGHT 600

class MenuAnimator : public QObject
{
public:
    MenuAnimator(QWidget* menuWidget, QLabel* titleLabel, QVBoxLayout* buttonsLayout,
                 int normalWidth, int expandedWidth, QObject* parent = nullptr)
        : QObject(parent), menu(menuWidget), label(titleLabel), layout(buttonsLayout),
        normalSize(normalWidth), expandedSize(expandedWidth)
    {
        // Инициализируем начальное состояние
        menu->setMinimumWidth(normalSize);
        menu->setMaximumWidth(expandedSize);

        // Настройка единой анимации ширины
        widthAnimation = new QPropertyAnimation(menu, "minimumWidth");
        widthAnimation->setDuration(300);
        widthAnimation->setEasingCurve(QEasingCurve::OutQuint);

        // Собираем все кнопки
        for (int i = 0; i < layout->count(); ++i) {
            if (QPushButton* button = qobject_cast<QPushButton*>(layout->itemAt(i)->widget())) {
                buttons.append(button);
                button->setMinimumWidth(0);
                button->setMaximumWidth(expandedWidth - 30);
            }
        }

        // Сразу скрываем кнопки при старте
        collapseMenu(true);
    }

    bool eventFilter(QObject* watched, QEvent* event) override
    {
        if (event->type() == QEvent::Enter) {
            expandMenu();
            return true;
        }
        else if (event->type() == QEvent::Leave) {
            collapseMenu();
            return true;
        }
        return QObject::eventFilter(watched, event);
    }

private:
    void expandMenu()
    {
        menu->setMaximumWidth(WIDTH/4);
        widthAnimation->stop();
        widthAnimation->setStartValue(menu->minimumWidth());
        widthAnimation->setEndValue(expandedSize);
        widthAnimation->start();

        for (auto* btn : buttons) {
            btn->show();
            btn->setEnabled(true);
        }
    }

    void collapseMenu(bool immediate = false)
    {
        widthAnimation->stop();
        if (immediate) {
            menu->setMinimumWidth(normalSize);
        } else {
            widthAnimation->setStartValue(menu->minimumWidth());
            widthAnimation->setEndValue(normalSize);
            widthAnimation->start();
        }

        for (auto* btn : buttons) {
            btn->hide();
            btn->setEnabled(false);
        }
    }

private:
    QWidget* menu;
    QLabel* label;
    QVBoxLayout* layout;
    QList<QPushButton*> buttons;
    int normalSize;
    int expandedSize;
    QPropertyAnimation* widthAnimation;
};

void MainWindow::setupBack(QWidget *widget, QString NameSetWindowTitle)
{
    if (!widget) return;

    QMainWindow *back = qobject_cast<QMainWindow*>(widget);
    if (back)
    {
        back->setWindowTitle(NameSetWindowTitle);
        back->resize(WIDTH, HEIGHT);
        back->setMinimumSize(WIDTH, HEIGHT);

        QWidget *centralWidget = new QWidget(back);
        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        back->setCentralWidget(centralWidget);

        centralWidget->setStyleSheet(
            "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #CAD8E8, stop:1 #163F6E);"
            );

        setupMenu(back, centralWidget);
    }
}

void MainWindow::setupMenu(QMainWindow* parent, QWidget* centralWidget)
{
    QWidget* menu = new QWidget(parent);
    menu->setMinimumWidth(WIDTH / 10 - 20);
    menu->setMaximumWidth(WIDTH/4);
    menu->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    menu->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #CAD8E8, stop:1 #6889AE);"
        "border-top-right-radius: 15px;"
        "border-bottom-right-radius: 15px;"
        "border: none;"
        );

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(menu);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 160));
    shadow->setOffset(5, 0);
    menu->setGraphicsEffect(shadow);

    QVBoxLayout *menuLayout = new QVBoxLayout(menu);
    menuLayout->setContentsMargins(15, 10, 10, 10);

    QLabel *titleLabel = new QLabel("MENU", menu);
    titleLabel->setStyleSheet("color: white; background: transparent; font-size: 20px;");
    menuLayout->addWidget(titleLabel, 0, Qt::AlignTop | Qt::AlignLeft);

    QHBoxLayout* mainLayout = qobject_cast<QHBoxLayout*>(centralWidget->layout());
    mainLayout->addWidget(menu);

    setupButton(menu, menuLayout);

    MenuAnimator* animator = new MenuAnimator(menu, titleLabel, menuLayout,
                                              WIDTH/10 - 20, WIDTH/4, menu);
    menu->installEventFilter(animator);
}

void MainWindow::setupButton(QWidget* widget, QVBoxLayout* layout)
{
    layout->setSpacing(5);

    const QStringList buttonNames = {
        "База Данных",
        "Параметры",
        "Линейная Регрессия",
        "ИИ",
        "Сравнение",
        "Выход"
    };

    QString buttonStyle =
        "QPushButton {"
        "   background: #5B7FAF;"
        "   color: white;"
        "   border-radius: 10px;"
        "   padding: 10px;"
        "   font-size: 14px;"
        "   margin: 5px;"
        "   min-width: 100px;"
        "}"
        "QPushButton:hover {"
        "   background: #6B8FBF;"
        "}";

    for(int i = 0; i < buttonNames.size(); i++)
    {
        QPushButton* btn = new QPushButton(buttonNames[i], widget);
        btn->setStyleSheet(buttonStyle);
        layout->addWidget(btn);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QObject::connect(btn, &QPushButton::clicked, [=]() {
            QMainWindow* currentWindow = qobject_cast<QMainWindow*>(widget->window());

            switch(i) {
            case 0: // База данных
            {
                basedata* window = new basedata();
                if (currentWindow) currentWindow->close();
                window->show();
                break;
            }
            case 1: // Параметры
            {
                parametr* window = new parametr();
                if (currentWindow) currentWindow->close();
                window->show();
                break;
            }
            case 2: // Линейная регрессия
            {
                parametr* paramsWindow = new parametr();
                regressia* window = new regressia(paramsWindow);
                if (currentWindow) currentWindow->close();
                window->show();
                break;
            }
            case 3: // ИИ
            {
                parametr* paramsWindow = new parametr();
                ai* window = new ai(paramsWindow);
                if (currentWindow) currentWindow->close();
                window->show();
                break;
            }
            case 4: // Сравнение
            {
                // Проверяем существование файлов с результатами
                bool hasRegression = QFile::exists("regression_results.csv");
                bool hasAI = QFile::exists("ai_results.csv");

                if (!hasRegression || !hasAI) {
                    QMessageBox::warning(nullptr, "Ошибка",
                                         "Для сравнения необходимо сначала выполнить расчеты в обоих режимах (регрессия и ИИ)");
                    return;
                }

                // Создаем окно сравнения
                Comparison* compareWindow = new Comparison();
                if (currentWindow) currentWindow->close();

                compareWindow->show();
                break;
            }
            case 5: // Выход
                if (currentWindow) currentWindow->close();
                break;
            }
        });
    }

    layout->addStretch();
}
