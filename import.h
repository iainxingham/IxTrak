#ifndef IMPORT_H
#define IMPORT_H

#include <QDialog>

namespace Ui {
class import;
}

class import : public QDialog
{
    Q_OBJECT

public:
    explicit import(QWidget *parent = nullptr);
    ~import();

private slots:
    void on_nivImportButton_clicked();

    void on_cpapImportButton_clicked();

    void on_ixtrakImportButton_clicked();

private:
    Ui::import *ui;
};

#endif // IMPORT_H
