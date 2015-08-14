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

#ifndef MORPH_DELFT3DIO_H
#define MORPH_DELFT3DIO_H

#include "morph_base.h"
#include "unistd.h"
#include <cstdlib>

class MORPHED_LIBSHARED_EXPORT MORPH_Delft3DIO : public MORPH_Base
{
public:
    static const int nAddCells = 20;
    static const double DELFT_NODATA = -999.000;

    MORPH_Delft3DIO(QString xmlPath);
    ~MORPH_Delft3DIO();

    void setDelftData();

    void calculateDSWSE();
    void closeDelftDatasets();
    void extendDEMBoundary();
    QVector<int> getDisColVector();
    QVector<int> getDisRowVector();
    void run();
    void setDischargePoints();
    void setDownstreamBoundary();
    void setObservationPoints();
    void setOriginPoint();
    void setOutputPaths();
    void setXDischargeAddresses();
    void setYDischargeAddresses();
    void writeBND();
    void writeBCQ();
    void writeDEP();
    void writeDIS();
    void writeENC();
    void writeFIL();
    void writeGRD();
    void writeINI();
    void writeMDF();
    void writeOBS();
    void writeOutputMacro();
    void writeSRC();
    void writeXYZ();
    int xCellAddress(double coord);
    int yCellAddress(double coord);

private:
    int     nRowsExt,
            nColsExt,
            nusx1,
            nusx2,
            nusy1,
            nusy2,
            ndsx1,
            ndsx2,
            ndsy1,
            ndsy2,
            xstart,
            xend,
            ystart,
            yend,
            disCount,
            disMin,
            disMax,
            nPID;

    double  extTransform[6],
            usx1,
            usx2,
            usy1,
            usy2,
            dsx1,
            dsx2,
            dsy1,
            dsy2,
            xLowerLeft,
            yLowerLeft,
            xOrigin,
            yOrigin,
            origDemMax,
            newDemMax,
            dSlope;

    QString qsDelftPath,
            qsExtendedDemPath,
            qsBnd,
            qsBcq,
            qsDep,
            qsDis,
            qsEnc,
            qsFil,
            qsGrd,
            qsIni,
            qsMdf,
            qsObs,
            qsMacro,
            qsSrc,
            qsXyz;

    QVector<int>    xUSCoords,
                    yUSCoords,
                    xObsCoords,
                    yObsCoords,
                    dsCoords;

    GDALDataset *pExtendedDEM,
                *pSourceDEM;

    QProcess    processDelft,
                processQp;

    void openExtendedDEM();
    void openSourceDEM();

signals:
    void updateQuickPlotPID(int qdID);
    void updateDelftPID(int delftID);

public slots:

};

#endif // MORPH_DELFT3DIO_H
