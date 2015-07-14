#ifndef MORPH_FILEMANAGER_H
#define MORPH_FILEMANAGER_H

#include "morphed_lib.h"

class MORPHED_LIBSHARED_EXPORT MORPH_FileManager
{
public:
    MORPH_FileManager();
    MORPH_FileManager(QString rootDir);

    void copyFinalOutputs(int flood);
    void createDirectory(QString dirName);
    void createFloodDirectories(int flood);
    void setRootDir(QString rootDir);

    static QString copyFileToDirectory(QString originalPath, QString newDirectory, QString newFileName);
    static QString getFloodName(int flood);

private:
    QString rootDirPath,
            inputDirPath,
            outputDirPath,
            tempDirPath;

    QDir    root,
            input,
            output,
            temp;

    void setDirectories();
};

#endif // MORPH_FILEMANAGER_H
