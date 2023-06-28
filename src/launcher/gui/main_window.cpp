#include <string>

#include <QApplication>
#include <QListWidget>
#include <QString>
#include <QListWidgetItem>
#include <QMenu>
#include <QAction>

#include "program_setting.hpp"
#include "Program.hpp"

int main_window(int argc, char* argv[]) noexcept
{
    QApplication app(argc, argv);

    QPalette palette;
    palette.setColor(QPalette::Base, QColor{37, 37, 37});

    QListWidget listWidget;
    listWidget.resize(800, 600);
    listWidget.setPalette(palette);
    listWidget.setContextMenuPolicy(Qt::CustomContextMenu);

    auto program_menu = new QMenu{};
    auto settings_action = new QAction{"Settings", &listWidget};
    program_menu->addAction(settings_action);

    program_setting settingsDialog;
    QObject::connect(settings_action, &QAction::triggered, &settingsDialog, &program_setting::exec);

    // QObject::connect(&listWidget, &QListWidget::itemDoubleClicked, [](QListWidgetItem* item) {
    //     // start a program
    // });

    QObject::connect(&settingsDialog, &program_setting::select_file, [&](const QString& path) {
        qDebug() << path;
    });

    QObject::connect(&listWidget, &QListWidget::customContextMenuRequested, [&](const QPoint& pos) {
        program_menu->exec(listWidget.mapToGlobal(pos));
    });

    std::vector<Program> programs;
    programs.emplace_back(L"self", true);
    programs.emplace_back(L"hello", false);
    programs.emplace_back(L"world", false);
    programs.emplace_back(L"haha", true);
    programs.emplace_back(L"this", false);

    QFont font;
    font.setPointSize(14);
    for (const Program& program : programs)
    {
        auto item = new QListWidgetItem{};
        item->setText(QString::fromStdWString(program.path));
        item->setFont(font);
        item->setForeground(Qt::white);
        listWidget.addItem(item);
    }

    listWidget.show();

    return app.exec();
}
