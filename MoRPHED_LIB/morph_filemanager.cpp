#include "morph_filemanager.h"

MORPH_FileManager::MORPH_FileManager()
{
}

MORPH_FileManager::MORPH_FileManager(QString rootDir)
{
    rootDirPath = rootDir;
    setDirectories();
}

void MORPH_FileManager::createDirectory(QString dirName)
{

}

void MORPH_FileManager::createFloodDirectories(int flood)
{
    QString floodName = getFloodName(flood);

    if (flood < 10)
    {
        floodName = "Flood00" + QString::number(flood);
    }
    else if (flood < 100)
    {
        floodName = "Flood0" + QString::number(flood);
    }
    else if (flood <1000)
    {
        floodName = "Flood" + QString::number(flood);
    }
    else
    {

    }

    output.mkdir(floodName);
    input.mkdir(floodName);
    QDir inFlood (input.absolutePath() + "/" + floodName);
    QDir outFlood (output.absolutePath() + "/" + floodName);
    outFlood.mkdir("GTIFF");
    outFlood.mkdir("PNG");
    inFlood.mkdir("Delft3D");
    inFlood.mkdir("Hydraulics");
}

void MORPH_FileManager::setRootDir(QString rootDir)
{
    rootDirPath = rootDir;
    setDirectories();
}

QString MORPH_FileManager::copyFileToDirectory(QString originalPath, QString newDirectory, QString newFileName)
{
    bool result = false;
    QFile file(originalPath);
    QFileInfo fileInfo(file);
    QString suf;
    suf = fileInfo.suffix();
    QString newPath = newDirectory+"/"+newFileName + "." + suf;
    QFile file1(newPath);
    if (file1.exists())
    {
        file1.remove();
    }
    result = QFile::copy(originalPath, newPath);

    if (!result)
    {
        newPath = QString();
        qDebug()<<"copy unsuccessful";
    }

    return newPath;
}

QString MORPH_FileManager::getFloodName(int flood)
{
    QString floodName;
    if (flood < 10)
    {
        floodName = "Flood00" + QString::number(flood);
    }
    else if (flood < 100)
    {
        floodName = "Flood0" + QString::number(flood);
    }
    else if (flood <1000)
    {
        floodName = "Flood" + QString::number(flood);
    }
    else
    {

    }
    return floodName;
}

void MORPH_FileManager::setDirectories()
{
    inputDirPath = rootDirPath + "/Inputs";
    outputDirPath = rootDirPath + "/Outputs";
    tempDirPath = rootDirPath + "/temp";

    root = QDir(rootDirPath);
    input = QDir(inputDirPath);
    output = QDir(outputDirPath);
    temp = QDir(tempDirPath);

    if (!input.exists())
    {
        root.mkdir("Inputs");
    }

    root.mkdir("Outputs");
    root.mkdir("temp");

    input.mkdir("01_InitialInputs");
    output.mkdir("01_FinalOutputs");
}
