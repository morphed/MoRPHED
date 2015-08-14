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
#ifndef MORPH_SEDIMENTTRANSPORT_H
#define MORPH_SEDIMENTTRANSPORT_H

#include "morph_base.h"
#include "morph_pathlengthdist.h"
#include <QtConcurrent>

class MORPHED_LIBSHARED_EXPORT MORPH_SedimentTransport : public MORPH_Base
{
public:
    MORPH_SedimentTransport(QString xmlPath);

    void addDeposition();
    void calcBankShear();
    void calcLateralRetreat();
    void depositTo3x3(int row, int col, double amt);
    void depositTo5x5(int row, int col, double amt);
    void depositToDEM();
    void eliminateRegionsArea(int nRegions);
    void eliminateRegionsValue(int nRegions);
    void erodeBanks();
    void findDepositionCells(int row, int col, double amt);
    void importSediment();
    void loadRasters();
    void makeRegionInput();
    void recalcRegionInput();
    void runBankErode();
    void runBedErode();
    void setImportCells(QVector<int> rows, QVector<int> cols);
    void sloughBanks();
    void transportSediment();
    QVector<double> getMechanismVolumes();

private:
    double exported,
            unaccounted,
            counterBankDepoEvent,
            counterBankDepoTotal,
            counterBankErodEvent,
            counterBankErodTotal,
            counterBedDepoEvent,
            counterBedDepoTotal,
            counterBedErodEvent,
            counterBedErodTotal,
            counterImportEvent,
            counterImportTotal,
            counterExportEvent,
            counterExportTotal,
            maxSlope;

    QString qsXvelPath,
            qsYvelPath,
            qsFdirPath,
            qsShearPath,
            qsDepthPath,
            qsDepoPath,
            qsErodePath,
            qsErodeFilterPath,
            qsSlopePath,
            qsSlopeFiltPath,
            qsRegionsPath,
            qsInterRegPath,
            qsRegShearPath,
            qsAspectPath,
            qsLateralErode,
            qsBankErode,
            qsBankShear,
            qsBankErodeInput,
            qsDodPath;

    QVector<int> qvSeedRow,
                qvSeedCol,
                qvDepoRow,
                qvDepoCol,
                qvImportRow,
                qvImportCol;

    QVector<double> qvSeedAmt,
                    qvDepoAmt;

    GDALDataset *pShearRaster,
                *pDepthRaster,
                *pFdirRaster,
                *pOldDem,
                *pNewDem,
                *pErodeRaster,
                *pDepositRaster,
                *pSlopeRaster,
                *pDoD,
                *pAspectRaster;

    MORPH_PathLengthDist pathLength1;

    double averageShear_FlowLine(int row, int col, int cellsUS, int cellsDS);
    void clearDeposition();
    void clearErosion();
    void createDoD();
    double erodeBedFlow(double shearCrit, double shear, int row, int col);
    void findImportCells();
    QVector<double> findNextCell(double startX, double startY, int prevRow, int prevCol);
    QVector<double> findNextCell_Backward(double startX, double startY, int prevRow, int prevCol);
    QVector<int> findNextCell_D8(int startRow, int startCol, int prevRow, int prevCol);
    void fixDeposition(QString dodPath);
    void openDepositionRaster();
    void openDepthRaster();
    void openErosionRaster();
    void openFdirRaster();
    void openNewDem();
    void openOldDem();
    void openShearRaster();
    void openSlopeRaster();
    void printDelftDem();
    void setPaths();
    void runDeposition(const char *erosionRasterPath);

};

#endif // MORPH_SEDIMENTTRANSPORT_H
