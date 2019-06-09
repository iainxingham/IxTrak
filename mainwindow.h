#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "physiology.h"

#include <QMainWindow>
#include <QSqlDatabase>

#define CONFIG_FILE_NAME    "ixtrak.cfg"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    physiology limits;
    QString db_path;
    QSqlDatabase db;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    double validate_physiology(QString s, double val);
    void db_insert_physiology(QString rxr, QString type, double val, double high=0, double low=0);
    bool valid_rxr(QString rxr);

private slots:
    void on_actionAbout_triggered();

    void on_actionBMI_triggered();

    void on_actionImport_triggered();

    void on_actionQuit_triggered();

    void on_actionTLco_triggered();

    void on_actionPhysiology_triggered();

    void on_actionMetric_to_Imperial_triggered();

private:
    Ui::MainWindow *ui;
    void closeEvent(QCloseEvent *event);

    void load_phys_limits();
    bool load_config();
    bool init_db_connection();

    int db_get_rxr(QString rxr);
    void db_insert_rxr(QString rxr, QString nhs="NULL");
};

#endif // MAINWINDOW_H
