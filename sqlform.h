#ifndef SQLFORM_H
#define SQLFORM_H

#include <QDialog>

namespace Ui {
class SQLform;
}

class SQLform : public QDialog
{
    Q_OBJECT

public:
    explicit SQLform(QWidget *parent = nullptr);
    ~SQLform();

private slots:
    void on_pushButton_clicked();

private:
    Ui::SQLform *ui;
};

#endif // SQLFORM_H
