#include "dialog_morphparams.h"
#include "ui_dialog_morphparams.h"

dialog_morphParams::dialog_morphParams(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialog_morphParams)
{
    ui->setupUi(this);
}

dialog_morphParams::~dialog_morphParams()
{
    delete ui;
}

void dialog_morphParams::on_btn_cancel_clicked()
{
    this->close();
}

void dialog_morphParams::on_btn_ok_clicked()
{
    checkClose();
}

void dialog_morphParams::checkClose()
{
    if (closeOk)
    {
        this->close();
    }
}
