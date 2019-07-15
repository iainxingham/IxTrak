#include "data_entry.h"
#include "ui_data_entry.h"

#include "singleton.h"

#include <QMessageBox>
#include <QDateTime>
#include <QDebug>

data_entry::data_entry(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::data_entry)
{
    ui->setupUi(this);

    IxTrak->get_options(Options::Contact, contacts);
    IxTrak->get_options(Options::Disposal, disposals);
    IxTrak->get_options(Options::Investigation, investigations);
    IxTrak->get_options(Options::Referral, referrals);
    IxTrak->get_options(Options::Diagnosis, diagnoses);

    set_option_pointers(contacts, true, ui->contactBox);
    set_option_pointers(disposals, true, ui->disposalBox);
    set_option_pointers(diagnoses, false, ui->diagnosisBox);
    set_option_pointers(referrals, false, ui->referralBox);
    set_option_pointers(investigations, false, ui->investigationBox);

    IxTrak->populate_clinician_box(ui->seenbyCombo);
}

data_entry::~data_entry()
{
    delete ui;
}

void data_entry::update_entry_log()
{
    ui->enteredLog->setPlainText(IxTrak->get_logs());
}

void data_entry::set_option_pointers(QList<IxTrakOption> &list, bool radio, QGroupBox *box)
{
    QVBoxLayout *vbox = new QVBoxLayout;
    QList<IxTrakOption>::iterator i;

    for (i = list.begin(); i != list.end(); ++i) {
        if(radio) {
            i->radio = new QRadioButton(i->label);
            i->radio->setChecked(false);
            vbox->addWidget(i->radio);
        }
        else {
            i->check = new QCheckBox(i->label);
            i->check->setChecked(false);
            vbox->addWidget(i->check);
        }
    }

    // add stretch?

    box->setLayout(vbox);
}

void data_entry::clear_input_form()
{
    QList<IxTrakOption>::iterator i;

    ui->rxrEdit->clear();
    ui->nhsEdit->clear();

    ui->seenbyCheckBox->setChecked(false);
    ui->radiologyCheckBox->setChecked(false);
    ui->admitCheckBox->setChecked(false);

    for(i = contacts.begin(); i != contacts.end(); ++i) {
        if(i->radio->isChecked()) {
            i->radio->setAutoExclusive(false);
            i->radio->setChecked(false);
            i->radio->setAutoExclusive(true);
        }
    }
    for(i = disposals.begin(); i != disposals.end(); ++i) {
        if(i->radio->isChecked()) {
            i->radio->setAutoExclusive(false);
            i->radio->setChecked(false);
            i->radio->setAutoExclusive(true);
        }
    }
    for(i = diagnoses.begin(); i != diagnoses.end(); ++i) i->check->setChecked(false);
    for(i = referrals.begin(); i != referrals.end(); ++i) i->check->setChecked(false);
    for(i = investigations.begin(); i != investigations.end(); ++i) i->check->setChecked(false);

    ui->followupEdit->clear();
    ui->notesEdit->clear();
    ui->diagEdit->clear();
    ui->ixEdit->clear();
    ui->refEdit->clear();
}

void data_entry::save_entered_data()
{
    int rxr_id, interact_type, disposal_type, seen_by, interact_num;
    int ref_count = 0, ix_count = 0;
    QMessageBox msg;
    QString str, nhs_num, dt;
    QString diagnosis_str = "";
    QList<IxTrakOption>::iterator i;

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

    rxr_id = IxTrak->db_get_or_add_rxr(ui->rxrEdit->text(), nhs_num);
    dt = QDateTime::currentDateTime().toString("dd:MM:yyyy hh:mm t");
    interact_type = get_dbentry_from_list(contacts);
    disposal_type = get_dbentry_from_list(disposals);

    for(i = diagnoses.begin(); i != diagnoses.end(); ++i) {
        if(i->check->isChecked()) diagnosis_str += (i->dbName + ", ");
    }

    //qDebug() << "seen_by: " << ui->seenbyCheckBox->isChecked();

    seen_by = ui->seenbyCheckBox->isChecked() ? IxTrak->db_get_clinician_id(ui->seenbyCombo->currentText()):
                                                IxTrak->get_primary_clinician_id();
    //qDebug() << "seen_by: " << seen_by;


    interact_num = IxTrak->db_insert_interaction(dt,
                                                 rxr_id,
                                                 interact_type,
                                                 disposal_type,
                                                 ui->radiologyCheckBox->isChecked(),
                                                 ui->admitCheckBox->isChecked(),
                                                 seen_by,
                                                 diagnosis_str,
                                                 ui->followupEdit->text().toInt(),
                                                 ui->notesEdit->toPlainText());

    for(i = investigations.begin(); i != investigations.end(); i++) {
        if(i->check->isChecked()) {
            IxTrak->db_insert_investigation(rxr_id, interact_num, i->dbNum, dt);
            ++ix_count;
        }
    }

    for(i = referrals.begin(); i != referrals.end(); i++) {
        if(i->check->isChecked()) {
            IxTrak->db_insert_referral(rxr_id, interact_num, i->dbName, dt);
            ++ref_count;
        }
    }

    str = ui->rxrEdit->text() + " - " + get_name_from_list(contacts) + ": %1 investigation(s), %2 referral(s)";
    //qDebug() << str << "\nix_count: " << ix_count << "ref_count: " << ref_count;
    str = str.arg(ix_count).arg(ref_count);

    IxTrak->log_db_entry(str);

    update_entry_log();
}

int data_entry::get_dbentry_from_list(QList<IxTrakOption> &list)
{
    QList<IxTrakOption>::iterator i;

    for (i = list.begin(); i != list.end(); ++i) {
        if(i->radio->isChecked()) return i->dbNum;
    }

    return -1;
}

QString data_entry::get_name_from_list(QList<IxTrakOption> &list)
{
    QList<IxTrakOption>::iterator i;

    for (i = list.begin(); i != list.end(); ++i) {
        if(i->radio->isChecked()) return i->dbName;
    }

    return "Error";
}

void data_entry::on_clearButton_clicked()
{
    clear_input_form();
}

void data_entry::on_saveaddButton_clicked()
{
    if(!valid_rxr(ui->rxrEdit->text())) {
        QMessageBox msg;
        msg.setWindowTitle("Warning");
        msg.setText("RXR not valid. No data will be saved.");
        msg.setIcon(QMessageBox::Warning);
        msg.exec();

        return;
    }

    save_entered_data();
    clear_input_form();
}
