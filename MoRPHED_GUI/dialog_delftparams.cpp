#include "dialog_delftparams.h"
#include "ui_dialog_delftparams.h"

dialog_delftParams::dialog_delftParams(XMLReadWrite &XmlObj, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialog_delftParams)
{
    ui->setupUi(this);

    XmlDoc = XmlObj;

    closeOk = true;
    setInitialValues();
    readXml();
}

dialog_delftParams::~dialog_delftParams()
{
    delete ui;
}

bool dialog_delftParams::getCloseOk()
{
    return closeOk;
}

void dialog_delftParams::on_btn_cancel_clicked()
{
    this->close();
}

void dialog_delftParams::on_btn_ok_clicked()
{
    writeXml();

    checkClose();
}

void dialog_delftParams::checkClose()
{
    if (closeOk)
    {
        this->close();
    }
}

int dialog_delftParams::getDsBound()
{
    if (ui->rbtn_dsNorth->isChecked())
    {
        return 1;
    }
    else if (ui->rbtn_dsSouth->isChecked())
    {
        return 2;
    }
    else if (ui->rbtn_dsEast->isChecked())
    {
        return 3;
    }
    else if (ui->rbtn_dsWest->isChecked())
    {
        return 4;
    }
    else
    {
        return 0;
    }
}

int dialog_delftParams::getUsBound()
{
    if (ui->rbtn_usNorth->isChecked())
    {
        return 1;
    }
    else if (ui->rbtn_usSouth->isChecked())
    {
        return 2;
    }
    else if (ui->rbtn_usEast->isChecked())
    {
        return 3;
    }
    else if (ui->rbtn_usWest->isChecked())
    {
        return 4;
    }
    else
    {
        return 0;
    }
}

void dialog_delftParams::readXml()
{
    QString nodeData;

    nodeData = XmlDoc.readNodeData("Delft3DParameters", "DelftPath");
    qsDelftPath = nodeData;
    nodeData = XmlDoc.readNodeData("Delft3DParameters", "USBoundLocation");
    nUsBound = nodeData.toInt();
    nodeData = XmlDoc.readNodeData("Delft3DParameters", "DSBoundLocation");
    nDsBound = nodeData.toInt();
    nodeData = XmlDoc.readNodeData("Delft3DParameters", "SimTime");
    simTime = nodeData.toDouble();
    nodeData = XmlDoc.readNodeData("Delft3DParameters", "TimeStep");
    timeStep = nodeData.toDouble();
    nodeData = XmlDoc.readNodeData("Delft3DParameters", "Roughness");
    roughness = nodeData.toDouble();
    nodeData = XmlDoc.readNodeData("Delft3DParameters", "HEV");
    hev = nodeData.toDouble();

    ui->line_delft->setText(qsDelftPath);
}

void dialog_delftParams::setDsBound(int value)
{
    if (value == 1)
    {
        ui->rbtn_dsNorth->setChecked(true);
    }
    else if (value == 2)
    {
        ui->rbtn_dsSouth->setChecked(true);
    }
    else if (value == 3)
    {
        ui->rbtn_dsEast->setChecked(true);
    }
    else if (value == 4)
    {
        ui->rbtn_dsWest->setChecked(true);
    }
}

void dialog_delftParams::setInitialValues()
{
    hev = 0.01;
    simTime = 60.0;
    timeStep = 0.025;
    roughness = 0.1;

    ui->dblSpin_hev->setValue(hev);
    ui->dblSpin_rough->setValue(roughness);
    ui->dblSpin_simTime->setValue(simTime);
    ui->dblSpin_timeStep->setValue(timeStep);
}

void dialog_delftParams::setUsBound(int value)
{
    if (value == 1)
    {
        ui->rbtn_usNorth->setChecked(true);
    }
    else if (value == 2)
    {
        ui->rbtn_usSouth->setChecked(true);
    }
    else if (value == 3)
    {
        ui->rbtn_usEast->setChecked(true);
    }
    else if (value == 4)
    {
        ui->rbtn_usWest->setChecked(true);
    }
}

void dialog_delftParams::writeXml()
{
    closeOk = true;

    nUsBound = getUsBound();
    nDsBound = getDsBound();

    hev = ui->dblSpin_hev->value();
    simTime = ui->dblSpin_simTime->value();
    timeStep = ui->dblSpin_timeStep->value();
    roughness = ui->dblSpin_rough->value();

    if (nDsBound == 0)
    {
        closeOk = false;
        QMessageBox::information(this, "Invalid Boundary", "Select a downstream boundary location");
    }

    if (nUsBound == 0)
    {
        closeOk = false;
        QMessageBox::information(this, "Invalid Boundary", "Select a upstream boundary location");
    }

    if (hev<=0.0 || simTime<=0.0 || timeStep<=0.0 || roughness<=0.0)
    {
        closeOk = false;
        QMessageBox::information(this, "Invalid Parameter", "All parameter values must be greater than zero");
    }

    if (qsDelftPath.isNull() || qsDelftPath.isEmpty())
    {
        closeOk = false;
        QMessageBox::information(this, "Invalid Delft3D Path", "The Delft3D path is empty or null, please enter a valid path");
    }

    XmlDoc.writeNodeData("Delft3DParameters", "DelftPath", qsDelftPath);
    XmlDoc.writeNodeData("Delft3DParameters", "USBoundLocation", QString::number(nUsBound));
    XmlDoc.writeNodeData("Delft3DParameters", "DSBoundLocation", QString::number(nDsBound));
    XmlDoc.writeNodeData("Delft3DParameters", "SimTime", QString::number(simTime));
    XmlDoc.writeNodeData("Delft3DParameters", "TimeStep", QString::number(timeStep));
    XmlDoc.writeNodeData("Delft3DParameters", "Roughness", QString::number(roughness));
    XmlDoc.writeNodeData("Delft3DParameters", "HEV", QString::number(hev));

    XmlDoc.printXML();
}

void dialog_delftParams::on_tbtn_dem_clicked()
{
    QString filename = QFileDialog::getExistingDirectory(this, "Select Delft3D directory");
    qsDelftPath = filename;
    ui->line_delft->setText(qsDelftPath);
}
