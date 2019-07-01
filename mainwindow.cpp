#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "bmicalc.h"
#include "tlcocalc.h"
#include "insertphys.h"
#include "convert.h"
#include "preclinic.h"
#include "singleton.h"
#include "data_entry.h"
#include "ixtrak_options.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    IxTrak = new Singleton();
    load_config();
    IxTrak->db_init_connection();
}

MainWindow::~MainWindow()
{
    delete IxTrak;
    delete ui;
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "IxTrak", "I Crossingham 2019");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton reallyQ = QMessageBox::question(this, "IxTrak",
                                                                tr("Are you sure?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (reallyQ != QMessageBox::Yes) {
        event->ignore();
    }
    else {
        event->accept();
    }
}

void MainWindow::on_actionBMI_triggered()
{
    BMICalc *bmi;

    bmi = new BMICalc(this);
    bmi->exec();
    delete bmi;
}

void MainWindow::on_actionImport_triggered()
{
    QMessageBox::about(this, "IxTrak", "Import not implemented");
}

void MainWindow::on_actionQuit_triggered()
{
    qApp->quit();
}

void MainWindow::on_actionTLco_triggered()
{
    tlcocalc *tlco;

    tlco = new tlcocalc(this);
    tlco->exec();
    delete tlco;
}

bool MainWindow::load_config()
{
    QFile file(CONFIG_FILE_NAME);
    QTextStream in;
    QMessageBox msg;
    QString line;
    QStringList strlist;

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        msg.setWindowTitle("Error");
        msg.setText("Unable to open configuration file");
        msg.setIcon(QMessageBox::Critical);
        msg.exec();

        return false;
    }

    in.setDevice(&file);
    msg.setWindowTitle("Debug");
    msg.setIcon(QMessageBox::Warning);
    while (!in.atEnd()) {
        line = in.readLine();

        // Handle comments
        if(line.startsWith("#")) continue;      // '#' for comments

        // Handle physiology limits
        if(line.startsWith("Physiology limits:")) {
            line = in.readLine();
            while (line.startsWith(" ") || line.startsWith("\t")){
                if(line.trimmed().startsWith("#")) {
                    line = in.readLine();
                    continue;
                }
                strlist = line.split(",");
                if(strlist.size() >= 3) {       // >= 3 to allow for trailing comments
                    IxTrak->set_physiology_limit(strlist[0].trimmed(), strlist[1].toDouble(), strlist[2].toDouble());
                }
                line = in.readLine();
            }
            continue;   // This means a blank line is needed following the end of physiology limits
        }

        // Load database path
        if(line.startsWith("Database:")) {
            line = in.readLine();
            while (line.trimmed().startsWith("#")) line = in.readLine();
            strlist = line.trimmed().split("#");
            if(QFile::exists(strlist[0].trimmed())) IxTrak->set_db_path(strlist[0].trimmed());
            continue;
        }

        // Load data entry options
        if(line.startsWith("Contact options:")) {
            read_options(in, Options::Contact);
            continue;   // This means a blank line is needed following the end of each option block
        }
        if(line.startsWith("Diagnosis options:")) {
            read_options(in, Options::Diagnosis);
            continue;   // This means a blank line is needed following the end of each option block
        }
        if(line.startsWith("Investigation options:")) {
            read_options(in, Options::Investigation);
            continue;   // This means a blank line is needed following the end of each option block
        }
        if(line.startsWith("Referral options:")) {
            read_options(in, Options::Referral);
            continue;   // This means a blank line is needed following the end of each option block
        }
        if(line.startsWith("Disposal options:")) {
            read_options(in, Options::Disposal);
            continue;   // This means a blank line is needed following the end of each option block
        }

        // Other configs here as else ifs
    }

    return true;
}

void MainWindow::set_entry_logs()
{
    ui->dbEntryLog->setPlainText(IxTrak->get_logs());
}

void MainWindow::read_options(QTextStream &in, Options opt)
{
    QString line;
    QStringList strlist;

    line = in.readLine();
    while (line.startsWith(" ") || line.startsWith("\t")) {
        if(line.trimmed().startsWith("#")) {
            line = in.readLine();
            continue;
        }

        strlist = line.split(",");
        if(strlist.size() >= 2) {       // >= 2 to allow for trailing comments
            IxTrak->set_data_option(opt, strlist[0].trimmed(), strlist[1].trimmed());
            /*qDebug() << "Option: " << optionToString(opt) << " DB name: "
                     << strlist[0].trimmed() << " Label: " << strlist[1].trimmed();*/
        }

        line = in.readLine();
    }
}

void MainWindow::on_actionPhysiology_triggered()
{
    insertphys *phys;

    phys = new insertphys(this);
    phys->exec();
    delete phys;
    set_entry_logs();
}

void MainWindow::on_actionMetric_to_Imperial_triggered()
{
    convert *conv;

    conv = new convert(this);
    conv->exec();
    delete conv;
    //set_entry_logs();
}

void MainWindow::on_actionPre_clinic_Ix_triggered()
{
    preclinic *preclin;

    preclin = new preclinic(this);
    preclin->exec();
    delete preclin;
    set_entry_logs();
}

void MainWindow::on_actionNew_interaction_triggered()
{
    data_entry *de;

    de = new data_entry(this);
    de->exec();
    delete de;
    set_entry_logs();
}
