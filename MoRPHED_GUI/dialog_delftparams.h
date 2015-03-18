#ifndef DIALOG_DELFTPARAMS_H
#define DIALOG_DELFTPARAMS_H

#include <QDialog>

namespace Ui {
class dialog_delftParams;
}

class dialog_delftParams : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_delftParams(QWidget *parent = 0);
    ~dialog_delftParams();

private slots:
    void on_btn_cancel_clicked();

    void on_btn_ok_clicked();

private:
    Ui::dialog_delftParams *ui;
};

#endif // DIALOG_DELFTPARAMS_H
