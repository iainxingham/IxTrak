#include "bmicalc.h"
#include "ui_bmicalc.h"

//#include "mainwindow.h"
#include "singleton.h"

#include <QMessageBox>
#include <QString>

BMICalc::BMICalc(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BMICalc)
{
    ui->setupUi(this);
}

BMICalc::~BMICalc()
{
    delete ui;
}

void BMICalc::on_calcButton_clicked()
{
    bool okh, okw;
    double height, weight, bmi;
    QString str;
    //MainWindow *mw;

    //mw = qobject_cast<MainWindow*> (this->parent());

    height = ui->htLine->text().toDouble(&okh);
    weight = ui->wtLine->text().toDouble(&okw);

    if(okh) height = IxTrak->validate_physiology("Height", height);
    if(okw) weight = IxTrak->validate_physiology("Weight", weight);

    if((!okh) || (height < 0)) {
        QMessageBox::about(this, "Error", "Please check height value");
        setValuesToBlank();
    }

    else if((!okw) || (weight < 0)) {
        QMessageBox::about(this, "Error", "Please check weight value");
        setValuesToBlank();
    }

    else {
        height = height / 100.0;
        bmi = (weight / (height * height));

        ui->bmiLabel->setNum(bmi);
        ui->primeLabel->setNum(bmi/25.0);
        str = "%1 to %2";
        str = str.arg(QString::number(18.5 * height * height, 'f', 2), QString::number(25.0 * height * height, 'f', 2));
        ui->normalLabel->setText(str);
        ui->categoryLabel->setText(BMItoClass(bmi));
    }
}

void BMICalc::setValuesToBlank()
{
    ui->bmiLabel->setText("-");
    ui->primeLabel->setText("-");
    ui->normalLabel->setText("-");
    ui->categoryLabel->setText("-");
}

QString BMICalc::BMItoClass(double BMIval)
{
    QString str;

    if(BMIval < 15.0) str = "Very severely underweight";
    else if(BMIval < 16.0) str = "Severely underweight";
    else if(BMIval < 18.5) str = "Underweight";
    else if(BMIval < 25.0) str = "Healthy weight";
    else if(BMIval < 30.0) str = "Overweight";
    else if(BMIval < 35.0) str = "Obese Class I (Moderately obese)";
    else if(BMIval < 40.0) str = "Obese Class II (Severely obese)";
    else if(BMIval < 45.0) str = "Obese Class III (Very severely obese)";
    else if(BMIval < 50.0) str = "Obese Class IV (Morbidly Obese)";
    else if(BMIval < 60.0) str = "Obese Class V (Super Obese)";
    else str = "Obese Class VI (Hyper Obese)";

    return str;
}
