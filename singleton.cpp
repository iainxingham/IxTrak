#include "singleton.h"
#include "ixtrak_options.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QDateTime>
#include <QString>
#include <QVariant>
#include <QSqlRecord>
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

int Singleton::db_insert_interaction(QString date, int patid, int inttype, int disp, bool for_radiology,
                                     bool admit, int seenby, QString diagnosis, int follow_up, QString notes)
{
    QSqlQuery query;

    query.prepare("INSERT INTO interaction (interact_date, pat_id, interact_type, disposal, for_radiology, "
                                           "admit, seen_by, diagnosis, follow_up, notes) "
                  "VALUES (:interact_date, :pat_id, :interact_type, :disposal, :for_radiology, "
                          ":admit, :seen_by, :diagnosis, :follow_up, :notes)");
    query.bindValue(":interact_date", date);
    query.bindValue(":pat_id", patid);
    query.bindValue(":interact_type", inttype);
    query.bindValue(":disposal", disp);
    query.bindValue(":for_radiology", for_radiology ? 1 : 0);
    query.bindValue(":admit", admit ? 1 : 0);
    query.bindValue(":seen_by", seenby);
    query.bindValue(":diagnosis", diagnosis);
    query.bindValue(":follow_up", follow_up);
    query.bindValue(":notes", notes);
    query.exec();

    query.exec("SELECT id FROM interaction WHERE ROWID = (SELECT last_insert_rowid())");
    query.first();
    return (query.value(0).toInt());
}

void Singleton::db_insert_investigation(int patid, int interactid, int invid, QString date, QString notes)
{
    QSqlQuery query;

    query.prepare("INSERT INTO investigation (pat_id, interact_id, inv_type, request_date, notes) "
                  "VALUES (:pat_id, :interact_id, :inv_type, :request_date, :notes)");
    query.bindValue(":pat_id", patid);
    query.bindValue(":interact_id", interactid);
    query.bindValue(":inv_type", invid);
    query.bindValue(":request_date", date);
    query.bindValue(":notes", notes);

    query.exec();
}

void Singleton::db_insert_referral(int patid, int interactid, QString speciality, QString date)
{
    QSqlQuery query;

    query.prepare("INSERT INTO referral (pat_id, interact_id, request_date, speciality) "
                  "VALUES (:pat_id, :interact_id, :request_date, :speciality)");
    query.bindValue(":pat_id", patid);
    query.bindValue(":interact_id", interactid);
    query.bindValue(":request_date", date);
    query.bindValue(":speciality", speciality);

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

int Singleton::db_get_clinician_id(QString name)
{
    QSqlQuery query;

    query.prepare("SELECT id FROM clinician WHERE name = :name");
    query.bindValue(":name", name);
    query.exec();
    query.first();
    return query.value(0).toInt();
}

void Singleton::db_get_active_clinicians()
{
    QSqlQuery query;
    Clinician c;

//    qDebug() << "In db_get_active_clinicians()";
    query.exec("SELECT id, name, grade FROM clinician WHERE active > 0");
    while(query.next()) {
        c.id = query.value(0).toInt();
        c.name = query.value(1).toString();
        c.grade = query.value(2).toInt();
        clinicians.append(c);
        //qDebug() << "db_get_active_clinician() Added: " << c.id << " " << c.name << " " << c.grade;
    }

    load_primary_clinician();
}

bool Singleton::db_run_sql(QString cmd, QStringList &result)
{
    QSqlQuery query;
    int fields, i;
    QString s1;

    query.exec(cmd);
    if(query.first() == false) return false;

    fields = query.record().count();

    do {
        s1 = "";
        for(i=0; i<fields; i++) {
            s1 += query.value(i).toString();
            s1 += " ";
        }
        result << s1;
    } while(query.next());

    return true;
}

void Singleton::db_vacuum()
{
    QSqlQuery query;

    query.exec("VACUUM");
}

void Singleton::db_import_device(QString rxr, QString nhs, QString start, QString finish, QString mode, QString serial, QString model)
{
    QSqlQuery query;
    int device_no;

    if(serial == "") {
        // Just add deployed_dev
        query.prepare("INSERT INTO deployed_dev (pat_id, issue_date, withdraw_date, dev_mode) "
                      "VALUES (:pat, :issue, :withdraw, :mode)");
        query.bindValue(":rxr", db_get_or_add_rxr(rxr, nhs));
        query.bindValue(":issue", start);
        query.bindValue(":withdraw", finish);
        query.bindValue(":mode", mode);
        query.exec();
        return;
    }

    device_no = db_get_device(serial);
    if(device_no == -1) {
        query.prepare("INSERT INTO device (manufacturer, model, dev_type, serial_no) "
                      "VALUES (:company, :model, :machine, :serial)");
        query.bindValue(":company", (model == "A40") ? "Philips Respironics" : "ResMed");
        query.bindValue(":model", model);
        query.bindValue(":machine", "NIV");
        query.bindValue(":serial", serial);

        query.exec();
        query.exec("SELECT id FROM device WHERE ROWID = (SELECT last_insert_rowid())");
        query.first();
        device_no = query.value(0).toInt();
    }

    query.prepare("INSERT INTO deployed_dev (pat_id, dev_id, issue_date, withdraw_date, dev_mode) "
                  "VALUES (:pat, :dev, :issue, :withdraw, :mode)");
    query.bindValue(":rxr", db_get_or_add_rxr(rxr, nhs));
    query.bindValue(":dev", device_no);
    query.bindValue(":issue", start);
    query.bindValue(":withdraw", finish);
    query.bindValue(":mode", mode);
    query.exec();
}

int Singleton::db_get_device(QString serial)
{
    QSqlQuery query;

    query.prepare("SELECT id FROM device WHERE serial_no = :serial");
    query.bindValue(":serial", serial);
    query.exec();
    if(query.first() == false) return -1;
    return query.value(0).toInt();
}

bool Singleton::db_pat_exists(QString rxr)
{
    if(!valid_rxr(rxr)) return false;
    if(db_get_rxr(rxr) == -1) return false;

    return true;
}

QString Singleton::db_import_cpap(QString rxr, QString model, QString serial, QString mode, QString start, QString finish, QString make)
{
    QSqlQuery query;
    int device_no;

    device_no = db_get_device(serial);
    if(device_no == -1) {
        query.prepare("INSERT INTO device (manufacturer, model, dev_type, serial_no) "
                      "VALUES (:company, :model, :machine, :serial)");
        query.bindValue(":company", make);
        query.bindValue(":model", model);
        query.bindValue(":machine", "CPAP");
        query.bindValue(":serial", serial);

        query.exec();
        query.exec("SELECT id FROM device WHERE ROWID = (SELECT last_insert_rowid())");
        query.first();
        device_no = query.value(0).toInt();
    }

    query.prepare("INSERT INTO deployed_dev (pat_id, dev_id, issue_date, withdraw_date, dev_mode) "
                  "VALUES (:pat, :dev, :issue, :withdraw, :mode)");
    query.bindValue(":rxr", db_get_or_add_rxr(rxr));
    query.bindValue(":dev", device_no);
    query.bindValue(":issue", start);
    query.bindValue(":withdraw", finish);
    query.bindValue(":mode", mode);
    query.exec();

    return (rxr + "," + serial + "," + start + "," + finish);
}

QString Singleton::db_import_ixtrak02(QString line)
{
    QSqlQuery query; // necessary?
    QStringList vals;
    int patid, interact_id;

    vals = line.split(',');

    if(vals[0] == "RXR") return "Ignoring header line";
    if(!valid_rxr(vals[0])) return "Invalid RXR \"" + vals[0] + "\"";

    patid = db_get_or_add_rxr(vals[0], vals[1]);
    // create interaction, then add each investigation etc
    interact_id = db_insert_interaction(vals[12], patid,
                                        db_lookup_or_add("contact_types", vals[2]),
                                        db_lookup_or_add("disp_types", vals[6]),
                                        (vals[10] == "TRUE"),
                                        (vals[8] == "TRUE"),
                                        vals[11] == "FALSE" ? "me" : "junior NOS", // Plus call to db_get_clinician_id()
                                        vals[3],
                                        vals[7].toInt(),
                                        vals[9]);

}

void Singleton::populate_clinician_box(QComboBox *box)
{
    QList<Clinician>::iterator i;

    for(i = clinicians.begin(); i < clinicians.end(); i++) box->addItem(i->name);
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

void Singleton::set_primary_clinician(QString s)
{
    temp_s = s;
    //qDebug() << "set_primary_clinician() Primary clinician: " << temp_s;
}

void Singleton::load_primary_clinician()
{
    primary_clinican.name = temp_s;
    primary_clinican.grade = db_lookup_or_add("clin_grades", "Consultant");
    primary_clinican.id = db_get_clinician_id(temp_s);
    //qDebug() << "load_primary_clinician() Primary clinician: " << primary_clinican.name << " grade: " << primary_clinican.grade << " id: " << primary_clinican.id;
}

int Singleton::get_primary_clinician_id()
{
    return primary_clinican.id;
}
