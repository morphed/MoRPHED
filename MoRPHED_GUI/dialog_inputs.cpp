#include "dialog_inputs.h"
#include "ui_dialog_inputs.h"

dialog_inputs::dialog_inputs(XMLReadWrite &XmlObj, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialog_inputs)
{
    ui->setupUi(this);

    XmlDoc = XmlObj;
    closeOk = true;

    readXml();
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
    writeXml();

    checkClose();
}

void dialog_inputs::checkClose()
{
    if (closeOk)
    {
        this->close();
    }
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
        ui->line_input->setText(qsHydroSedi);
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

void dialog_inputs::writeXml()
{
    closeOk = true;

    if (ui->rbtn_volume->isChecked())
    {
        nImportType = 1;
    }
    else if (ui->rbtn_proportion->isChecked())
    {
        nImportType = 2;
    }
    else
    {
        nImportType = 0;
        closeOk = false;
        QMessageBox::information(this, "Invalid Import Type", "Please select an option for importing sediment");
    }

    if (qsDem.isNull() || qsDem.isEmpty())
    {
        closeOk = false;
        QMessageBox::information(this, "Invalid DEM path", "Please enter a valid DEM path");
    }

    if (qsHydroSedi.isNull() || qsHydroSedi.isEmpty())
    {
        closeOk = false;
        QMessageBox::information(this, "Invalid Input path", "Please enter a valid Input path");
    }

    XmlDoc.writeNodeData("Inputs", "DEMPath", qsDem);
    XmlDoc.writeNodeData("Inputs", "HydroSediPath", qsHydroSedi);
    XmlDoc.writeNodeData("Inputs", "ImportType", QString::number(nImportType));

    XmlDoc.printXML();
}

void dialog_inputs::on_line_dem_textChanged(const QString &arg1)
{
    qsDem = arg1;
}

void dialog_inputs::on_line_input_textEdited(const QString &arg1)
{
    qsHydroSedi = arg1;
}

void dialog_inputs::on_btn_graph_clicked()
{

}

void dialog_inputs::on_tbtn_dem_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select DEM");

    if (!filename.isNull() || !filename.isEmpty())
    {
        QFile file(filename);

        if (file.exists())
        {
            QFileInfo fi(file);
            qsDem = filename;
            ui->line_dem->setText(qsDem);
        }
    }
}

void dialog_inputs::on_tbtn_input_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select DEM");

    if (!filename.isNull() || !filename.isEmpty())
    {
        QFile file(filename);

        if (file.exists())
        {
            QFileInfo fi(file);
            qsHydroSedi = filename;
            ui->line_input->setText(qsHydroSedi);
        }
    }
}
