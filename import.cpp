#include "import.h"
#include "ui_import.h"
#include "singleton.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

import::import(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::import)
{
    ui->setupUi(this);
}

import::~import()
{
    delete ui;
}

void import::on_nivImportButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open csv to import", "", "csv files (*.csv)");
    QMessageBox msg;
    QString line;
    QStringList vals;

    if(filename != "") {
        QFile file(filename);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            msg.setWindowTitle("Import error");
            msg.setText("Can't open file: " + filename);
            msg.setIcon(QMessageBox::Warning);
            msg.exec();
        }

        else {
            QTextStream in(&file);
            while (!in.atEnd()) {
                line = in.readLine();
                vals = line.split(',');
                if(vals[0] == "RXR") continue;
                IxTrak->db_import_device(vals[0], vals[1], vals[2], vals[3], vals[4], vals[5], vals[6]);
            }
        }
    }
}
