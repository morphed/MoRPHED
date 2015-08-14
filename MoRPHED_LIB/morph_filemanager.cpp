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
#include "morph_filemanager.h"

MORPH_FileManager::MORPH_FileManager()
{
}

MORPH_FileManager::MORPH_FileManager(QString rootDir)
{
    rootDirPath = rootDir;
    setDirectories();
}

void MORPH_FileManager::copyFinalOutputs(int flood)
{
    QString floodName = getFloodName(flood);
    QString demPath = outputDirPath + "/" + floodName + "/GTIFF/DEM_" + QString::number(flood) + ".tif";
    QString depthPath = outputDirPath + "/" + floodName + "/GTIFF/Depth" + QString::number(flood) + ".tif";
    QString newDemPath = outputDirPath + "/01_FinalOutputs/dem.tif";
    QString newDepthPath = outputDirPath + "/01_FinalOutputs/depth.tif";

    QFile::copy(demPath, newDemPath);
    QFile::copy(depthPath, newDepthPath);
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
