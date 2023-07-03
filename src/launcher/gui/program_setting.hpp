#pragma once

#include <QDialog>

class QWidget;
class QString;
class QLineEdit;
class QToolButton;

struct program_setting : public QDialog
{
    Q_OBJECT
private:
    using Self = program_setting;

private:
    QLineEdit* filename;
    QToolButton* read_file_button;

public:
    program_setting(QWidget* parent = nullptr);

private slots:
    void get_file_path();
};
