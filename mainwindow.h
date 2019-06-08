#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "physiology.h"

#include <QMainWindow>

#define CONFIG_FILE_NAME    "ixtrak.cfg"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    physiology limits;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    double validate_physiology(QString s, double val);

private slots:
    void on_actionAbout_triggered();

    void on_actionBMI_triggered();

    void on_actionImport_triggered();

    void on_actionQuit_triggered();

    void on_actionTLco_triggered();

private:
    Ui::MainWindow *ui;
    void closeEvent(QCloseEvent *event);

    void load_phys_limits();
    bool load_config();
};

#endif // MAINWINDOW_H
