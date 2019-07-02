#include "data_entry.h"
#include "ui_data_entry.h"

#include "singleton.h"

#include <QMessageBox>

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

    for(i = contacts.begin(); i != contacts.end(); ++i) i->radio->setChecked(false);
    for(i = disposals.begin(); i != disposals.end(); ++i) i->radio->setChecked(false);
    for(i = diagnoses.begin(); i != diagnoses.end(); ++i) i->check->setChecked(false);
    for(i = referrals.begin(); i != referrals.end(); ++i) i->check->setChecked(false);
    for(i = investigations.begin(); i != investigations.end(); ++i) i->check->setChecked(false);

    ui->followupEdit->clear();
    ui->notesEdit->clear();
    ui->diagEdit->clear();
    ui->ixEdit->clear();
    ui->refEdit->clear();
}

void data_entry::on_pushButton_clicked()
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

void data_entry::on_clearButton_clicked()
{
    clear_input_form();
}
