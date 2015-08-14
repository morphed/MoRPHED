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
