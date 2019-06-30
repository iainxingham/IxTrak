#include "physiology.h"

#include <QMessageBox>
#include <QString>

physiology::physiology()
{

}

void physiology::setlimit(QString s, double min, double max)
{
    MinMax m;

    m.min = min;
    m.max = max;
    limits.insert(s, m);
}

double physiology::validate(QString s, double val)
{
    if(!limits.contains(s)) {
        QMessageBox msg;
        msg.setWindowTitle("Error");
        msg.setText("Physiological parameter not in lookup table");
        msg.setIcon(QMessageBox::Critical);
        msg.exec();
        return val;
    }

/*    if(s == "FeNO") {
        QString m = "FeNo, min %1, max %2";
        m = m.arg(QString::number(limits[s].min), QString::number(limits[s].max));
        QMessageBox msg;
        msg.setWindowTitle("Debug");
        msg.setText(m);
        msg.setIcon(QMessageBox::Warning);
        msg.exec();
    }*/

    if(val < limits[s].min) return -1.0;
    if(val > limits[s].max) return -1.0;

    return val;
}
