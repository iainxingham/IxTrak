#include "physiology.h"

#include <QMessageBox>

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
    if(val < limits[s].min) return -1.0;
    if(val > limits[s].max) return -1.0;

    return val;
}
