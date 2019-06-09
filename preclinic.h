#ifndef PRECLINIC_H
#define PRECLINIC_H

#include <QDialog>

namespace Ui {
class preclinic;
}

class preclinic : public QDialog
{
    Q_OBJECT

public:
    explicit preclinic(QWidget *parent = nullptr);
    ~preclinic();

private slots:
    void on_buttonBox_accepted();

    void on_pushButton_clicked();

private:
    Ui::preclinic *ui;

    void insert_ix();
};

#endif // PRECLINIC_H
