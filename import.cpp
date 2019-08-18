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
    int count = 0;

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
                ++count;
            }
            file.close();

            line = "Imported %1 NIV records";
            line = line.arg(count);
            msg.setWindowTitle("Import");
            msg.setText(line);
            msg.setIcon(QMessageBox::Information);
            msg.exec();
        }
    }
}

void import::on_cpapImportButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open csv to import", "", "csv files (*.csv)");
    QMessageBox msg;
    QString line;
    QStringList vals, log;
    int count = 0;

    if(filename != "") {
        QFile file(filename);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            msg.setWindowTitle("Import error");
            msg.setText("Can't open file: " + filename);
            msg.setIcon(QMessageBox::Warning);
            msg.exec();
        }

        else {
            log << "RXR,Serial_num,Start,Finish";
            QTextStream in(&file);
            while (!in.atEnd()) {
                line = in.readLine();
                vals = line.split(',');
                if(vals[0] == "RXR") continue;  // Skip header
                if(IxTrak->db_pat_exists(vals[0])) {
                    log << IxTrak->db_import_cpap(vals[0], vals[1], vals[2], vals[3], vals[4], vals[5], vals[6]);
                    ++count;
                }
            }
            file.close();

            QFile logfile("cpap_import.log");
            if(!logfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                msg.setWindowTitle("Import log error");
                msg.setText("Error with log file");
                msg.setIcon(QMessageBox::Warning);
                msg.exec();
            }
            else {
                QTextStream out(&logfile);
                for(int i=0; i<log.size(); ++i)
                    out << log.at(i) << "\n";
            }
            logfile.close();

            line = "Imported %1 CPAP device records. Details written to \"cpap_import.log\"";
            line = line.arg(count);
            msg.setWindowTitle("Import");
            msg.setText(line);
            msg.setIcon(QMessageBox::Information);
            msg.exec();
        }
    }
}

void import::on_ixtrakImportButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open csv to import", "", "csv files (*.csv)");
    QMessageBox msg;
    QStringList log;
    QString line;
    int count = -1;     // To deal with header line of CSV file

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
            log << "Importing: " << filename;
            while (!in.atEnd()) {
                line = in.readLine();
                log << IxTrak->db_import_ixtrak02(line);
                ++count;
            }
            file.close();

            QFile logfile("ixtrak02_import.log");
            if(!logfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                msg.setWindowTitle("Import log error");
                msg.setText("Error with log file");
                msg.setIcon(QMessageBox::Warning);
                msg.exec();
            }
            else {
                QTextStream out(&logfile);
                for(int i=0; i<log.size(); ++i)
                    out << log.at(i) << "\n";
            }
            logfile.close();

            line = "Attempted imported of %1 IxTrak02 records. Details written to \"ixtrak02.log\", including any failed records";
            line = line.arg(count);
            msg.setWindowTitle("Import");
            msg.setText(line);
            msg.setIcon(QMessageBox::Information);
            msg.exec();
        }

    }

}
