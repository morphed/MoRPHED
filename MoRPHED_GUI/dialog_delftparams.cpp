#include "dialog_delftparams.h"
#include "ui_dialog_delftparams.h"

dialog_delftParams::dialog_delftParams(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialog_delftParams)
{
    ui->setupUi(this);
}

dialog_delftParams::~dialog_delftParams()
{
    delete ui;
}

void dialog_delftParams::on_btn_cancel_clicked()
{
    this->close();
}

void dialog_delftParams::on_btn_ok_clicked()
{
    this->close();
}
