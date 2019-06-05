#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    QMessageBox::about(this, "IxTrak", "BMI calculator not implemented");
}

void MainWindow::on_actionImport_triggered()
{
    QMessageBox::about(this, "IxTrak", "Import not implemented");
}

void MainWindow::on_actionQuit_triggered()
{
    qApp->quit();
}
