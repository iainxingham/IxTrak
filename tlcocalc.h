#ifndef TLCOCALC_H
#define TLCOCALC_H

#include <QDialog>

namespace Ui {
class tlcocalc;
}

class tlcocalc : public QDialog
{
    Q_OBJECT

public:
    explicit tlcocalc(QWidget *parent = nullptr);
    ~tlcocalc();

private slots:
    void on_calculateButton_clicked();

private:
    Ui::tlcocalc *ui;
};

#endif // TLCOCALC_H
