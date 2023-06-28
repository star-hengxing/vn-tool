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
    QLineEdit* pathLineEdit;
    QToolButton* browseButton;

public:
    program_setting(QWidget* parent = nullptr);

signals:
    void select_file(const QString& path);

private slots:
    void get_file_path();
};
