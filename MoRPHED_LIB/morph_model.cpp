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

    QString path, dir;
    dir = m_inXML.readNodeData("ProjectDirectory") + "/Inputs/01_InitialInputs";
    path = m_inXML.readNodeData("OriginalDEMPath");

    path = m_FileManager.copyFileToDirectory(path, dir, "InitialDEM");
    m_inXML.writeNodeData("Inputs", "DEMPath", path);

    path = m_inXML.readNodeData("OriginalHydroSediPath");

    path = m_FileManager.copyFileToDirectory(path, dir, "InputHydroSedi");
    m_inXML.writeNodeData("Inputs", "HydroSediPath", path);

    m_inXML.printXML();

    return PROCESS_OK;
}

int MORPH_Model::run()
{
    MORPH_Delft3DIO *delft = new MORPH_Delft3DIO(m_inXML.getDocumentFilename());
    MORPH_SedimentTransport *trans = new MORPH_SedimentTransport(m_inXML.getDocumentFilename());


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
    }

    delete(delft);
    delete(trans);

    return PROCESS_OK;
}
