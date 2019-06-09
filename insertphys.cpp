#include "insertphys.h"
#include "ui_insertphys.h"

#include "mainwindow.h"

insertphys::insertphys(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::insertphys)
{
    ui->setupUi(this);
}

insertphys::~insertphys()
{
    delete ui;
}

void insertphys::on_buttonBox_accepted()
{
    MainWindow *mw;
    bool okh, okw, okf;
    QString rxr;
    double ht, wt, feno;

    mw = qobject_cast<MainWindow*> (this->parent());

    rxr = ui->rxrEdit->text();
    ht = ui->heightEdit->text().toDouble(&okh);
    wt = ui->weightEdit->text().toDouble(&okw);
    feno = ui->fenoEdit->text().toDouble(&okf);

    if(!mw->valid_rxr(rxr)) return;

    if(okh) {
        ht = mw->validate_physiology("Height", ht);
        if(ht > 0) mw->db_insert_physiology(rxr, "Height", ht);
    }
    if(okw) {
        wt = mw->validate_physiology("Weight", wt);
        if(wt > 0) {
            if(okh && (ht > 0)) mw->db_insert_physiology(rxr, "Weight", wt, (25*(ht/100)*(ht/100)), (18.5*(ht/100)*(ht/100)));
            else mw->db_insert_physiology(rxr, "Weight", wt);
        }
    }
    if(okf) {
        feno = mw->validate_physiology("FeNO", feno);
        if(feno > 0) mw->db_insert_physiology(rxr, "FeNO", feno);
    }
}
