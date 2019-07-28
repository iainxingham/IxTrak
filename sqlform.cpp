#include "sqlform.h"
#include "ui_sqlform.h"

#include "singleton.h"

SQLform::SQLform(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SQLform)
{
    ui->setupUi(this);
}

SQLform::~SQLform()
{
    delete ui;
}

/* This would obviously be insane in anything other than a personal tool
 * The database is exposed to unsanitised user input
 */
void SQLform::on_pushButton_clicked()
{
    QStringList result;
    bool success;

    success = IxTrak->db_run_sql(ui->inputText->toPlainText(), result);
    ui->outputText->clear();

    if(success) {
        for(int i=0; i<result.size(); i++) ui->outputText->append(result.at(i));
    }
    else ui->outputText->append("Invalid SQL or query returned no results");

}
