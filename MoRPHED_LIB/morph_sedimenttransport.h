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

private:
    double exported,
            unaccounted,
            counterDepoEvent,
            counterDepoTotal,
            counterErodEvent,
            counterErodTotal,
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
