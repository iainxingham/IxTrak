#include "singleton.h"
#include "ixtrak_options.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QDateTime>
#include <QString>
#include <QVariant>
#include <QDebug>

const QString DRIVER("QSQLITE");


// Standalone functions
bool valid_rxr(QString rxr)
{
    if(!rxr.startsWith("rxr", Qt::CaseInsensitive)) return false;
    if(rxr.length() != 10) return false;

    return true;
}

// Prototype in ixtrak_options.h
QString optionToString(Options o)
{
    switch(o) {
    case Options::Contact:
        return "Contact";
    case Options::Disposal:
        return "Disposal";
    case Options::Referral:
        return "Referral";
    case Options::Diagnosis:
        return "Diagnosis";
    case Options::Investigation:
        return "Investigation";
    };

    return "Error";
}

// Singleton class
Singleton::Singleton()
{

}

Singleton::~Singleton()
{
    if(db.open()) db.close();
}

void Singleton::set_physiology_limit(QString s, double min, double max)
{
    physiology_limits.setlimit(s, min, max);
}

double Singleton::validate_physiology(QString s, double val)
{
    return physiology_limits.validate(s, val);
}

void Singleton::set_db_path(QString s)
{
    db_path = s;
}

bool Singleton::db_init_connection()
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

void Singleton::db_insert_preclinic(QString rxr, QString nhs, QString test)
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
int Singleton::db_lookup_or_add(QString table, QString val)
{
    QSqlQuery query;
    QString qu_str = "SELECT id FROM %1 WHERE name = :val";

    qu_str = qu_str.arg(table);
    query.prepare(qu_str);
    query.bindValue(":val", val);
    query.exec();

    if(query.first()) return query.value(0).toInt();

    qu_str = "INSERT INTO %1 (name) VALUES (:val)";
    qu_str = qu_str.arg(table);
    query.prepare(qu_str);
    query.bindValue(":val", val);
    query.exec();

    qu_str = "SELECT id FROM %1 WHERE ROWID = (SELECT last_insert_rowid())";
    qu_str = qu_str.arg(table);
    query.prepare(qu_str);
    query.exec();
    query.first();
    return query.value(0).toInt();
}

int Singleton::db_get_rxr(QString rxr)
{
    QSqlQuery query;

    query.prepare("SELECT id FROM pats WHERE rxr = ?");
    query.addBindValue(rxr.toUpper());
    query.exec();

    if(query.first()) return query.value(0).toInt();
    return -1;
}

void Singleton::db_insert_rxr(QString rxr, QString nhs)
{
    QSqlQuery query;

    if(db_get_rxr(rxr) != -1) return;

    query.prepare("INSERT INTO pats (rxr, nhs) VALUES (:rxr_num, :nhs_num)");
    query.bindValue(":rxr_num", rxr.toUpper());
    query.bindValue(":nhs_num", nhs);
    query.exec();
}

void Singleton::db_insert_physiology(QString rxr, QString type, double val, double high, double low)
{
    int rxr_id, phys_id;
    QSqlQuery query;
    QDateTime dt;

    //qDebug() << "Database: " << db.isOpen();

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

    entered.append(rxr + " - physiology: " + type);
}

/*  Slightly hacky return type holding
 *
 *     < <rxr_id, interaction_id>, datetime string >
 *
 *  as nested QPairs
 */
QPair<QPair<int, int>, QString> Singleton::db_insert_preclinic_interaction(QString rxr, QString nhs)
{
    int rxr_id, contact_num, int_num;
    QSqlQuery query;
    QDateTime dt;
    QString cur_dt;

    rxr_id = db_get_rxr(rxr);
    if(rxr_id == -1) {
        db_insert_rxr(rxr, nhs);
        rxr_id = db_get_rxr(rxr);
    }

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

    return qMakePair(qMakePair(rxr_id, int_num), cur_dt);
}

void Singleton::db_insert_preclinic_ix(QPair<QPair<int, int>, QString> details, QString test)
{
    QSqlQuery query;
    int test_num;

    test_num = db_lookup_or_add("inv_types", test);
    query.prepare("INSERT INTO investigation (pat_id, interact_id, inv_type, request_date) "
                  "VALUES (:pat_id, :int_num, :test_num, :int_date)");
    query.bindValue(":int_date", details.second);
    query.bindValue(":pat_id", details.first.first);
    query.bindValue(":test_num", test_num);
    query.bindValue(":int_num", details.first.second);
    query.exec();
}

int Singleton::db_get_or_add_rxr(QString rxr, QString nhs)
{
    int rxr_id;

    rxr_id = db_get_rxr(rxr);
    if(rxr_id == -1) {
        db_insert_rxr(rxr, nhs);
        rxr_id = db_get_rxr(rxr);
    }

    return rxr_id;
}

void Singleton::log_db_entry(QString s)
{
    entered.append(s);
}

QString Singleton::get_logs()
{
    QString ret = "";

    for (int i = 0; i < entered.size(); ++i) {
        ret += (entered.at(i) + "\n");
    }

    return ret;
}

void Singleton::set_data_option(Options opt, QString db, QString label)
{
    ShortOption so;

    so.opt = opt;
    so.dbName = db;
    so.label = label;
    dataOptions.append(so);
}

void Singleton::get_options(Options opt, QList<IxTrakOption> &list)
{
    IxTrakOption ito;

    for (int i=0; i < dataOptions.size(); i++) {
        if(dataOptions.at(i).opt == opt) {
            ito.dbName = dataOptions.at(i).dbName;
            ito.label = dataOptions.at(i).label;
            ito.check = nullptr;
            ito.radio = nullptr;

            if(opt == Options::Contact)
                ito.dbNum = db_lookup_or_add("contact_types", ito.dbName);
            else if(opt == Options::Investigation)
                ito.dbNum = db_lookup_or_add("inv_types", ito.dbName);
            else if(opt == Options::Disposal)
                ito.dbNum = db_lookup_or_add("disp_types", ito.dbName);
            else ito.dbNum = 0;

            list.append(ito);
        }
    }
}
