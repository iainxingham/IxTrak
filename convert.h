#ifndef CONVERT_H
#define CONVERT_H

#include <QDialog>

namespace Ui {
class convert;
}

class convert : public QDialog
{
    Q_OBJECT

public:
    explicit convert(QWidget *parent = nullptr);
    ~convert();

private slots:
    void on_convertButton_clicked();

private:
    Ui::convert *ui;
};

#endif // CONVERT_H
