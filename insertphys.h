#ifndef INSERTPHYS_H
#define INSERTPHYS_H

#include <QDialog>

namespace Ui {
class insertphys;
}

class insertphys : public QDialog
{
    Q_OBJECT

public:
    explicit insertphys(QWidget *parent = nullptr);
    ~insertphys();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::insertphys *ui;
};

#endif // INSERTPHYS_H
