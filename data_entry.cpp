#include "data_entry.h"
#include "ui_data_entry.h"

#include "singleton.h"

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
