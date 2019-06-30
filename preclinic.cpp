#include "preclinic.h"
#include "ui_preclinic.h"

#include "mainwindow.h"

#include <QMessageBox>

preclinic::preclinic(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::preclinic)
{
    ui->setupUi(this);
}

preclinic::~preclinic()
{
    delete ui;
}

void preclinic::on_buttonBox_accepted()
{
    insert_ix();
}

void preclinic::on_pushButton_clicked()
{
    insert_ix();

    ui->rxrEdit->setText("");
    ui->nhsEdit->setText("");
    ui->oxiCheck->setChecked(false);
    ui->domiCheck->setChecked(false);
    ui->oasysCheck->setChecked(false);
    ui->ctCheck->setChecked(false);
}

void preclinic::insert_ix()
{
    MainWindow *mw;
    QMessageBox msg;
    QString str, nhs_num;

    mw = qobject_cast<MainWindow*> (this->parent());

    if(!mw->valid_rxr(ui->rxrEdit->text())) {
        msg.setWindowTitle("Warning");
        str = "Invalid RXR: \"%1\"";
        str = str.arg(ui->rxrEdit->text());
        msg.setText(str);
        msg.setIcon(QMessageBox::Warning);
        msg.exec();
        return;
    }

    nhs_num = ui->nhsEdit->text();
    if(nhs_num == "") nhs_num = "NULL";

    // Will be separate interactions if multiple Ix requested
    // Deal with by refactoring to take db out of mainwindow.cpp?
    if(ui->oxiCheck->isChecked())
        mw->db_insert_preclinic(ui->rxrEdit->text(), nhs_num, "Overnight oximetry");
    if(ui->domiCheck->isChecked())
        mw->db_insert_preclinic(ui->rxrEdit->text(), nhs_num, "Limited polysomnography");
    if(ui->oasysCheck->isChecked())
        mw->db_insert_preclinic(ui->rxrEdit->text(), nhs_num, "Oasys diary");
    if(ui->ctCheck->isChecked())
        mw->db_insert_preclinic(ui->rxrEdit->text(), nhs_num, "CT thorax");
}
