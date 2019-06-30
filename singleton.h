#ifndef SINGLETON_H
#define SINGLETON_H

#include "physiology.h"

#include <QString>
#include <QSqlDatabase>

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

    bool db_init_connection();
    void db_insert_preclinic(QString rxr, QString nhs, QString test);
    int db_lookup_or_add(QString table, QString val);
    void db_insert_physiology(QString rxr, QString type, double val, double high=0.0, double low=0.0);

private:
    physiology physiology_limits;
    QString db_path = "ixtrak.sqlite";
    QSqlDatabase db;

    int db_get_rxr(QString rxr);
    void db_insert_rxr(QString rxr, QString nhs="NULL");
};

#ifndef MAIN_CPP
extern Singleton *IxTrak;
#endif // MAIN_CPP

#endif // SINGLETON_H
