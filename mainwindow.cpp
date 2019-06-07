#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "bmicalc.h"
#include "tlcocalc.h"

#include <QMessageBox>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    load_phys_limits();
}

MainWindow::~MainWindow()
{
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
    //QMessageBox::about(this, "IxTrak", "BMI calculator not implemented");
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

double MainWindow::validate_physiology(QString s, double val)
{
    return limits.validate(s, val);
}

void MainWindow::load_phys_limits()
{
    // Hack for now
    limits.setlimit("Height", 100, 230);
    limits.setlimit("Weight", 30, 300);
    limits.setlimit("Haemoglobin", 2.0, 22.0);
    limits.setlimit("TLco", 2.0, 18.0);
    limits.setlimit("Haematocrit", 10, 75);
}
