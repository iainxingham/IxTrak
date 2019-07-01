#ifndef DATA_ENTRY_H
#define DATA_ENTRY_H

#include <QDialog>
#include <QGroupBox>

#include "ixtrak_options.h"

namespace Ui {
class data_entry;
}

class data_entry : public QDialog
{
    Q_OBJECT

public:
    explicit data_entry(QWidget *parent = nullptr);
    ~data_entry();

private:
    Ui::data_entry *ui;

    QList<IxTrakOption> contacts;
    QList<IxTrakOption> diagnoses;
    QList<IxTrakOption> investigations;
    QList<IxTrakOption> referrals;
    QList<IxTrakOption> disposals;

    void update_entry_log();
    void set_option_pointers(QList<IxTrakOption> &list, bool radio, QGroupBox *box);

};

#endif // DATA_ENTRY_H
