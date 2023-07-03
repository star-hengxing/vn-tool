#include <vector>

#include <QApplication>
#include <QListWidget>
#include <QString>
#include <QListWidgetItem>
#include <QMenu>
#include <QAction>

#include <base/unsafe/buffer_view.hpp>
#include <base/range.hpp>
#include "program_setting.hpp"
#include "Program.hpp"

struct GUI
{
    QFont font;
    std::vector<QListWidgetItem*> item_list;

public:
    GUI(const unsafe::buffer_view<Program> programs);

    void run() noexcept;

    QListWidgetItem* allocate_item(QString&& text) const noexcept;
};

QListWidgetItem* GUI::allocate_item(QString&& text) const noexcept
{
    auto item = new QListWidgetItem{};
    item->setText(text);
    item->setFont(font);
    item->setForeground(Qt::white);
    return item;
}

GUI::GUI(const unsafe::buffer_view<Program> programs)
{
    QPalette palette;
    palette.setColor(QPalette::Base, QColor{37, 37, 37});

    QListWidget list_widget;
    list_widget.resize(800, 600);
    list_widget.setPalette(palette);
    list_widget.setContextMenuPolicy(Qt::CustomContextMenu);

    auto program_menu = new QMenu{};
    auto settings_action = new QAction{"Settings", &list_widget};
    program_menu->addAction(settings_action);

    QObject::connect(&list_widget, &QListWidget::customContextMenuRequested, [&](const QPoint& pos) {
        program_menu->exec(list_widget.mapToGlobal(pos));
    });

    program_setting settings_dialog;
    QObject::connect(settings_action, &QAction::triggered, &settings_dialog, &program_setting::exec);

    QObject::connect(&list_widget, &QListWidget::itemDoubleClicked, [this](QListWidgetItem* item) {
        isize index = -1;
        for (auto i : range(item_list.size()))
        {
            if (item_list[i] == item)
            {
                index = i;
                break;
            }
        }

        if (index != -1)
        {
            qDebug() << index;
            // start a program
        }
    });

    font.setPointSize(14);
    item_list.resize(programs.size());
    for (auto i : range(programs.size()))
    {
        auto text = QString::fromStdWString(programs[i].path);
        item_list[i] = this->allocate_item(std::move(text));
    }

    list_widget.show();
}
