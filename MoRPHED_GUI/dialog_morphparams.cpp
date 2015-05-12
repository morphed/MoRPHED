#include "dialog_morphparams.h"
#include "ui_dialog_morphparams.h"

dialog_morphParams::dialog_morphParams(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialog_morphParams)
{
    ui->setupUi(this);

    closeOk = true;
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

int dialog_morphParams::getType1()
{
    if (ui->rbtn_exp1->isChecked())
    {
        return 1;
    }
    else if (ui->rbtn_gaus1->isChecked())
    {
        return 2;
    }
    else if (ui->rbtn_custom1->isChecked())
    {
        return 3;
    }
    else
    {
        return 0;
    }
}

int dialog_morphParams::getType2()
{
    if (ui->rbtn_exp2->isChecked())
    {
        return 1;
    }
    else if (ui->rbtn_gaus2->isChecked())
    {
        return 2;
    }
    else if (ui->rbtn_custom2->isChecked())
    {
        return 3;
    }
    else
    {
        return 0;
    }
}

void dialog_morphParams::readXml()
{

}

void dialog_morphParams::writeXml()
{
    length1 = ui->spinDbl_length1->value();
    length2 = ui->spinDbl_length2->value();
    nType1 = getType1();
    nType2 = getType2();
}
