#include "convert.h"
#include "ui_convert.h"

#include <QtMath>

convert::convert(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::convert)
{
    ui->setupUi(this);
}

convert::~convert()
{
    delete ui;
}

void convert::on_convertButton_clicked()
{
    bool okl, okw;
    double conv_length, conv_weight, lbs, inches;
    int stone, feet;
    QString str;

    conv_length = ui->lengthEdit->text().toDouble(&okl);
    conv_weight = ui->weightEdit->text().toDouble(&okw);

    if(okl) {
        inches = conv_length * 0.393701;
        feet = qFloor(inches / 12);
        inches = inches - feet * 12;
        str = "%1 ft %2 ins";
        str = str.arg(QString::number(feet), QString::number(inches, 'f', 2));

        ui->imp_length->setText(str);
    }
    else ui->imp_length->setText("-");

    if(okw) {
        lbs = conv_weight * 2.20462;
        stone = qFloor(lbs / 14);
        lbs = lbs - stone * 14;
        str = "%1 st %2 lbs";
        str = str.arg(QString::number(stone), QString::number(lbs, 'f', 2));

        ui->imp_weight->setText(str);
    }
    else ui->imp_weight->setText("-");
}
