/*
This file is part of MoRPHED

MoRPHED is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

MoRPHED is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

(c)2015 MoRPHED Development team
*/
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
    m_outXML.printXML();

    return PROCESS_OK;
}

int MORPH_Model::run()
{
    MORPH_Delft3DIO *delft = new MORPH_Delft3DIO(m_inXML.getDocumentFilename());
    MORPH_SedimentTransport *trans = new MORPH_SedimentTransport(m_inXML.getDocumentFilename());
    m_outXML.writeNodeData("USBound", QString::number(trans->getUsBoundary()));

    QVector<double> volumes;


    for (int i=0; i<trans->getIterations(); i++)
    {

        if (i == 0)
        {
            m_FileManager.createFloodDirectories(i);
            qDebug()<<"running delft";
            delft->run();
            trans->loadRasters();
            qDebug()<<"rasters loaded";
        }

        m_FileManager.createFloodDirectories(i+1);
        delft->setCurrentIteration(i);
        trans->setCurrentIteration(i);

        qDebug()<<"running bank erosion";
        trans->runBankErode();

        if (i == 0)
        {
            writeInitialData(0);
        }

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

    m_FileManager.copyFinalOutputs(trans->getIterations());

    delete(delft);
    delete(trans);

    return PROCESS_OK;
}

int MORPH_Model::writeDisplayData(int nFlood, QVector<double> volumes)
{
    nFlood = nFlood+1;
    QString eventName = "Event" + QString::number(nFlood);
    m_outXML.writeEvent(nFlood);
    m_outXML.printXML();;

    writeVolumes(eventName, volumes);
    GDALAllRegister();
    writePngOutputs(eventName, nFlood);

    return PROCESS_OK;
}

int MORPH_Model::writeInitialData(int nFlood)
{
    QString floodName = m_FileManager.getFloodName(nFlood);
    QString baseDir, demPath, hydroPath, hlsdPath, hlsdPng, depthPath, depthPng;

    baseDir = m_inXML.readNodeData("ProjectDirectory");
    demPath = baseDir + "/Inputs/01_InitialInputs/InitialDEM.tif";
    hydroPath = baseDir + "/Inputs/01_InitialInputs/InputHydroSedi.txt";
    hlsdPath = baseDir + "/Inputs/01_InitialInputs/InitialHlsd.tif";
    depthPath = baseDir + "/Outputs/" + floodName + "/GTIFF/Depth" + QString::number(nFlood+1) + ".tif";
    hlsdPng = baseDir + "/Outputs/" + floodName + "/PNG/hlsd.png";
    depthPng = baseDir + "/Outputs/" + floodName + "/PNG/depth.png";

    //Create hillshade
    MORPH_Raster raster;
    raster.hillshade(demPath.toStdString().c_str(), hlsdPath.toStdString().c_str());

    Renderer_ByteData hlsdRender(hlsdPath.toStdString().c_str());
    hlsdRender.rasterToPNG(hlsdPng.toStdString().c_str(), 100, 1000);
    Renderer_StretchMinMax depthRender(depthPath.toStdString().c_str(), CR_LtBlueDkBlue, 120, false, true);
    depthRender.rasterToPNG(depthPng.toStdString().c_str(), 100, 1000);
    depthRender.printLegend();

    m_outXML.writeNodeData("InputHydroSedi", hydroPath);
    m_outXML.writeNodeData("InitialHillshade", hlsdPng);
    m_outXML.writeNodeData("InitialDepth", depthPng);

    return PROCESS_OK;
}

int MORPH_Model::writePngOutputs(QString eventName, int nFlood)
{
    //Setup file names for PNG outputs
    QString floodName = m_FileManager.getFloodName(nFlood);
    QString baseDir, demPath, demInitPath, depthPath, dodPath, dodTotPath, hlsdPath, hlsdPng, depthPng, dodPng, dodTotPng;
    baseDir = m_inXML.readNodeData("ProjectDirectory");
    demPath = baseDir + "/Outputs/" + floodName + "/GTIFF/DEM_" + QString::number(nFlood) + ".tif";
    demInitPath = baseDir + "/Inputs/01_InitialInputs/InitialDEM.tif";
    hlsdPath = baseDir + "/Outputs/" + floodName + "/GTIFF/hlsd_" + QString::number(nFlood) + ".tif";
    depthPath = baseDir + "/Outputs/" + floodName + "/GTIFF/Depth" + QString::number(nFlood) + ".tif";
    dodPath = baseDir + "/Outputs/" + floodName + "/GTIFF/DoD_" + QString::number(nFlood) + ".tif";
    dodTotPath = baseDir + "/Outputs/" + floodName + "/GTIFF/DoD_total" + QString::number(nFlood) + ".tif";
    hlsdPng = baseDir + "/Outputs/" + floodName + "/PNG/hlsd" + QString::number(nFlood) + ".png";
    depthPng = baseDir + "/Outputs/" + floodName + "/PNG/depth" + QString::number(nFlood) + ".png";
    dodPng = baseDir + "/Outputs/" + floodName + "/PNG/dod" + QString::number(nFlood) + ".png";
    dodTotPng = baseDir + "/Outputs/" + floodName + "/PNG/dod_total" + QString::number(nFlood) + ".png";

    //Create hillshade raster
    MORPH_Raster raster;
    raster.hillshade(demPath.toStdString().c_str(), hlsdPath.toStdString().c_str());

    //Create cumulative DoD
    qDebug()<<"writing cumulative dod";
    raster.subtract(demPath.toStdString().c_str(), demInitPath.toStdString().c_str(), dodTotPath.toStdString().c_str());
    qDebug()<<"dod done";

    //Create PNG images from raster layers
    Renderer_ByteData hlsdRender(hlsdPath.toStdString().c_str());
    hlsdRender.rasterToPNG(hlsdPng.toStdString().c_str(), 100, 1000);
    Renderer_StretchMinMax depthRender(depthPath.toStdString().c_str(), CR_LtBlueDkBlue, 120, false, true);
    depthRender.rasterToPNG(depthPng.toStdString().c_str(), 100, 1000);
    depthRender.printLegend();
    Renderer_Classified dodRender(dodPath.toStdString().c_str(), 20, CR_DoD, 255, true, true);
    dodRender.rasterToPNG(dodPng.toStdString().c_str(), 100, 1000);
    dodRender.setPrecision(2);
    dodRender.printLegend();
    Renderer_Classified dodRender2(dodTotPath.toStdString().c_str(), 20, CR_DoD, 255, true, true);
    dodRender2.rasterToPNG(dodTotPng.toStdString().c_str(), 100, 1000);
    dodRender2.setPrecision(2);
    dodRender2.printLegend();

    m_outXML.writeNodeData(eventName, "HillshadePath", hlsdPng);
    m_outXML.writeNodeData(eventName, "DoDRecentPath", dodPng);
    m_outXML.writeNodeData(eventName, "DoDCumulativePath", dodTotPng);
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
