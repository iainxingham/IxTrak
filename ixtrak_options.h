#ifndef IXTRAK_OPTIONS_H
#define IXTRAK_OPTIONS_H

#include <QRadioButton>
#include <QCheckBox>
#include <QString>

struct IxTrakOption {
    QString dbName;
    int dbNum;
    QString label;
    QRadioButton *radio;
    QCheckBox *check;

    //IxTrakOption (const IxTrakOption &) = default;
};

enum class Options {
    Contact,
    Diagnosis,
    Investigation,
    Referral,
    Disposal
};

struct ShortOption {
    QString dbName;
    QString label;
    Options opt;

    //ShortOption (const ShortOption &) = default;
};

struct Clinician {
    QString name;
    int grade;
    int id;

    //Clinician (const Clinician &) = default;
};

QString optionToString(Options o);

#endif // IXTRAK_OPTIONS_H
