#ifndef PHYSIOLOGY_H
#define PHYSIOLOGY_H

#include <QMap>
#include <QString>

struct MinMax
{
    double min;
    double max;
};

class physiology
{

private:
    QMap<QString, MinMax> limits;

public:
    physiology();
    void setlimit(QString s, double min, double max);
    double validate(QString s, double val);
};

#endif // PHYSIOLOGY_H
