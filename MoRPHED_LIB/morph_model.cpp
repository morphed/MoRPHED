#include "morph_model.h"

MORPH_Model::MORPH_Model(XMLReadWrite inputXML, QObject *parent) :
    QObject(parent)
{
    init(inputXML);
}

int MORPH_Model::init(XMLReadWrite inputXML)
{
    m_inXML = inputXML;

    QFileInfo fi(m_inXML.getDocumentFilename());
    if (!fi.exists())
    {
        throw MORPH_Exception(INPUTXML_ERROR, "xml path does not exist");
    }

    m_FileManager = MORPH_FileManager(fi.absolutePath());

    QString path, dir, dirName;
    QDir baseDir(m_inXML.readNodeData("ProjectDirectory"));
    dirName = baseDir.dirName();
    dir = m_inXML.readNodeData("ProjectDirectory") + "/Inputs/01_InitialInputs";
    path = m_inXML.readNodeData("OriginalDEMPath");

    path = m_FileManager.copyFileToDirectory(path, dir, "InitialDEM");
    m_inXML.writeNodeData("Inputs", "DEMPath", path);

    path = m_inXML.readNodeData("OriginalHydroSediPath");

    path = m_FileManager.copyFileToDirectory(path, dir, "InputHydroSedi");
    m_inXML.writeNodeData("Inputs", "HydroSediPath", path);

    m_inXML.printXML();

    m_outXML.writeXMLdocViewer();
    m_outXML.setDocumentFilename(m_inXML.readNodeData("ProjectDirectory") + "/" + dirName + ".display.morph");
    qDebug()<<(m_inXML.readNodeData("ProjectDirectory") + "/" + dirName + ".display.morph");
    m_outXML.printXML();

    return PROCESS_OK;
}

int MORPH_Model::run()
{
    MORPH_Delft3DIO *delft = new MORPH_Delft3DIO(m_inXML.getDocumentFilename());
    MORPH_SedimentTransport *trans = new MORPH_SedimentTransport(m_inXML.getDocumentFilename());

    QVector<double> volumes;


    for (int i=0; i<trans->getIterations(); i++)
    {

        if (i == 0)
        {
            m_FileManager.createFloodDirectories(i);
            qDebug()<<"running delft";
            delft->run();
            qDebug()<<"delft done, load rasters";
            trans->loadRasters();
            qDebug()<<"rasters loaded";
        }

        m_FileManager.createFloodDirectories(i+1);
        delft->setCurrentIteration(i);
        trans->setCurrentIteration(i);

        qDebug()<<"running bank erosoin";
        trans->runBankErode();

        qDebug()<<"running delft "<<i;
        delft->run();
        qDebug()<<"done running delft "<<i;

        trans->loadRasters();
        qDebug()<<"running bed erosion";
        trans->runBedErode();
        qDebug()<<"importing";
        trans->importSediment();
        qDebug()<<"flood done "<<i;
        volumes = trans->getMechanismVolumes();
        qDebug()<<"volumes obtained";
        writeDisplayData(trans->getCurrentIteration(), volumes);
        m_outXML.printXML();
    }

    delete(delft);
    delete(trans);

    return PROCESS_OK;
}

int MORPH_Model::writeDisplayData(int nFlood, QVector<double> volumes)
{
    nFlood = nFlood+1;
    QString eventName = "Event" + QString::number(nFlood);
    qDebug()<<eventName;
    m_outXML.writeEvent(nFlood);
    m_outXML.printXML();
    qDebug()<<"xml outline written";

    writeVolumes(eventName, volumes);
    qDebug()<<"volumes written";
    GDALAllRegister();
    writePngOutputs(eventName, nFlood);

    return PROCESS_OK;
}

int MORPH_Model::writePngOutputs(QString eventName, int nFlood)
{
    //Setup file names for PNG outputs
    QString floodName = m_FileManager.getFloodName(nFlood);
    qDebug()<<floodName;
    QString baseDir, demPath, depthPath, dodPath, hlsdPng, depthPng, dodPng;
    baseDir = m_inXML.readNodeData("ProjectDirectory");
    demPath = baseDir + "/Outputs/" + floodName + "/GTIFF/DEM_" + QString::number(nFlood) + ".tif";
    depthPath = baseDir + "/Outputs/" + floodName + "/GTIFF/Depth" + QString::number(nFlood) + ".tif";
    dodPath = baseDir + "/Outputs/" + floodName + "/GTIFF/DoD_" + QString::number(nFlood) + ".tif";
    hlsdPng = baseDir + "/Outputs/" + floodName + "/PNG/hlsd" + QString::number(nFlood) + ".tif";
    depthPng = baseDir + "/Outputs/" + floodName + "/PNG/depth" + QString::number(nFlood) + ".tif";
    dodPng = baseDir + "/Outputs/" + floodName + "/PNG/dod" + QString::number(nFlood) + ".tif";

    //Create PNG images from raster layers
    //Renderer_ByteData hlsdRender;
    Renderer_StretchMinMax depthRender(depthPath.toStdString().c_str(), CR_LtBlueDkBlue, 120, false, true);
    depthRender.rasterToPNG(depthPng.toStdString().c_str(), 100, 1000);
    depthRender.printLegend();
    Renderer_Classified dodRender(dodPath.toStdString().c_str(), 20, CR_DoD, 255, true, true);
    dodRender.rasterToPNG(dodPng.toStdString().c_str(), 100, 1000);
    dodRender.setPrecision(2);
    dodRender.printLegend();

    //delete(depthRender);
    //delete(dodRender);

    m_outXML.writeNodeData(eventName, "HillshadePath", hlsdPng);
    m_outXML.writeNodeData(eventName, "DoDCumulativePath", dodPng);
    m_outXML.writeNodeData(eventName, "WaterDepthPath", depthPng);

    return PROCESS_OK;
}

int MORPH_Model::writeVolumes(QString eventName, QVector<double> volumes)
{
    m_outXML.writeNodeData(eventName, "ExportedSediment", "Event", QString::number(volumes[0]));
    m_outXML.writeNodeData(eventName, "ExportedSediment", "Total", QString::number(volumes[1]));
    m_outXML.writeNodeData(eventName, "ImportedSediment", "Event", QString::number(volumes[2]));
    m_outXML.writeNodeData(eventName, "ImportedSediment", "Total", QString::number(volumes[3]));
    m_outXML.writeNodeData(eventName, "BedErosion", "Event", QString::number(volumes[4]));
    m_outXML.writeNodeData(eventName, "BedErosion", "Total", QString::number(volumes[5]));
    m_outXML.writeNodeData(eventName, "BedDeposition", "Event", QString::number(volumes[6]));
    m_outXML.writeNodeData(eventName, "BedDeposition", "Total", QString::number(volumes[7]));
    m_outXML.writeNodeData(eventName, "BankErosion", "Event", QString::number(volumes[8]));
    m_outXML.writeNodeData(eventName, "BankErosion", "Total", QString::number(volumes[9]));
    m_outXML.writeNodeData(eventName, "BankDeposition", "Event", QString::number(volumes[10]));
    m_outXML.writeNodeData(eventName, "BankDeposition", "Total", QString::number(volumes[11]));

    m_outXML.printXML();

    return PROCESS_OK;
}
