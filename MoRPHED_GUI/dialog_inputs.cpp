#include "dialog_inputs.h"
#include "ui_dialog_inputs.h"

dialog_inputs::dialog_inputs(XMLReadWrite &XmlObj, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialog_inputs)
{
    XmlDoc = XmlObj;

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

void dialog_inputs::readXml()
{
    QString nodeData;
    nodeData = XmlDoc.readNodeData("Inputs", "DEMPath");

    if (!nodeData.isNull() || !nodeData.isEmpty())
    {
        qsDem = nodeData;
        ui->line_dem->setText(qsDem);
    }

    nodeData = XmlDoc.readNodeData("Inputs", "HydroSediPath");

    if (!nodeData.isNull() || !nodeData.isEmpty())
    {
        qsHydroSedi = nodeData;
        ui->line_dem->setText(qsHydroSedi);
    }

    nodeData = XmlDoc.readNodeData("Inputs", "ImportType");

    if (!nodeData.isNull() || !nodeData.isEmpty())
    {
        nImportType = nodeData.toInt();

        if (nImportType == 1)
        {
            ui->rbtn_volume->setChecked(true);
            ui->rbtn_proportion->setChecked(false);
        }
        else if (nImportType == 2)
        {
            ui->rbtn_proportion->setChecked(true);
            ui->rbtn_volume->setChecked(false);
        }
    }
}
