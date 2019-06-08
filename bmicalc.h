#ifndef BMICALC_H
#define BMICALC_H

#include <QDialog>

namespace Ui {
class BMICalc;
}

class BMICalc : public QDialog
{
    Q_OBJECT

public:
    explicit BMICalc(QWidget *parent = nullptr);
    ~BMICalc();

private slots:
    void on_calcButton_clicked();

private:
    Ui::BMICalc *ui;
    void setValuesToBlank();
    QString BMItoClass(double BMIval);
};

#endif // BMICALC_H
