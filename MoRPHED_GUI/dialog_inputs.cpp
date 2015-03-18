#include "dialog_inputs.h"
#include "ui_dialog_inputs.h"

dialog_inputs::dialog_inputs(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialog_inputs)
{
    ui->setupUi(this);
}

dialog_inputs::~dialog_inputs()
{
    delete ui;
}

void dialog_inputs::on_btn_cancel_clicked()
{
    this->close();
}

void dialog_inputs::on_btn_ok_clicked()
{
    this->close();
}
