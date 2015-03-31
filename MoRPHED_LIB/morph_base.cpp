#include "morph_base.h"

MORPH_Base::MORPH_Base(QString xmlPath)
{
    XmlInit.loadDocument(xmlPath, 1);
    qsXmlPath = xmlPath;
    init();
}

MORPH_Base::~MORPH_Base()
{
    GDALDestroyDriverManager();
}

int MORPH_Base::getCurrentIteration()
{
    return nCurrentIteration;
}

int MORPH_Base::getIterations()
{
    return nIterations;
}

void MORPH_Base::setCurrentIteration(int iter)
{
    nCurrentIteration = iter;
    qsFloodName = MORPH_FileManager::getFloodName(iter+1);
}

void MORPH_Base::init()
{
    loadDrivers();

    qsInputText = XmlInit.readNodeData("Inputs", "HydroSediPath");
    loadInputText();

    qsRootPath = XmlInit.readNodeData("ProjectDirectory");
    qsTempPath = qsRootPath + "/temp";
    qsOldDemPath = qsTempPath + "/olddem.tif";
    qsNewDemPath = qsTempPath + "/newdem.tif";
    qsDelftDemPath = qsTempPath + "/delftdem.tif";

    qsOrigDemPath = XmlInit.readNodeData("Inputs", "DEMPath");
    GDALDataset *pTemp, *pTemp2, *pTemp3, *pTemp4;
    GDALAllRegister();
    pTemp = (GDALDataset*) GDALOpen(qsOrigDemPath.toStdString().c_str(), GA_ReadOnly);
    pTemp->GetGeoTransform(transform);
    nRows = pTemp->GetRasterBand(1)->GetYSize();
    nCols = pTemp->GetRasterBand(1)->GetXSize();
    noData = pTemp->GetRasterBand(1)->GetNoDataValue();

    //create copies of original dataset
    pDriverTIFF = GetGDALDriverManager()->GetDriverByName("GTiff");
    pTemp2 = pDriverTIFF->CreateCopy(qsOldDemPath.toStdString().c_str(), pTemp, FALSE, NULL, NULL, NULL);
    pTemp3 = pDriverTIFF->CreateCopy(qsNewDemPath.toStdString().c_str(), pTemp, FALSE, NULL, NULL, NULL);
    pTemp4 = pDriverTIFF->CreateCopy(qsDelftDemPath.toStdString().c_str(), pTemp, FALSE, NULL, NULL, NULL);
    GDALClose(pTemp);
    GDALClose(pTemp2);
    GDALClose(pTemp3);
    GDALClose(pTemp4);

    cellWidth = transform[1];
    cellHeight = transform[5];
    topLeftX = transform[0];
    topLeftY = transform[3];

    nIterations = XmlInit.readNodeData("Inputs", "ModelIterations").toInt();
    nDirUSbound = XmlInit.readNodeData("Delft3DParameters", "USBoundLocation").toInt();
    nDirDSbound = XmlInit.readNodeData("Delft3DParameters", "DSBoundLocation").toInt();
    nImportType = XmlInit.readNodeData("Inputs", "ImportType").toInt();
    nCurrentIteration = 0;

    nGsType = XmlInit.readNodeData("Inputs", "GrainSize", "Type").toInt();
    nGsActType = XmlInit.readNodeData("Inputs", "GrainSize", "ActiveType").toInt();
    nGsSubType = XmlInit.readNodeData("Inputs", "GrainSize", "SubSurfType").toInt();
    nPlDistType = XmlInit.readNodeData("MorphedParameters", "DistributionType").toInt();
    nDirDSbound = XmlInit.readNodeData("Delft3DParameters", "DSBoundLocation").toInt();
    nDirUSbound = XmlInit.readNodeData("Delft3DParameters", "USBoundLocation").toInt();


    sigA = XmlInit.readNodeData("MorphedParameters", "SigA").toDouble();
    muB = XmlInit.readNodeData("MorphedParameters", "MuB").toDouble();
    bankSlopeThresh = XmlInit.readNodeData("MorphedParameters", "BankSlopeThresh").toDouble();
    bankShearThresh = XmlInit.readNodeData("MorphedParameters", "BankShearThresh").toDouble();
    simTime = XmlInit.readNodeData("Delft3DParameters", "SimTime").toDouble();
    timeStep = XmlInit.readNodeData("Delft3DParameters", "TimeStep").toDouble();
    roughness = XmlInit.readNodeData("Delft3DParameters", "Roughness").toDouble();
    hev = XmlInit.readNodeData("Delft3DParameters", "HEV").toDouble();
    layerThickness = XmlInit.readNodeData("Inputs", "GrainSize", "LayerThickness").toDouble();
    plDistLength = XmlInit.readNodeData("MorphedParameters", "DistributionLength").toDouble();
    gsActiveSize = XmlInit.readNodeData("Inputs", "GrainSize", "ActiveSize").toDouble();


    qsInputPath = qsRootPath + "/Inputs";
    qsOutputPath = qsRootPath + "/Outputs";


    qsGsActPath = XmlInit.readNodeData("Inputs", "GrainSize", "ActiveRasterPath");
    qsGsSubPath = XmlInit.readNodeData("Inputs", "GrainSize", "SubSurfRasterPath");
    qsFloodName = MORPH_FileManager::getFloodName(nCurrentIteration);
}

void MORPH_Base::loadDrivers()
{
    pDriverTIFF = GetGDALDriverManager()->GetDriverByName("GTiff");
}

void MORPH_Base::loadInputText()
{
    date.clear();
    q.clear();
    dswe.clear();
    import.clear();

    //declare temp variables to hold stream data
    QString qsDate, qsQ, qsDSWE, qsImport;
    QDateTime tempDate;
    nIterations = 0;

    //load file
    QFile in(qsInputText);
    if (in.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&in);
        while (!stream.atEnd())
        {
            //read elements from stream to temp variable, convert to double, and store in a QVector
            stream >> qsDate;
            tempDate = QDateTime::fromString("MM/dd/yyyy,hh:mm");
            date.append(tempDate.toTime_t());
            stream >> qsQ;
            q.append(qsQ.toDouble());
            stream >> qsDSWE;
            dswe.append(qsDSWE.toDouble());
            stream >> qsImport;
            import.append(qsImport.toDouble());

            //Each line represents 1 model iteration, increment model iterations after each line is read
            //qDebug()<<date[nIterations]<<" "<<q[nIterations]<<" "<<dswe[nIterations]<<" "<<import[nIterations];
            nIterations++;
        }
    }
}
