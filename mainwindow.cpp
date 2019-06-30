#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "bmicalc.h"
#include "tlcocalc.h"
#include "insertphys.h"
#include "convert.h"
#include "preclinic.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>
#include <QDebug>

const QString DRIVER("QSQLITE");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //load_phys_limits();
    load_config();
    init_db_connection();
}

MainWindow::~MainWindow()
{
    db.close();
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

double MainWindow::validate_physiology(QString s, double val)
{
    return limits.validate(s, val);
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
                    limits.setlimit(strlist[0].trimmed(), strlist[1].toDouble(), strlist[2].toDouble());
                    //qDebug() << "Physiology: " << strlist[0].trimmed() << " min: " << strlist[1].toDouble() << " max: " << strlist[2].toDouble();
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
            if(QFile::exists(strlist[0].trimmed())) db_path = strlist[0].trimmed();
            continue;
        }

        // Other configs here as else ifs
    }

    return true;
}

bool MainWindow::init_db_connection()
{
    if(!QSqlDatabase::isDriverAvailable(DRIVER)) {
        QMessageBox msg;

        msg.setWindowTitle("Error");
        msg.setText("Database driver unavailable");
        msg.setIcon(QMessageBox::Critical);
        msg.exec();

        return false;
    }

    db = db.addDatabase(DRIVER);
    db.setDatabaseName(db_path);

    if(!db.open()) {
        QMessageBox msg;

        msg.setWindowTitle("Error");
        msg.setText("Can't open database");
        msg.setIcon(QMessageBox::Critical);
        msg.exec();

        return false;
    }

    return true;
}

void MainWindow::on_actionPhysiology_triggered()
{
    insertphys *phys;

    phys = new insertphys(this);
    phys->exec();
    delete phys;
}

int MainWindow::db_get_rxr(QString rxr)
{
    QSqlQuery query;

    query.prepare("SELECT id FROM pats WHERE rxr = ?");
    query.addBindValue(rxr);
    query.exec();

    if(query.first()) return query.value(0).toInt();
    return -1;
}

void MainWindow::db_insert_rxr(QString rxr, QString nhs)
{
    QSqlQuery query;

    if(db_get_rxr(rxr) != -1) return;

    query.prepare("INSERT INTO pats (rxr, nhs) VALUES (:rxr_num, :nhs_num)");
    query.bindValue(":rxr_num", rxr);
    query.bindValue(":nhs_num", nhs);
    query.exec();
}

void MainWindow::db_insert_physiology(QString rxr, QString type, double val, double high, double low)
{
    int rxr_id, phys_id;
    QSqlQuery query;
    QDateTime dt;

    rxr_id = db_get_rxr(rxr);
    if(rxr_id == -1) {
        db_insert_rxr(rxr);
        rxr_id = db_get_rxr(rxr);
    }

    query.prepare("SELECT id FROM phys_types WHERE measure = :phys");
    query.bindValue(":phys", type);
    query.exec();

    if(!query.first()) return;

    phys_id = query.value(0).toInt();

    if(high <= low) {   // ie no normal range values
        query.prepare("INSERT INTO physiology (pat_id, phys_date, phys_type, phys_value) "
                      "VALUES (:rxr_id, :cur_date, :phys, :val)");
    }
    else {
        query.prepare("INSERT INTO physiology (pat_id, phys_date, phys_type, phys_value, ref_high, ref_low) "
                      "VALUES (:rxr_id, :cur_date, :phys, :val, :high, :low)");
        query.bindValue(":high", high);
        query.bindValue(":low", low);
    }
    query.bindValue(":rxr_id", rxr_id);
    query.bindValue(":cur_date", dt.currentDateTime().toString("dd:MM:yyyy hh:mm t"));
    query.bindValue(":phys", phys_id);
    query.bindValue(":val", val);

    query.exec();
}

bool MainWindow::valid_rxr(QString rxr)
{
    if(!rxr.startsWith("rxr")) return false;
    if(rxr.length() != 10) return false;

    return true;
}

void MainWindow::on_actionMetric_to_Imperial_triggered()
{
    convert *conv;

    conv = new convert(this);
    conv->exec();
    delete conv;
}

void MainWindow::on_actionPre_clinic_Ix_triggered()
{
    preclinic *preclin;

    preclin = new preclinic(this);
    preclin->exec();
    delete preclin;
}

void MainWindow::db_insert_preclinic(QString rxr, QString nhs, QString test)
{
    int rxr_id, test_num, contact_num, int_num;
    QSqlQuery query;
    QDateTime dt;
    QString cur_dt;

    rxr_id = db_get_rxr(rxr);
    if(rxr_id == -1) {
        db_insert_rxr(rxr, nhs);
        rxr_id = db_get_rxr(rxr);
    }

    test_num = db_lookup_or_add("inv_types", test);
    contact_num = db_lookup_or_add("contact_types", "Pre clinic investigation");
    cur_dt = dt.currentDateTime().toString("dd:MM:yyyy hh:mm t");

    query.prepare("INSERT INTO interaction (interact_date, pat_id, interact_type) "
                  "VALUES (:int_date, :pat_id, :contact)");
    query.bindValue(":int_date", cur_dt);
    query.bindValue(":pat_id", rxr_id);
    query.bindValue(":contact", contact_num);
    query.exec();

    query.exec("SELECT id FROM interaction WHERE ROWID = (SELECT last_insert_rowid())");
    query.first();
    int_num = query.value(0).toInt();
    qDebug() << "Interaction id: " << int_num;
    qDebug() << "Test_num: " << test_num;

    query.prepare("INSERT INTO investigation (pat_id, interact_id, inv_type, request_date) "
                  "VALUES (:pat_id, :int_num, :test_num, :int_date)");
    query.bindValue(":int_date", cur_dt);
    query.bindValue(":pat_id", rxr_id);
    query.bindValue(":test_num", test_num);
    query.bindValue(":int_num", int_num);
    query.exec();
}

// Works for
//   - inv_types
//   - clin_grades
//   - contact_types
//   - disp_types
int MainWindow::db_lookup_or_add(QString table, QString val)
{
    QSqlQuery query;
    QString qu_str = "SELECT id FROM %1 WHERE name = :val";

    /* Not sure why can't use bindValue() in FROM part of SQL statement but it doesn't work.
     *
     * if(table == "inv_types") query.prepare("SELECT id FROM inv_types WHERE name = :val");
    else if(table == "contact_types") query.prepare("SELECT id FROM contact_types WHERE name = :val");
    else if(table == "disp_types") query.prepare("SELECT id FROM disp_types WHERE name = :val");*/

    //query.prepare("SELECT id FROM :tbl WHERE name = :val");
    //query.bindValue(":tbl", table);
    qu_str = qu_str.arg(table);
    query.prepare(qu_str);
    query.bindValue(":val", val);

    if(query.first()) return query.value(0).toInt();

    qu_str = "INSERT INTO %1 (name) VALUES (:val)";
    qu_str = qu_str.arg(table);
    query.prepare(qu_str);
    query.bindValue(":val", val);
    //query.bindValue(":table", table);
    query.exec();

    //query.prepare("SELECT id FROM :table WHERE name = :val");
    //query.bindValue(":val", val);
    //query.bindValue(":table", table);
    //query.exec();

    qu_str = "SELECT id FROM %1 WHERE ROWID = (SELECT last_insert_rowid())";
    qu_str = qu_str.arg(table);
    query.prepare(qu_str);
    query.exec();
    //qDebug() << "First; " << query.first() << "Last row: " << query.value(0).toInt();
    query.first();
    return query.value(0).toInt();
}

bool MainWindow::db_isOK()
{
    if(!db.open()) {
        qDebug() << "Database isn't open";
        return false;
    }

    return true;
}
