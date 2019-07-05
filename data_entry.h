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

private slots:
    void on_pushButton_clicked();

    void on_clearButton_clicked();

private:
    Ui::data_entry *ui;

    QList<IxTrakOption> contacts;
    QList<IxTrakOption> diagnoses;
    QList<IxTrakOption> investigations;
    QList<IxTrakOption> referrals;
    QList<IxTrakOption> disposals;

    void update_entry_log();
    void set_option_pointers(QList<IxTrakOption> &list, bool radio, QGroupBox *box);
    void clear_input_form();
    void save_entered_data();
    int get_dbentry_from_list(QList<IxTrakOption> &list);
    QString get_name_from_list(QList<IxTrakOption> &list);

};

#endif // DATA_ENTRY_H
