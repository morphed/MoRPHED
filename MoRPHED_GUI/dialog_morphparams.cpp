#include "dialog_morphparams.h"
#include "ui_dialog_morphparams.h"

dialog_morphParams::dialog_morphParams(XMLReadWrite &XmlObj, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialog_morphParams)
{
    ui->setupUi(this);

    XmlDoc = XmlObj;
    closeOk = true;
    readXml();

    setupPlots();
}

dialog_morphParams::~dialog_morphParams()
{
    delete ui;
}

void dialog_morphParams::setupDisplay()
{
    ui->rbtn_custom1->setEnabled(false);
    ui->rbtn_custom2->setEnabled(false);
    ui->spinDbl_a1->setEnabled(false);
    ui->spinDbl_a2->setEnabled(false);
    ui->spinDbl_b1->setEnabled(false);
    ui->spinDbl_b2->setEnabled(false);
    ui->spinDbl_mu1->setEnabled(false);
    ui->spinDbl_mu2->setEnabled(false);
    ui->spinDbl_sigma1->setEnabled(false);
    ui->spinDbl_sigma2->setEnabled(false);
}


void dialog_morphParams::setupPlots()
{
    ui->plot_pl1->addGraph();
    ui->plot_pl1->xAxis->setLabel("Distance from Sediment Origin (m)");
    ui->plot_pl1->yAxis->setLabel("Proportion of Sediment Deposited");
    ui->plot_pl1->yAxis->setRange(0, 1);
    ui->plot_pl1->xAxis->setRange(0, 10);
    ui->plot_pl1->xAxis->setAutoTickStep(false);
    ui->plot_pl1->xAxis->setTickStep(1);
    ui->plot_pl1->yAxis->setAutoTickStep(false);
    ui->plot_pl1->yAxis->setAutoSubTicks(false);
    ui->plot_pl1->yAxis->setTickStep(0.1);
    ui->plot_pl1->yAxis->setSubTickCount(1);

    ui->plot_pl2->addGraph();
    ui->plot_pl2->xAxis->setLabel("Distance from Sediment Origin (m)");
    ui->plot_pl2->yAxis->setLabel("Proportion of Sediment Deposited");
    ui->plot_pl2->yAxis->setRange(0, 1);
    ui->plot_pl2->xAxis->setRange(0, 10);
    ui->plot_pl2->xAxis->setAutoTickStep(false);
    ui->plot_pl2->xAxis->setTickStep(1);
    ui->plot_pl2->yAxis->setAutoTickStep(false);
    ui->plot_pl2->yAxis->setAutoSubTicks(false);
    ui->plot_pl2->yAxis->setTickStep(0.1);
    ui->plot_pl2->yAxis->setSubTickCount(1);
}

void dialog_morphParams::setValues()
{
    closeOk = true;
    plotOk = true;

    length1 = ui->spinDbl_length1->value();
    length2 = ui->spinDbl_length2->value();
    nType1 = getType1();
    nType2 = getType2();
    grainSize = ui->spinDbl_gs->value();
    slpThresh = ui->spinDbl_slp->value();
    shrThresh = ui->spinDbl_shr->value();
    areaThresh = ui->spinDbl_area->value();
    erosionFactor = ui->spinDbl_erosion->value();

    if (nType1 == 1)
    {
        sigA1 = ui->spinDbl_a1->value();
        muB1 = ui->spinDbl_b1->value();
    }
    else if (nType1 == 2)
    {
        sigA1 = ui->spinDbl_sigma1->value();
        muB1 = ui->spinDbl_mu1->value();
    }
    else
    {
        closeOk = false;
        plotOk = false;
        QMessageBox::information(this, "Invalid Path-Length Type", "Please select an option for the Path-Length Distribution");
    }

    if (nType2 == 1)
    {
        sigA2 = ui->spinDbl_a2->value();
        muB2 = ui->spinDbl_b2->value();
    }
    else if (nType2 == 2)
    {
        sigA2 = ui->spinDbl_sigma2->value();
        muB2 = ui->spinDbl_mu2->value();
    }
    else
    {
        closeOk = false;
        plotOk = false;
        QMessageBox::information(this, "Invalid Path-Length Type", "Please select an option for the import Path-Length Distribution");
    }

    if (length1<=0.0 || sigA1<=0.0 || muB1<=0.0)
    {
        closeOk = false;
        plotOk = false;
        QMessageBox::information(this, "Invalid Path-Length Parameters", "Path-Length Distribution parameters must be greater than 0.0");
    }

    if (length2<=0.0 || sigA2<=0.0 || muB2<=0.0)
    {
        closeOk = false;
        plotOk = false;
        QMessageBox::information(this, "Invalid Path-Length Parameters", "Import Path-Length Distribution parameters must be greater than 0.0");
    }

    if (areaThresh<=0.0 || slpThresh<=0.0|| shrThresh<= 0.0 || erosionFactor<=0.0 || grainSize<=0.0)
    {
        closeOk = false;
        QMessageBox::information(this, "Invalid Erosion Parameters", "Erosion parameters must be greater than 0.0");
    }
}

void dialog_morphParams::updatePlots()
{
    setValues();
    x1.clear(), x2.clear(), y1.clear(), y2.clear();
    int nLength = length1 / nCellWidth;

    y1 = MORPH_PathLengthDist::getDistribution(nType1, sigA1, muB1, nLength);
    for (int i=0; i<nLength; i++)
    {
        x1.append((i+1)*nCellWidth);
    }

    nLength = length2 / nCellWidth;

    y2 = MORPH_PathLengthDist::getDistribution(nType2, sigA2, muB2, nLength);
    for (int i=0; i<nLength; i++)
    {
        x2.append((i+1)*nCellWidth);
    }

    QPen pen;
    pen.setColor(Qt::green);
    pen.setWidth(2);

    //setup graph styling
    ui->plot_pl1->graph(0)->setPen(pen);
    ui->plot_pl1->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDot));
    ui->plot_pl1->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->plot_pl2->graph(0)->setPen(pen);
    ui->plot_pl2->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDot));
    ui->plot_pl2->graph(0)->setLineStyle(QCPGraph::lsLine);

    //set graph data
    ui->plot_pl1->graph(0)->setData(x1, y1);
    ui->plot_pl2->graph(0)->setData(x2, y2);

    //setup axes ranges
    double max = MORPH_Base::findMaxVector(x1);
    ui->plot_pl1->yAxis->setAutoTickStep(true);
    ui->plot_pl1->yAxis->setSubTickCount(1);
    ui->plot_pl1->xAxis->setAutoTickStep(false);
    ui->plot_pl1->xAxis->setRange(0, max);
    ui->plot_pl1->xAxis->setTickStep(max/10.0);
    max = MORPH_Base::findMaxVector(y1);
    ui->plot_pl1->yAxis->setRange(0, max+(max*0.05));
    max = MORPH_Base::findMaxVector(x2);
    ui->plot_pl2->yAxis->setAutoTickStep(true);
    ui->plot_pl2->yAxis->setSubTickCount(1);
    ui->plot_pl2->xAxis->setAutoTickStep(false);
    ui->plot_pl2->xAxis->setRange(0, max);
    ui->plot_pl2->xAxis->setTickStep(max/10.0);
    max = MORPH_Base::findMaxVector(y2);
    ui->plot_pl2->yAxis->setRange(0, max+(max*0.05));

    ui->plot_pl1->replot();
    ui->plot_pl1->update();

    ui->plot_pl2->replot();
    ui->plot_pl2->update();
}

void dialog_morphParams::on_btn_cancel_clicked()
{
    this->close();
}

void dialog_morphParams::on_btn_ok_clicked()
{
    setValues();
    writeXml();
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
    length1 = XmlDoc.readNodeData("MorphedParameters", "PathLength1", "DistributionLength").toDouble();
    nType1 = XmlDoc.readNodeData("MorphedParameters", "PathLength1", "DistributionType").toInt();
    sigA1 = XmlDoc.readNodeData("MorphedParameters", "PathLength1", "SigA").toDouble();
    muB1 = XmlDoc.readNodeData("MorphedParameters", "PathLength1", "MuB").toDouble();

    length2 = XmlDoc.readNodeData("MorphedParameters", "PathLengthImport", "DistributionLength").toDouble();
    nType2 = XmlDoc.readNodeData("MorphedParameters", "PathLengthImport", "DistributionType").toInt();
    sigA2 = XmlDoc.readNodeData("MorphedParameters", "PathLengthImport", "SigA").toDouble();
    muB2 = XmlDoc.readNodeData("MorphedParameters", "PathLengthImport", "MuB").toDouble();

    shrThresh = XmlDoc.readNodeData("MorphedParameters", "BankShearThresh").toDouble();
    slpThresh = XmlDoc.readNodeData("MorphedParameters", "BankSlopeThresh").toDouble();
    areaThresh = XmlDoc.readNodeData("MorphedParameters", "BankAreaThresh").toDouble();
    erosionFactor = XmlDoc.readNodeData("MorphedParameters", "ErosionFactor").toDouble();
    grainSize = XmlDoc.readNodeData("MorphedParameters", "GrainSize").toDouble();
    nCellWidth = XmlDoc.readNodeData("DEMProperties", "CellWidth").toDouble();

    ui->spinDbl_length1->setValue(length1);
    ui->spinDbl_length2->setValue(length2);
    ui->spinDbl_slp->setValue(slpThresh);
    ui->spinDbl_shr->setValue(shrThresh);
    ui->spinDbl_gs->setValue(grainSize);
    ui->spinDbl_area->setValue(areaThresh);
    ui->spinDbl_erosion->setValue(erosionFactor);

    if (nType1 == 1)
    {
        ui->rbtn_exp1->setChecked(true);
        ui->spinDbl_a1->setValue(sigA1);
        ui->spinDbl_b1->setValue(muB1);
    }
    else if (nType1 == 2)
    {
        ui->rbtn_gaus1->setChecked(true);
        ui->spinDbl_mu1->setValue(muB1);
        ui->spinDbl_sigma1->setValue(sigA1);
    }

    if (nType2 == 1)
    {
        ui->rbtn_exp2->setChecked(true);
        ui->spinDbl_a2->setValue(sigA2);
        ui->spinDbl_b2->setValue(muB2);
    }
    else if (nType2 == 2)
    {
        ui->rbtn_gaus2->setChecked(true);
        ui->spinDbl_mu2->setValue(muB2);
        ui->spinDbl_sigma2->setValue(sigA2);
    }
}

void dialog_morphParams::writeXml()
{
    XmlDoc.writeNodeData("MorphedParameters", "BankShearThresh", QString::number(shrThresh));
    XmlDoc.writeNodeData("MorphedParameters", "BankSlopeThresh", QString::number(slpThresh));
    XmlDoc.writeNodeData("MorphedParameters", "BankAreaThresh", QString::number(areaThresh));
    XmlDoc.writeNodeData("MorphedParameters", "ErosionFactor", QString::number(erosionFactor));
    XmlDoc.writeNodeData("MorphedParameters", "GrainSize", QString::number(grainSize));
    XmlDoc.writeNodeData("MorphedParameters", "PathLength1", "DistributionLength", QString::number(length1));
    XmlDoc.writeNodeData("MorphedParameters", "PathLength1", "DistributionType", QString::number(nType1));
    XmlDoc.writeNodeData("MorphedParameters", "PathLength1", "SigA", QString::number(sigA1));
    XmlDoc.writeNodeData("MorphedParameters", "PathLength1", "MuB", QString::number(muB1));
    XmlDoc.writeNodeData("MorphedParameters", "PathLengthImport", "DistributionLength", QString::number(length2));
    XmlDoc.writeNodeData("MorphedParameters", "PathLengthImport", "DistributionType", QString::number(nType2));
    XmlDoc.writeNodeData("MorphedParameters", "PathLengthImport", "SigA", QString::number(sigA2));
    XmlDoc.writeNodeData("MorphedParameters", "PathLengthImport", "MuB", QString::number(muB2));

    XmlDoc.printXML();
}

void dialog_morphParams::on_btn_update_clicked()
{
    updatePlots();
}

void dialog_morphParams::on_rbtn_exp2_toggled(bool checked)
{
    if (checked)
    {
        ui->spinDbl_a2->setEnabled(true);
        ui->spinDbl_b2->setEnabled(true);
        ui->spinDbl_mu2->setEnabled(false);
        ui->spinDbl_sigma2->setEnabled(false);
    }
}

void dialog_morphParams::on_rbtn_exp1_toggled(bool checked)
{
    if (checked)
    {
        ui->spinDbl_a1->setEnabled(true);
        ui->spinDbl_b1->setEnabled(true);
        ui->spinDbl_mu1->setEnabled(false);
        ui->spinDbl_sigma1->setEnabled(false);
    }
}

void dialog_morphParams::on_rbtn_gaus1_toggled(bool checked)
{
    if (checked)
    {
        ui->spinDbl_mu1->setEnabled(true);
        ui->spinDbl_sigma1->setEnabled(true);
        ui->spinDbl_a1->setEnabled(false);
        ui->spinDbl_b1->setEnabled(false);
    }
}

void dialog_morphParams::on_rbtn_gaus2_toggled(bool checked)
{
    if (checked)
    {
        ui->spinDbl_mu2->setEnabled(true);
        ui->spinDbl_sigma2->setEnabled(true);
        ui->spinDbl_a2->setEnabled(false);
        ui->spinDbl_b2->setEnabled(false);
    }
}
