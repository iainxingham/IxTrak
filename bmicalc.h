#ifndef BMICALC_H
#define BMICALC_H

#include <QDialog>

// Max and min heights in cm
#define MAX_HEIGHT  220.0
#define MIN_HEIGHT  100.0

// Max and min weights in kg
#define MAX_WEIGHT  300.0
#define MIN_WEIGHT  25.0


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
