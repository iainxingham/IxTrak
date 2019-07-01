#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "physiology.h"
#include "ixtrak_options.h"

#include <QMainWindow>
#include <QTextStream>

#define CONFIG_FILE_NAME    "ixtrak.cfg"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    // Private variables - nil currently

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionAbout_triggered();

    void on_actionBMI_triggered();

    void on_actionImport_triggered();

    void on_actionQuit_triggered();

    void on_actionTLco_triggered();

    void on_actionPhysiology_triggered();

    void on_actionMetric_to_Imperial_triggered();

    void on_actionPre_clinic_Ix_triggered();

    void on_actionNew_interaction_triggered();

private:
    Ui::MainWindow *ui;
    void closeEvent(QCloseEvent *event);

    bool load_config();
    void set_entry_logs();
    void read_options(QTextStream &in, Options opt);

};

#endif // MAINWINDOW_H
