#pragma once

#include <QString>
#include <QLineEdit>
#include <QToolButton>
#include <QFileDialog>
#include <QGridLayout>
#include <QPushButton>

#include "program_setting.hpp"

program_setting::program_setting(QWidget* parent) :
    QDialog(parent)
{
    this->setWindowTitle("Settings");
    // this->resize(300, 300);

    pathLineEdit = new QLineEdit{};
    browseButton = new QToolButton{};
    browseButton->setText("Browse...");

    auto layout = new QGridLayout{};
    layout->addWidget(pathLineEdit, 0, 0, 1, 2); 
    layout->addWidget(browseButton, 0, 2);

    auto ok_button = new QPushButton{"OK"};
    auto cancel_button = new QPushButton{"Cancel"};
    layout->addWidget(ok_button, 1, 2, 1, 1);
    layout->addWidget(cancel_button, 1, 3, 1, 1);

    this->setLayout(layout);

    this->connect(browseButton, &QToolButton::clicked, this, &Self::get_file_path);
    this->connect(ok_button, &QPushButton::clicked, this, &Self::accept);
    this->connect(cancel_button, &QPushButton::clicked, this, &Self::reject);
}

void program_setting::get_file_path()
{
    QString filter = "Executable Files (*.exe)";
    QString file = QFileDialog::getOpenFileName(this, "Select Program", "", filter);

    if (!file.isEmpty())
    {
        pathLineEdit->setText(file);
        emit select_file(file);
    }
}
