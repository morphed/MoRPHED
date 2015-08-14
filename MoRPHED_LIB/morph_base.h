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

#ifndef MORPH_BASE_H
#define MORPH_BASE_H

#include "morphed_lib.h"
#include "xmlreadwrite.h"
#include "morph_raster.h"
#include "morph_filemanager.h"
#include "morph_exception.h"

class MORPHED_LIBSHARED_EXPORT MORPH_Base : QObject
{
public:
    MORPH_Base(QString xmlPath);
    ~MORPH_Base();

    int getCurrentIteration();
    int getIterations();
    int getUsBoundary();
    void setCurrentIteration(int iter);

    static double findMaxVector(QVector<double> vector);
    static void loadInputText(QString filename, QVector<double> &dates, QVector<double> &discharge, QVector<double> &waterElev, QVector<double> &sediment);

protected:
    XMLReadWrite XmlInit;

    int     nRows,
            nCols,
            nIterations,
            nCurrentIteration,
            nImportType,
            nGsType,
            nGsActType,
            nGsSubType,
            nPlDistType1,
            nPlDistType2,
            nDirDSbound,
            nDirUSbound;

    double  transform[6],
            noData,
            sigA1,
            muB1,
            sigA2,
            muB2,
            simTime,
            timeStep,
            roughness,
            hev,
            cellWidth,
            cellHeight,
            topLeftX,
            topLeftY,
            plDistLength1,
            plDistLength2,
            gsActiveSize,
            bankSlopeThresh,
            bankShearThresh,
            erosionFactor,
            areaThresh;

    QString qsRootPath,
            qsInputPath,
            qsOutputPath,
            qsTempPath,
            qsOrigDemPath,
            qsHydroSediPath,
            qsFloodName,
            qsInputText,
            qsXmlPath,
            qsOldDemPath,
            qsNewDemPath,
            qsDelftDemPath;

    QVector<double> dswe,
                    date,
                    q,
                    import;

    GDALDriver *pDriverTIFF;

    void loadDrivers();

private:
    void init();

    void loadInputText();

signals:
    void sendMessage(QString message);

public slots:

};

#endif // MORPH_BASE_H
