#ifndef SINGLETON_H
#define SINGLETON_H

#include "physiology.h"
#include "ixtrak_options.h"

#include <QString>
#include <QSqlDatabase>
#include <QList>
#include <QComboBox>

// Standalone functions
bool valid_rxr(QString rxr);

class Singleton
{
public:
    Singleton();
    ~Singleton();

    void set_physiology_limit(QString s, double min, double max);
    double validate_physiology(QString s, double val);

    void set_db_path(QString s);
    void log_db_entry(QString s);
    QString get_logs();

    void set_data_option(Options opt, QString db, QString label);
    void get_options(Options opt, QList<IxTrakOption> &list);
    void set_primary_clinician(QString s);
    int get_primary_clinician_id();

    bool db_init_connection();
    void db_insert_preclinic(QString rxr, QString nhs, QString test);
    int db_lookup_or_add(QString table, QString val);
    void db_insert_physiology(QString rxr, QString type, double val, double high=0.0, double low=0.0);
    QPair<QPair<int, int>, QString> db_insert_preclinic_interaction(QString rxr, QString nhs);
    void db_insert_preclinic_ix(QPair<QPair<int, int>, QString> details, QString test);
    int db_insert_interaction(QString date, int patid, int inttype, int disp, bool for_radiology, bool admit, int seenby, QString diagnosis, int follow_up, QString notes);
    void db_insert_investigation(int patid, int interactid, int invid, QString date, QString notes="NULL");
    void db_insert_referral(int patid, int interactid, QString speciality, QString date);
    int db_get_or_add_rxr(QString rxr, QString nhs="NULL");
    int db_get_clinician_id(QString name);
    void db_get_active_clinicians();
    bool db_run_sql(QString cmd, QStringList &result);
    void db_vacuum();
    void db_import_device(QString rxr, QString nhs, QString start, QString finish,
                          QString mode, QString serial, QString model);
    int db_get_device(QString serial);
    bool db_pat_exists(QString rxr);
    QString db_import_cpap(QString rxr, QString model, QString serial, QString mode, QString start, QString finish, QString make);
    QString db_import_ixtrak02(QString line);

    void populate_clinician_box(QComboBox *box);


private:
    physiology physiology_limits;
    QString db_path = "ixtrak.sqlite";
    QSqlDatabase db;
    QList<QString> entered;
    QList<ShortOption> dataOptions;
    QList<Clinician> clinicians;
    Clinician primary_clinican;

    QString temp_s;     // Hold primary clinican until database connection established

    int db_get_rxr(QString rxr);
    void db_insert_rxr(QString rxr, QString nhs="NULL");
    void load_primary_clinician();
};

#ifndef MAIN_CPP
extern Singleton *IxTrak;
#endif // MAIN_CPP

#endif // SINGLETON_H
