#include "preclinic.h"
#include "ui_preclinic.h"

//#include "mainwindow.h"
#include "singleton.h"

#include <QMessageBox>

preclinic::preclinic(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::preclinic)
{
    ui->setupUi(this);
    set_entry_log();
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

    set_entry_log();
}

void preclinic::insert_ix()
{
    //MainWindow *mw;
    QMessageBox msg;
    QString str, nhs_num;
    QPair<QPair<int, int>, QString> interaction;
    bool added_test = false;

    //mw = qobject_cast<MainWindow*> (this->parent());

    if(!valid_rxr(ui->rxrEdit->text())) {
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

    if(!(ui->oxiCheck->isChecked() ||
         ui->domiCheck->isChecked() ||
         ui->oasysCheck->isChecked() ||
         ui->ctCheck->isChecked())) return;

    interaction = IxTrak->db_insert_preclinic_interaction(ui->rxrEdit->text(), nhs_num);
    str = ui->rxrEdit->text() + " - pre clinic ix: ";

    if(ui->oxiCheck->isChecked()) {
        IxTrak->db_insert_preclinic_ix(interaction, "Overnight oximetry");
        str += "oxi";
        added_test = true;
    }
    if(ui->domiCheck->isChecked()) {
        IxTrak->db_insert_preclinic_ix(interaction, "Limited polysomnography");
        if(added_test) str += ", ";
        str += "5 channel";
        added_test = true;
    }
    if(ui->oasysCheck->isChecked()) {
        IxTrak->db_insert_preclinic_ix(interaction, "Oasys diary");
        if(added_test) str += ", ";
        str += "Oasys";
        added_test = true;
    }
    if(ui->ctCheck->isChecked()) {
        IxTrak->db_insert_preclinic_ix(interaction, "CT thorax");
        if(added_test) str += ", ";
        str += "CT";
        added_test = true;
    }

    IxTrak->log_db_entry(str);
}

void preclinic::set_entry_log()
{
    ui->prevEntryLog->setPlainText(IxTrak->get_logs());
}
