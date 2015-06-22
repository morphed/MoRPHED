#include "morph_sedimenttransport.h"

MORPH_SedimentTransport::MORPH_SedimentTransport(QString xmlPath) : MORPH_Base(xmlPath)
{
    pathLength1.setupDistribution(plDistLength1, sigA1, muB1, nPlDistType1, cellWidth);
    exported = 0.0;
    unaccounted = 0.0;
    counterDepoTotal = 0.0;
    counterErodTotal = 0.0;
    counterDepoEvent = 0.0;
    counterErodEvent = 0.0;

    MORPH_Raster Raster;
    QString path = qsTempPath + "/slope_init.tif";
    Raster.slopeTOF(qsOldDemPath.toStdString().c_str(), path.toStdString().c_str());
    maxSlope = Raster.findMax(path.toStdString().c_str());
}

void MORPH_SedimentTransport::addDeposition()
{
    openDepositionRaster();

    float *oldVal = (float*) CPLMalloc(sizeof(float)*1);
    float *newVal = (float*) CPLMalloc(sizeof(float)*1);

    for (int i=0; i<qvDepoRow.size(); i++)
    {
        if (qvDepoRow[i] > 1 && qvDepoRow[i] < nRows-2 && qvDepoCol[i] > 1 && qvDepoCol[1] < nCols-2)
        {
            pDepositRaster->GetRasterBand(1)->RasterIO(GF_Read, qvDepoCol[i], qvDepoRow[i], 1, 1, oldVal, 1, 1, GDT_Float32, 0, 0);

            *newVal = *oldVal + qvDepoAmt[i];
            counterDepoEvent += qvDepoAmt[i];
            counterDepoTotal += qvDepoAmt[i];

            pDepositRaster->GetRasterBand(1)->RasterIO(GF_Write, qvDepoCol[i], qvDepoRow[i], 1, 1, newVal, 1, 1, GDT_Float32, 0, 0);
        }
        else
        {
            //update exported here
            //pDepositRaster->GetRasterBand(1)->RasterIO(GF_Read, qvDepoCol[i], qvDepoRow[i], 1, 1, oldVal, 1, 1, GDT_Float32, 0, 0);

            if (qvDepoAmt[i] < 50.0)
            {
                if (fabs(*oldVal) > 0.0)
                {
                    exported += qvDepoAmt[i];
                }
            }
        }
    }

    qvDepoRow.clear();
    qvDepoCol.clear();
    qvDepoAmt.clear();

    GDALClose(pDepositRaster);


    CPLFree(oldVal);
    CPLFree(newVal);
}

void MORPH_SedimentTransport::calcBankShear()
{
    GDALDataset *pRegions, *pRegionShear, *pAspect;

    pRegions = (GDALDataset*) GDALOpen(qsRegionsPath.toStdString().c_str(), GA_Update);
    pAspect = (GDALDataset*) GDALOpen(qsAspectPath.toStdString().c_str(), GA_ReadOnly);

    pRegionShear = pDriverTIFF->Create(qsRegShearPath.toStdString().c_str(), nCols, nRows, 1, GDT_Float32, NULL);

    pRegionShear->GetRasterBand(1)->Fill(noData);
    pRegionShear->SetGeoTransform(transform);
    pRegionShear->GetRasterBand(1)->SetNoDataValue(noData);

    float *regRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *aspRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *shearBlock = (float*) CPLMalloc(sizeof(float)*15);
    float *shearVal = (float*) CPLMalloc(sizeof(float)*1);

    bool calc;
    int shearCount;
    int count = 0;

    openShearRaster();

    for (int i=1; i<nRows-1; i++)
    {
        pRegions->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, regRow, nCols, 1, GDT_Float32, 0, 0);
        pAspect->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, aspRow, nCols, 1, GDT_Float32, 0, 0);

        for (int j=1; j<nCols-1; j++)
        {
            if (regRow[j] > 0.0)
            {
                calc = true;

                if ((aspRow[j]>=0.0 && aspRow[j]<45.0) || (aspRow[j]<=360.0 && aspRow[j]>315.0))
                {
                    pShearRaster->GetRasterBand(1)->RasterIO(GF_Read, j-1, i-4, 3, 5, shearBlock, 3, 5, GDT_Float32, 0, 0);
                }
                else if (aspRow[j]>=45.0 && aspRow[j]<135.0)
                {
                    pShearRaster->GetRasterBand(1)->RasterIO(GF_Read, j, i-1, 5, 3, shearBlock, 5, 3, GDT_Float32, 0, 0);
                }
                else if (aspRow[j]>=135.0 && aspRow[j]<225.0)
                {
                    pShearRaster->GetRasterBand(1)->RasterIO(GF_Read, j-1, i, 5, 3, shearBlock, 5, 3, GDT_Float32, 0, 0);
                }
                else if (aspRow[j]>=225.0 && aspRow[j]<=315.0)
                {
                    pShearRaster->GetRasterBand(1)->RasterIO(GF_Read, j-4, i-1, 5, 3, shearBlock, 5, 3, GDT_Float32, 0, 0);
                }
                else
                {
                    calc = false;
                }

                shearCount = 0;
                if (calc)
                {
                    for (int k=0; k<15; k++)
                    {
                        if (fabs(shearBlock[k]) > bankShearThresh)
                        {
                            shearCount++;
                            count++;
                        }
                    }
                }
                *shearVal = shearCount * 1.0;
                pRegionShear->GetRasterBand(1)->RasterIO(GF_Write, j, i, 1, 1, shearVal, 1, 1, GDT_Float32, 0, 0);
            }
        }
    }

    GDALClose(pRegions);
    GDALClose(pAspect);
    GDALClose(pRegionShear);
    GDALClose(pShearRaster);

    CPLFree(regRow);
    CPLFree(aspRow);
    CPLFree(shearBlock);
    CPLFree(shearVal);
}

void MORPH_SedimentTransport::calcLateralRetreat()
{
    GDALDataset *pBankShear, *pRetreat, *pSlopeFilt;

    pBankShear = (GDALDataset*) GDALOpen(qsBankShear.toStdString().c_str(), GA_ReadOnly);
    pSlopeFilt = (GDALDataset*) GDALOpen(qsSlopeFiltPath.toStdString().c_str(), GA_ReadOnly);
    pRetreat = pDriverTIFF->Create(qsLateralErode.toStdString().c_str(), nCols, nRows, 1, GDT_Float32, NULL);
    pRetreat->GetRasterBand(1)->Fill(noData);
    pRetreat->GetRasterBand(1)->SetNoDataValue(noData);
    pRetreat->SetGeoTransform(transform);

    openSlopeRaster();

    float *shrRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *slpRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *newRow = (float*) CPLMalloc(sizeof(float)*nCols);

    for (int i=0; i<nRows; i++)
    {
        pBankShear->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, shrRow, nCols, 1, GDT_Float32, 0, 0);
        pSlopeFilt->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, slpRow, nCols, 1, GDT_Float32, 0, 0);

        for (int j=0; j<nCols; j++)
        {
            if (shrRow[j] == noData || slpRow[j] == noData)
            {
                newRow[j] = noData;
            }
            else
            {
//                if (slpRow[j] > maxSlope)
//                {
//                    slpRow[j] = maxSlope;
//                }
                newRow[j] = (round((shrRow[j]/3.0 + 1) * (slpRow[j]/15.0)));
            }
        }

        pRetreat->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, newRow, nCols, 1, GDT_Float32, 0, 0);

    }

    GDALClose(pBankShear);
    GDALClose(pSlopeFilt);
    GDALClose(pRetreat);
    GDALClose(pSlopeRaster);

    CPLFree(shrRow);
    CPLFree(slpRow);
    CPLFree(newRow);
}

void MORPH_SedimentTransport::depositTo3x3(int row, int col, double amt)
{
    QVector<double> qvMatrix(2);
    double reassign, total = 0.0;
    int wetCells, dryCells;

    qvMatrix[0] = 0.2;
    qvMatrix[1] = 0.1;
    wetCells = 0, dryCells = 0;

    float *depVal = (float*) CPLMalloc(sizeof(float)*9);

    pDepthRaster->GetRasterBand(1)->RasterIO(GF_Read, col-1, row-1, 3, 3, depVal, 3, 3, GDT_Float32, 0, 0);

    for (int i=0; i<9; i++)
    {
        if (depVal[i] > 0.0)
        {
            wetCells++;
        }
        else
        {
            dryCells++;
        }
    }

    if (dryCells > 0)
    {
        reassign = 1.0 / (wetCells*1.0 + 1.0);
        qvMatrix[0] = reassign*2.0;
        qvMatrix[1] = reassign;
    }

    for (int i=0; i<9; i++)
    {
        if (depVal[i] > 0.0)
        {
            qvDepoRow.append(row + ROW_OFFSET[i]);
            qvDepoCol.append(col + COL_OFFSET[i]);

            if (i == 4)
            {
                qvDepoAmt.append(amt * qvMatrix[0]);
                total += (amt * qvMatrix[0]);
            }
            else
            {
                qvDepoAmt.append(amt * qvMatrix[1]);
                total += (amt * qvMatrix[1]);
            }
        }
    }

    if (total < amt || total > amt)
    {
        qvDepoRow.append(row);
        qvDepoCol.append(col);
        qvDepoAmt.append(amt-total);
    }

    CPLFree(depVal);
}

void MORPH_SedimentTransport::depositTo5x5(int row, int col, double amt)
{
    QVector<double> qvMatrix(3);
    double reassign, total = 0.0;
    int wetCellsOut, wetCellsIn, wetCells, dryCellsOut, dryCellsIn;

    qvMatrix[0] = 0.088, qvMatrix[1] = 0.054, qvMatrix[2] = 0.03;
    wetCells = 1, wetCellsIn = 0, wetCellsOut = 0, dryCellsIn = 0; dryCellsOut = 0;

    float *depWin = (float*) CPLMalloc(sizeof(float)*25);

    pDepthRaster->GetRasterBand(1)->RasterIO(GF_Read, col-2, row-2, 5, 5, depWin, 5, 5, GDT_Float32, 0, 0);

    for (int i=0; i<25; i++)
    {
        if (depWin[i] > 0.0)
        {
            if (ROW_OFFSET5[i] == (-2) || ROW_OFFSET5[i] == (2) || COL_OFFSET5[i] == (-2) || COL_OFFSET5[i] == (2))
            {
                wetCellsOut++;
                wetCells++;
            }
            else if (ROW_OFFSET5[i] == (-1) || ROW_OFFSET5[i] == (1) || COL_OFFSET5[i] == (-1) || COL_OFFSET5[i] == (1))
            {
                wetCellsIn++;
                wetCells++;
            }
        }
        else
        {
            if (ROW_OFFSET5[i] == (-2) || ROW_OFFSET5[i] == (2) || COL_OFFSET5[i] == (-2) || COL_OFFSET5[i] == (2))
            {
                dryCellsOut++;
            }
            else if (ROW_OFFSET5[i] == (-1) || ROW_OFFSET5[i] == (1) || COL_OFFSET5[i] == (-1) || COL_OFFSET5[i] == (1))
            {
                dryCellsIn++;
            }
        }

    }

    if ((dryCellsIn + dryCellsOut) > 0)
    {
        reassign = ((1.0)/((wetCellsOut*1.0) + (wetCellsIn*2.0) + 3.0));
        qvMatrix[2] = reassign;
        qvMatrix[1] = reassign * 2.0;
        qvMatrix[0] = reassign * 3.0;
    }

    for (int i=0; i<25; i++)
    {
        if (depWin[i] > 0.0)
        {

            qvDepoRow.append(row + ROW_OFFSET5[i]);
            qvDepoCol.append(col + COL_OFFSET5[i]);

            if (ROW_OFFSET5[i] == (-2) || ROW_OFFSET5[i] == (2) || COL_OFFSET5[i] == (-2) || COL_OFFSET5[i] == (2))
            {
                qvDepoAmt.append(amt * qvMatrix[2]);
                total += (amt*qvMatrix[2]);
            }
            else if (ROW_OFFSET5[i] == (-1) || ROW_OFFSET5[i] == (1) || COL_OFFSET5[i] == (-1) || COL_OFFSET5[i] == (1))
            {
                qvDepoAmt.append(amt * qvMatrix[1]);
                total += (amt*qvMatrix[1]);
            }
            else
            {
                qvDepoAmt.append(amt * qvMatrix[0]);
                total += (amt*qvMatrix[0]);
            }
        }
    }

    if (total < amt || total > amt)
    {
        qvDepoRow.append(row);
        qvDepoCol.append(col);
        qvDepoAmt.append(amt-total);
    }

    CPLFree(depWin);
}

void MORPH_SedimentTransport::depositToDEM()
{
    openNewDem();

    float *oldVal = (float*) CPLMalloc(sizeof(float)*1);
    float *newVal = (float*) CPLMalloc(sizeof(float)*1);

    for (int i=0; i<qvDepoRow.size(); i++)
    {
        pNewDem->GetRasterBand(1)->RasterIO(GF_Read, qvDepoCol[i], qvDepoRow[i], 1, 1, oldVal, 1, 1, GDT_Float32, 0, 0);

        *newVal = *oldVal + qvDepoAmt[i];

        pNewDem->GetRasterBand(1)->RasterIO(GF_Write, qvDepoCol[i], qvDepoRow[i], 1, 1, newVal, 1, 1, GDT_Float32, 0, 0);
    }

    GDALClose(pNewDem);

    CPLFree(oldVal);
    CPLFree(newVal);
}

void MORPH_SedimentTransport::eliminateRegionsArea(int nRegions)
{
    GDALDataset *pRegions;
    pRegions = (GDALDataset*) GDALOpen(qsRegionsPath.toStdString().c_str(), GA_Update);
    int minCells = round(areaThresh/cellWidth);

    float *regVal = (float*) CPLMalloc(sizeof(float)*1);

    for (int i=1; i<=nRegions; i++)
    {
        int regCount = 0;
        for (int j=0; j<nRows; j++)
        {
            for (int k=0; k<nCols; k++)
            {
                pRegions->GetRasterBand(1)->RasterIO(GF_Read, k, j, 1, 1, regVal, 1, 1, GDT_Float32, 0, 0);

                if (*regVal == i)
                {
                    regCount++;
                }
            }
        }

        if (regCount < minCells)
        {
            for (int j=0; j<nRows; j++)
            {
                for (int k=0; k<nCols; k++)
                {
                    pRegions->GetRasterBand(1)->RasterIO(GF_Read, k, j, 1, 1, regVal, 1, 1, GDT_Float32, 0, 0);

                    if (*regVal == i)
                    {
                        *regVal = noData;
                        pRegions->GetRasterBand(1)->RasterIO(GF_Write, k, j, 1, 1, regVal, 1, 1, GDT_Float32, 0, 0);
                    }
                }
            }
        }
    }

    GDALClose(pRegions);

    CPLFree(regVal);
}

void MORPH_SedimentTransport::eliminateRegionsValue(int nRegions)
{
    QVector<double> regionMeans(nRegions);

    GDALDataset *pRegions, *pValues;

    pRegions = (GDALDataset*) GDALOpen(qsRegionsPath.toStdString().c_str(), GA_Update);
    pValues = (GDALDataset*) GDALOpen(qsRegShearPath.toStdString().c_str(), GA_Update);

    float *regRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *valRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *regRowNew = (float*) CPLMalloc(sizeof(float)*nCols);
    float *valRowNew = (float*) CPLMalloc(sizeof(float)*nCols);

    double total;
    int count;

    for (int i=1; i<nRegions+1; i++)
    {
        total = 0.0;
        count = 0;

        for (int j=0; j<nRows; j++)
        {
            pRegions->GetRasterBand(1)->RasterIO(GF_Read, 0, j, nCols, 1, regRow, nCols, 1, GDT_Float32, 0,0);
            pValues->GetRasterBand(1)->RasterIO(GF_Read, 0, j, nCols, 1, valRow, nCols, 1, GDT_Float32, 0,0);

            for (int k=0; k<nCols; k++)
            {
                if (regRow[k] == i)
                {
                    total += valRow[k];
                    count++;
                }
            }
        }

        regionMeans[i-1] = (total / (count*1.0));
    }

    for (int i=0; i<nRows; i++)
    {
        pRegions->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, regRow, nCols, 1, GDT_Float32, 0,0);
        pValues->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, valRow, nCols, 1, GDT_Float32, 0,0);

        for (int j=0; j<nCols; j++)
        {
            if (regRow[j] != noData)
            {
                if (regionMeans[regRow[j] - 1] < 7.0)
                {
                    regRowNew[j] = noData;
                    valRowNew[j] = noData;
                }
                else
                {
                    regRowNew[j] = regRow[j];
                    valRowNew[j] = valRow[j];
                }
            }
            else
            {
                regRowNew[j] = noData;
                valRowNew[j] = noData;
            }
        }

        pRegions->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, regRowNew, nCols, 1, GDT_Float32, 0,0);
        pValues->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, valRowNew, nCols, 1, GDT_Float32, 0,0);
    }

    GDALClose(pRegions);
    GDALClose(pValues);

    CPLFree(regRow);
    CPLFree(valRow);
    CPLFree(regRowNew);
    CPLFree(valRowNew);
}

void MORPH_SedimentTransport::erodeBanks()
{
    GDALDataset *pRetreat, *pAspect, *pSlopeFilt;

    pRetreat = (GDALDataset*) GDALOpen(qsLateralErode.toStdString().c_str(), GA_ReadOnly);
    pAspect = (GDALDataset*) GDALOpen(qsAspectPath.toStdString().c_str(), GA_ReadOnly);
    pSlopeFilt = (GDALDataset*) GDALOpen(qsSlopeFiltPath.toStdString().c_str(), GA_ReadOnly);

    openOldDem();
    openNewDem();
    openErosionRaster();

    float *valRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *aspRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *slpRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *erodeVal = (float*) CPLMalloc(sizeof(float)*9);
    float *dem3x3 = (float*) CPLMalloc(sizeof(float)*9);
    float *erode3x3 = (float*) CPLMalloc(sizeof(float)*9);
    float *dem3x3New = (float*) CPLMalloc(sizeof(float)*9);

    int nLateral, nCountElev, colNew, rowNew;
    double adjElev, addElev;
    bool erode;

    for (int i=1; i<nRows-1; i++)
    {
        pRetreat->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, valRow, nCols, 1, GDT_Float32, 0, 0);
        pAspect->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, aspRow, nCols, 1, GDT_Float32, 0, 0);
        pSlopeFilt->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, slpRow, nCols, 1, GDT_Float32, 0, 0);

        for (int j=1; j<nCols-1; j++)
        {
            if (valRow[j] != noData)
            {
                nCountElev = 0;
                addElev = 0.0;
                pOldDem->GetRasterBand(1)->RasterIO(GF_Read, j-1, i-1, 3, 3, dem3x3, 3, 3, GDT_Float32, 0, 0);
                nLateral = round(valRow[j]);
                if (valRow[j] > 0.5 && nLateral < 1)
                {
                    qDebug()<<"ERROR: lateral erosion error "<<valRow[j]<< nLateral;
                }

                for (int k=0; k<9; k++)
                {
                    if (dem3x3[k] < dem3x3[4] && dem3x3[k] != noData)
                    {
                        nCountElev++;
                        addElev += dem3x3[k];
                    }
                }

                adjElev = (addElev / (nCountElev*1.0));
                rowNew = i, colNew = j;

                for (int k=0; k<nLateral; k++)
                {
                    erode = true;

                    if (k > 0)
                    {
                        if ((aspRow[j]>0.0 && aspRow[j]<=22.5) || (aspRow[j]>337.5 && aspRow[j]<=360.0))
                        {
                            rowNew = rowNew+1;
                            colNew = colNew;
                        }
                        else if (aspRow[j]>22.5 && aspRow[j]<=67.5)
                        {
                            rowNew = rowNew+1;
                            colNew = colNew-1;
                        }
                        else if (aspRow[j]>67.5 && aspRow[j]<=112.5)
                        {
                            rowNew = rowNew;
                            colNew = colNew-1;
                        }
                        else if (aspRow[j]>112.5 && aspRow[j]<=157.5)
                        {
                            rowNew = rowNew-1;
                            colNew = colNew-1;
                        }
                        else if (aspRow[j]>157.5 && aspRow[j]<=202.5)
                        {
                            rowNew = rowNew-1;
                            colNew = colNew;
                        }
                        else if (aspRow[j]>202.5 && aspRow[j]<=247.5)
                        {
                            rowNew = rowNew-1;
                            colNew = colNew+1;
                        }
                        else if (aspRow[j]>247.5 && aspRow[j]<=292.5)
                        {
                            rowNew = rowNew;
                            colNew = colNew+1;
                        }
                        else if (aspRow[j]>292.5 && aspRow[j]<=337.5)
                        {
                            rowNew = rowNew+1;
                            colNew = colNew+1;
                        }
                        else
                        {
                            erode = false;
                            qDebug()<<"ERROR: Aspect value out of range "<<aspRow[j];
                        }
                    }

                    if ((rowNew>nRows-1 || rowNew<1) || (colNew>nCols-1 || colNew<1))
                    {
                        erode = false;
                        qDebug()<<"ERROR: erode bank target cell out of range "<<rowNew<< colNew;
                    }

                    if (erode)
                    {
                        pNewDem->GetRasterBand(1)->RasterIO(GF_Read, colNew-1, rowNew-1, 3, 3, dem3x3New, 3, 3, GDT_Float32, 0, 0);
                        pErodeRaster->GetRasterBand(1)->RasterIO(GF_Read, colNew-1, rowNew-1, 3, 3, erode3x3, 3, 3, GDT_Float32, 0, 0);

                        for (int l=0; l<9; l++)
                        {
                            if (dem3x3New[l] > adjElev)
                            {
//                                if ((dem3x3New[l]-adjElev) > erode3x3[l])
//                                {
//                                    erodeVal[l] = dem3x3New[l]-adjElev;
//                                }
                                dem3x3New[l] = adjElev;
                            }
                            else
                            {
                                //erodeVal[l] = erode3x3[l]*0.0;
                            }
                        }

                        pNewDem->GetRasterBand(1)->RasterIO(GF_Write, colNew-1, rowNew-1, 3, 3, dem3x3New, 3, 3, GDT_Float32, 0, 0);
                    }
                }
            }
        }
    }

    qDebug()<<"bank erode loop finished";
    GDALClose(pRetreat);
    GDALClose(pAspect);
    GDALClose(pSlopeFilt);
    GDALClose(pOldDem);
    GDALClose(pNewDem);
    GDALClose(pErodeRaster);

    CPLFree(valRow);
    CPLFree(aspRow);
    CPLFree(slpRow);
    CPLFree(erodeVal);
    CPLFree(dem3x3);
    CPLFree(erode3x3);
    CPLFree(dem3x3New);
}

void MORPH_SedimentTransport::findDepositionCells(int row, int col, double amt)
{
    int prevRow, prevCol, prevRowHold, prevColHold;
    double sediment, sedimentRemaining;
    QVector<double> cells(4);

    sediment = amt;
    prevRow = 0, prevCol = 0;
    sedimentRemaining = sediment;

    for (int p=0; p<pathLength1.getLengthCells(); p++)
    {
        if (row>1 && row<nRows-2 && col>1 && col<nCols-2)
        {
            if (p == 0)
            {
                cells[0] = row;
                cells[1] = col;
                cells[2] = transform[0] + (col*cellWidth);
                cells[3] = transform[3] - (row*cellWidth);
            }
            prevRowHold = row, prevColHold = col;
            cells = findNextCell(cells[2], cells[3], prevRow, prevCol);
            row = cells[0];
            col = cells[1];
            prevRow = prevRowHold, prevCol = prevColHold;
            if (row<2 || row>nRows-2 || col<2 || col>nCols-2)
            {
                //add all remaining sediment to exported sediment here
                if (fabs(sedimentRemaining) < 50.0)
                {
                    if (fabs(sedimentRemaining) > 0.0)
                    {
                        exported += sedimentRemaining;
                    }
                }
                sedimentRemaining = 0.0;
                sediment = 0.0;
                break;
            }
            else
            {
                if (p < 3)
                {
                    //deposit to 3x3 here
                    depositTo3x3(row, col, (sediment*pathLength1.getValueAtPosition(p)));
                }
                else if (p >=3 )
                {
                    //deposit to 5x5 here
                    depositTo5x5(row, col, (sediment*pathLength1.getValueAtPosition(p)));
                }
                else
                {
                    qDebug()<<"no candidate distrib triggered";
                }
                sedimentRemaining -= (sediment * pathLength1.getValueAtPosition(p));
            }
        }
        else
        {
            //add all remaining sediment to exported sediment here
            if (fabs(sedimentRemaining) < 50.0)
            {
                if (fabs(sedimentRemaining) > 0.0)
                {
                    exported += sedimentRemaining;
                }
            }
            sedimentRemaining = 0.0;
            sediment = 0.0;
            break;
        }
    }
}

void MORPH_SedimentTransport::importSediment()
{
    pathLength1.setupDistribution(plDistLength2, sigA2, muB2, nPlDistType2, cellWidth);

    findImportCells();

    QString path1 = qsOutputPath + "/" + qsFloodName + "/GTIFF/ImportDepo" + QString::number(nCurrentIteration+1) + ".tif";
    QFile file(path1);
    if (file.exists())
    {
        file.remove();
    }
    QFile::copy(qsDepoPath, path1);

    MORPH_Raster Raster;

    clearDeposition();

    loadDrivers();
    createDoD();
    path1 = qsOutputPath + "/" + qsFloodName + "/GTIFF/DoD_" + QString::number(nCurrentIteration+1) + "_temp.tif";

    Raster.add(qsNewDemPath.toStdString().c_str(), path1.toStdString().c_str());

    qDebug()<<"undeposited "<<unaccounted;
    Raster.copyBoundary(qsNewDemPath.toStdString().c_str(), nDirDSbound);
    Raster.copyBoundary(qsNewDemPath.toStdString().c_str(), nDirUSbound);

    QString path = qsOutputPath + "/" + qsFloodName + "/GTIFF/DEM_" + QString::number(nCurrentIteration+1) + ".tif";

    file.setFileName(path);
    if (file.exists())
    {
        file.remove();
    }
    QFile::copy(qsNewDemPath, path);
    QFile::remove(qsOldDemPath);
    QFile::copy(qsNewDemPath, qsOldDemPath);


    //qDebug()<<"deposition "<<counterDepoEvent<< counterDepoTotal;
    //qDebug()<<"deposition "<<counterErodEvent<< counterErodTotal;
}

void MORPH_SedimentTransport::loadRasters()
{
    setPaths();

    MORPH_Raster Raster;

    QString qsXyzLocation, qsXvel, qsYvel, qsShear, qsDepth;

    qsXyzLocation = qsInputPath + "/" + qsFloodName + "/Hydraulics";

    qsXvel = qsXyzLocation + "/Xvelocity.xyz";
    qsYvel = qsXyzLocation + "/Yvelocity.xyz";
    qsShear = qsXyzLocation + "/BedShearStress.xyz";
    qsDepth = qsXyzLocation + "/WaterDepth.xyz";

    Raster.fromXYZ(qsXvelPath.toStdString().c_str(), qsXvel.toStdString().c_str(), nCols, nRows, noData, transform, 1);
    Raster.fromXYZ(qsYvelPath.toStdString().c_str(), qsYvel.toStdString().c_str(), nCols, nRows, noData, transform, 1);
    Raster.fromXYZ(qsShearPath.toStdString().c_str(), qsShear.toStdString().c_str(), nCols, nRows, noData, transform, 1);
    Raster.fromXYZ(qsDepthPath.toStdString().c_str(), qsDepth.toStdString().c_str(), nCols, nRows, noData, transform, 1);

    QString path = qsOutputPath + "/" + qsFloodName + "/GTIFF/Shear" + QString::number(nCurrentIteration+1) + ".tif";
    QFile file(path);
    if (file.exists())
    {
        file.remove();
    }
    QFile::copy(qsShearPath, path);
    path = qsOutputPath + "/" + qsFloodName + "/GTIFF/Depth" + QString::number(nCurrentIteration+1) + ".tif";
    file.setFileName(path);
    if (file.exists())
    {
        file.remove();
    }
    QFile::copy(qsDepthPath, path);

    Raster.flowDirection(qsXvelPath.toStdString().c_str(), qsYvelPath.toStdString().c_str(), qsFdirPath.toStdString().c_str());

    path = qsOutputPath + "/" + qsFloodName + "/GTIFF/FlowDir" + QString::number(nCurrentIteration+1) + ".tif";
    file.setFileName(path);
    if (file.exists())
    {
        file.remove();
    }
    QFile::copy(qsFdirPath, path);

    GDALDataset *pErode, *pDepo;

    pDriverTIFF = GetGDALDriverManager()->GetDriverByName("GTiff");

    pErode = pDriverTIFF->Create(qsErodePath.toStdString().c_str(), nCols, nRows, 1, GDT_Float32, NULL);
    pDepo = pDriverTIFF->Create(qsDepoPath.toStdString().c_str(), nCols, nRows, 1, GDT_Float32, NULL);

    pErode->SetGeoTransform(transform);
    pErode->GetRasterBand(1)->SetNoDataValue(noData);
    pDepo->SetGeoTransform(transform);
    pDepo->GetRasterBand(1)->SetNoDataValue(noData);
    pErode->GetRasterBand(1)->Fill(0.0);
    pDepo->GetRasterBand(1)->Fill(0.0);

    GDALClose(pErode);
    GDALClose(pDepo);
}

void MORPH_SedimentTransport::makeRegionInput()
{
    GDALDataset *pInter;

    openSlopeRaster();

    pDriverTIFF = GetGDALDriverManager()->GetDriverByName("GTiff");
    pInter = pDriverTIFF->Create(qsInterRegPath.toStdString().c_str(), nCols, nRows, 1, GDT_Float32, NULL);
    pInter->GetRasterBand(1)->Fill(noData);
    pInter->GetRasterBand(1)->SetNoDataValue(noData);
    pInter->SetGeoTransform(transform);

    float *oldRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *newRow = (float*) CPLMalloc(sizeof(float)*nCols);

    for (int i=0; i<nRows; i++)
    {
        pSlopeRaster->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, oldRow, nCols, 1, GDT_Float32, 0, 0);

        for (int j=0; j<nCols; j++)
        {
            if (oldRow[j] > 5.0 && oldRow[j] <= 90.0)
            {
                newRow[j] = 0.0;
            }
            else
            {
                newRow[j] = noData;
            }
        }

        pInter->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, newRow, nCols, 1, GDT_Float32, 0, 0);
    }

    GDALClose(pSlopeRaster);
    GDALClose(pInter);

    CPLFree(oldRow);
    CPLFree(newRow);
}

void MORPH_SedimentTransport::recalcRegionInput()
{
    GDALDataset *pRegions, *pInter;

    pRegions = (GDALDataset*) GDALOpen(qsRegionsPath.toStdString().c_str(), GA_ReadOnly);
    pInter = pDriverTIFF->CreateCopy(qsInterRegPath.toStdString().c_str(), pRegions, FALSE, NULL, NULL, NULL);

    float *oldRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *newRow = (float*) CPLMalloc(sizeof(float)*nCols);

    for (int i=0; i<nRows; i++)
    {
        pRegions->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, oldRow, nCols, 1, GDT_Float32, 0, 0);

        for (int j=0; j<nCols; j++)
        {
            if(oldRow[j] >= 1.0)
            {
                newRow[j] = 0.0;
            }
            else
            {
                newRow[j] = noData;
            }
        }

        pInter->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, newRow, nCols, 1, GDT_Float32, 0, 0);
    }

    GDALClose(pRegions);
    GDALClose(pInter);

    CPLFree(oldRow);
    CPLFree(newRow);
}

void MORPH_SedimentTransport::runBankErode()
{
    MORPH_Raster Raster;

    clearErosion();

    Raster.slopeTOF(qsNewDemPath.toStdString().c_str(), qsSlopePath.toStdString().c_str());
    Raster.filterLowPass(qsSlopePath.toStdString().c_str(), qsSlopeFiltPath.toStdString().c_str());
    Raster.aspect(qsNewDemPath.toStdString().c_str(), qsAspectPath.toStdString().c_str());

    int nRegions;

    makeRegionInput();
    nRegions = Raster.regions(qsInterRegPath.toStdString().c_str(), qsRegionsPath.toStdString().c_str());
    eliminateRegionsArea(nRegions);
    recalcRegionInput();
    nRegions = Raster.regions(qsInterRegPath.toStdString().c_str(), qsRegionsPath.toStdString().c_str());
    calcBankShear();
    eliminateRegionsValue(nRegions);
    recalcRegionInput();
    nRegions = Raster.regions(qsInterRegPath.toStdString().c_str(), qsRegionsPath.toStdString().c_str());
    Raster.filterLowPass(qsRegShearPath.toStdString().c_str(), qsBankShear.toStdString().c_str());
    calcLateralRetreat();
    erodeBanks();

    Raster.subtract(qsNewDemPath.toStdString().c_str(), qsOldDemPath.toStdString().c_str(), qsBankErode.toStdString().c_str());
    GDALDataset *pTemp, *pBankErode;
    pBankErode = (GDALDataset*) GDALOpen(qsBankErode.toStdString().c_str(), GA_ReadOnly);
    QString path = qsOutputPath + "/" + qsFloodName + "/GTIFF/BankErosion" + QString::number(nCurrentIteration+1) + ".tif";
    pTemp = pDriverTIFF->CreateCopy(path.toStdString().c_str(), pBankErode, FALSE, NULL, NULL, NULL);

    GDALClose(pBankErode);
    GDALClose(pTemp);

    //double sum;
    //sum = Raster.sum(qsBankErode.toStdString().c_str());
    //counterErodEvent += sum;
    //counterErodTotal += sum;
    //clearErosion();

    Raster.copyBoundary(qsNewDemPath.toStdString().c_str(), nDirDSbound);
    Raster.copyBoundary(qsNewDemPath.toStdString().c_str(), nDirDSbound);

    printDelftDem();
}

void MORPH_SedimentTransport::runBedErode()
{
    pathLength1.setupDistribution(plDistLength1, sigA1, muB1, nPlDistType1, cellWidth);

    //create object for raster operations
    MORPH_Raster Raster;

    //deposit sediment from bank erosoin
    QString path = qsOutputPath + "/" + qsFloodName + "/GTIFF/BankErosion" + QString::number(nCurrentIteration+1) + ".tif";
    runDeposition(path.toStdString().c_str());

    openDepositionRaster();
    GDALDataset *pTemp;

    path = qsOutputPath + "/" + qsFloodName + "/GTIFF/BankDeposition" + QString::number(nCurrentIteration+1) + ".tif";
    pDriverTIFF = GetGDALDriverManager()->GetDriverByName("GTiff");
    pTemp = pDriverTIFF->CreateCopy(path.toStdString().c_str(), pDepositRaster, FALSE, NULL, NULL, NULL);

    GDALClose(pDepositRaster);
    GDALClose(pTemp);

    clearDeposition();
    clearErosion();

    //calculate slope from the starting DEM
    Raster.slopeTOF(qsNewDemPath.toStdString().c_str(), qsSlopePath.toStdString().c_str());

    //open necessary datasets
    openShearRaster();
    openFdirRaster();
    openErosionRaster();
    openSlopeRaster();
    openOldDem();

    //declare varibles
    double averagedShear, shieldsStress, shearCrit, sedimentEroded, test, test2;

    //allocate memory to reading/writing datasets
    float *shrVal = (float*) CPLMalloc(sizeof(float)*1);
    float *slpVal = (float*) CPLMalloc(sizeof(float)*1);

    for (int i=2; i<nRows-2; i++)
    {
        for (int j=2; j<nCols-2; j++)
        {
            pShearRaster->GetRasterBand(1)->RasterIO(GF_Read, j, i, 1, 1, shrVal, 1, 1, GDT_Float32, 0, 0);

            //if shear contains a value continue
            if (*shrVal != noData)
            {
                //average shear stress along a flowline upstream and downstream of current cell
                averagedShear = averageShear_FlowLine(i, j, 10, 10);

                //calculate random value between 0.030 and 0.070 for shields stress
                shieldsStress = (qrand() % (70 - 30)) + 30;
                shieldsStress /= 1000.0;

                //this line makes shields stress constant when not commented
                //shieldsStress = 0.05;

                //recalculate shields stress based on slope
                pSlopeRaster->GetRasterBand(1)->RasterIO(GF_Read, j, i, 1, 1, slpVal, 1, 1, GDT_Float32, 0, 0);
                shieldsStress = shieldsStress * sqrt(1.0 - pow(sin(*slpVal), 2.0));

                //calculate the critical shear stress needed for erosion
                shearCrit = shieldsStress * (RHO_S - RHO) * G * (gsActiveSize);

                //if averaged shear exceeds critical shear find the erosion depth
                if (averagedShear >= shearCrit)
                {
                    sedimentEroded = erodeBedFlow(shearCrit, averagedShear, i, j);
                }
            }
        }
    }

    //close opened datasets
    GDALClose(pShearRaster);
    GDALClose(pFdirRaster);
    GDALClose(pErodeRaster);
    GDALClose(pSlopeRaster);
    GDALClose(pOldDem);

    //free allocated memory
    CPLFree(shrVal);
    CPLFree(slpVal);
    qDebug()<<"erosion loop done";

    //filter the erosion raster
    Raster.filterLowPass(qsErodePath.toStdString().c_str(), qsErodeFilterPath.toStdString().c_str());

    GDALDataset *pErodeFilter;

    pErodeFilter = (GDALDataset*) GDALOpen(qsErodeFilterPath.toStdString().c_str(), GA_ReadOnly);
    path = qsOutputPath + "/" + qsFloodName + "/GTIFF/BedErosion" + QString::number(nCurrentIteration+1) + ".tif";
    pTemp = pDriverTIFF->CreateCopy(path.toStdString().c_str(), pErodeFilter, FALSE, NULL, NULL, NULL);

    GDALClose(pErodeFilter);
    GDALClose(pTemp);

    //subtract the filtered erosoin from the new DEM
    //Raster.subtract(qsNewDemPath.toStdString().c_str(), qsErodeFilterPath.toStdString().c_str());
    //counterErodEvent += sum;
    //counterErodTotal += sum;
    clearErosion();

    //deposit eroded sediment
    runDeposition(qsErodeFilterPath.toStdString().c_str());
    //Raster.add(qsNewDemPath.toStdString().c_str(), qsDepoPath.toStdString().c_str());

    openDepositionRaster();

    path = qsOutputPath + "/" + qsFloodName + "/GTIFF/BedDeposition" + QString::number(nCurrentIteration+1) + ".tif";
    pTemp = pDriverTIFF->CreateCopy(path.toStdString().c_str(), pDepositRaster, FALSE, NULL, NULL, NULL);

    GDALClose(pTemp);
    GDALClose(pDepositRaster);

    clearDeposition();

    qDebug()<<"FINAL exported "<<exported;
    qDebug()<<"deposition "<<counterDepoEvent<< counterDepoTotal;
    qDebug()<<"erosion "<<counterErodEvent<< counterErodTotal;
}

void MORPH_SedimentTransport::setImportCells(QVector<int> rows, QVector<int> cols)
{
    qvImportRow = rows;
    qvImportCol = cols;
}

void MORPH_SedimentTransport::sloughBanks()
{
    //loadDrivers();
    qDebug()<<"starting slough";
    bool stop = false;
    int iterCount = 0, changedCount, lowCount, highCount;
    double amount, tempAmt;

    QFile::remove(qsSlopePath);

    MORPH_Raster Raster;
    Raster.slopeTOF(qsNewDemPath.toStdString().c_str(), qsSlopePath.toStdString().c_str());

    pSlopeRaster = (GDALDataset*) GDALOpen(qsSlopePath.toStdString().c_str(), GA_Update);
    openNewDem();

    float *demVals = (float*) CPLMalloc(sizeof(float)*9);
    float *slpVals = (float*) CPLMalloc(sizeof(float)*9);
    float *slpVal = (float*) CPLMalloc(sizeof(float)*1);
    float *newVals = (float*) CPLMalloc(sizeof(float)*9);
    qDebug()<<"slough loop";

    while (!stop && iterCount < 50)
    {
        changedCount = 0;

        for (int i=2; i<nRows-2; i++)
        {
            for (int j=2; j<nCols-2; j++)
            {
                pSlopeRaster->GetRasterBand(1)->RasterIO(GF_Read, j, i, 1, 1, slpVal, 1, 1, GDT_Float32, 0, 0);

                if (*slpVal>maxSlope && *slpVal!=noData)
                {
                    //qDebug()<<"doing slough "<<*slpVal<< changedCount;
                    changedCount++;
                    pNewDem->GetRasterBand(1)->RasterIO(GF_Read, j-1, i-1, 3, 3, demVals, 3, 3, GDT_Float32, 0, 0);
                    lowCount = 0, highCount = 0;
                    amount = 0.0, tempAmt = 0.0;

                    for (int k=0; k<9; k++)
                    {
                        if (demVals[k]!=noData && demVals[k]>demVals[4])
                        {
                            tempAmt = ((demVals[k] - demVals[4]) * 0.08);
                            amount = amount + tempAmt;
                            highCount++;
                        }
                        else if (demVals[k]<demVals[4] && demVals[k]!=noData)
                        {
                            lowCount++;
                        }
                    }

                    for (int k=0; k<9; k++)
                    {
                        if (demVals[k]!=noData && demVals[k]<demVals[4])
                        {
                            newVals[k] = demVals[k] + (amount/(lowCount*1.0));
                        }
                        else if (demVals[k]!=noData && demVals[k]>demVals[4])
                        {
                            newVals[k] = demVals[k] - ((demVals[k] - demVals[4]) * 0.08);
                        }
                        else
                        {
                            newVals[k] = demVals[k];
                        }
                    }

                    pNewDem->GetRasterBand(1)->RasterIO(GF_Write, j-1, i-1, 3, 3, newVals, 3, 3, GDT_Float32, 0, 0);

                    for (int k=0; k<9; k++)
                    {
                        int row, col;
                        row = i + ROW_OFFSET[k];
                        col = j + COL_OFFSET[k];

                        pNewDem->GetRasterBand(1)->RasterIO(GF_Read, col-1, row-1, 3, 3, newVals, 3, 3, GDT_Float32, 0, 0);

                        double xslope, yslope, xyslope, xypow;

                        if (newVals[4] == noData || newVals[0] == noData || newVals[1] == noData || newVals[2] == noData || newVals[3] == noData || newVals[5] == noData || newVals[6] == noData || newVals[7] == noData || newVals[8] == noData)
                        {
                            slpVals[j] = noData;
                        }
                        else
                        {
                            xslope = ((newVals[2]-newVals[0]) + ((2*newVals[5])-(2*newVals[3])) + (newVals[8]-newVals[6])) / (8*transform[1]);
                            yslope = ((newVals[0]-newVals[6]) + ((2*newVals[1])-(2*newVals[7])) + (newVals[2]-newVals[8]))/(8*transform[1]);
                            xyslope = pow(xslope,2.0) + pow(yslope,2.0);
                            xypow = pow(xyslope,0.5);
                            slpVals[k] = (atan(xypow)*180.0/PI);
                        }
                    }

                    pSlopeRaster->GetRasterBand(1)->RasterIO(GF_Write, j-1, i-1, 3, 3, slpVals, 3, 3, GDT_Float32, 0, 0);
                }
            }
        }

        if (changedCount == 0)
        {
            stop = true;
        }

        iterCount++;
        qDebug()<<"slough iteration done "<<iterCount<<" changed "<<changedCount<<" max slp "<<maxSlope;
    }


    GDALClose(pSlopeRaster);
    qDebug()<<"slope closed";
    GDALClose(pNewDem);
    qDebug()<<"dem closed";

    CPLFree(demVals);
    CPLFree(slpVals);
    CPLFree(slpVal);
    CPLFree(newVals);

    qDebug()<<"slough finished";
}

void MORPH_SedimentTransport::transportSediment()
{
    //use path length to identify cells receiving deposition
    qvDepoRow.clear();
    qvDepoCol.clear();
    qvDepoAmt.clear();


    for (int i=0; i<qvSeedRow.size(); i++)
    {
        findDepositionCells(qvSeedRow[i], qvSeedCol[i], qvSeedAmt[i]);
    }

    addDeposition();

    MORPH_Raster Raster;

    Raster.add(qsNewDemPath.toStdString().c_str(), qsDepoPath.toStdString().c_str());
}

double MORPH_SedimentTransport::averageShear_FlowLine(int row, int col, int cellsUS, int cellsDS)
{
    //declare varibles to hold location data
    double xCoord, yCoord;
    QVector<double> qvResult(4);
    QVector<int> qvRow, qvCol;

    //calculate geographic coordinates of current cell
    xCoord = transform[0] + (col*cellWidth);
    yCoord = transform[3] - (row*cellWidth);
    qvResult[0] = row, qvResult[1] = col, qvResult[2] = xCoord, qvResult[3] = yCoord;
    qvRow.append(qvResult[0]), qvCol.append(qvResult[1]);

    //variables for averageing
    int count = 0;
    double total = 0.0;
    double average;
    float *sVal = (float*) CPLMalloc(sizeof(float)*1);

    //find values for downstream cells
    for (int i=1; i<cellsDS+1; i++)
    {
        //find next downstream cell
        qvResult = findNextCell(qvResult[2], qvResult[3], qvResult[0], qvResult[1]);

        //make sure new cell is in a valid location
        if (qvResult[0] < nRows-2 && qvResult[0] > 1 && qvResult[1] < nCols-2 && qvResult[1] > 1)
        {
            //read shear value at new location
            pShearRaster->GetRasterBand(1)->RasterIO(GF_Read,qvResult[1],qvResult[0],1,1,sVal,1,1,GDT_Float32,0,0);

            if (*sVal != 0.0 && *sVal != noData)
            {
                qvRow.append(qvResult[0]), qvCol.append(qvResult[1]);
                total += fabs(*sVal);
                count++;
            }
        }
    }

    //reset variables
    qvResult[0] = row, qvResult[1] = col, qvResult[2] = xCoord, qvResult[3] = yCoord;

    //find values for upstream cells
    for (int i=1; i<cellsUS+1; i++)
    {

        qvResult = findNextCell_Backward(qvResult[2], qvResult[3], qvResult[0], qvResult[1]);

        //make sure new cell is in a valid location
        if (qvResult[0] < nRows-2 && qvResult[0] > 1 && qvResult[1] < nCols-2 && qvResult[1] > 1)
        {
            //read shear value at new location
            pShearRaster->GetRasterBand(1)->RasterIO(GF_Read,qvResult[1],qvResult[0],1,1,sVal,1,1,GDT_Float32,0,0);

            if (*sVal != 0.0 && *sVal != noData)
            {
                qvRow.append(qvResult[0]), qvCol.append(qvResult[1]);
                total += fabs(*sVal);
                count++;
            }
        }
        else
        {
            //break;
        }
    }

    //calculate average
    average = total / (count*1.0);

    CPLFree(sVal);

    return average;
}

void MORPH_SedimentTransport::clearDeposition()
{
    openDepositionRaster();
    pDepositRaster->GetRasterBand(1)->Fill(0.0);
    GDALClose(pDepositRaster);
}

void MORPH_SedimentTransport::clearErosion()
{
    openErosionRaster();
    pErodeRaster->GetRasterBand(1)->Fill(0.0);
    GDALClose(pErodeRaster);
}

void MORPH_SedimentTransport::createDoD()
{
    GDALDataset *pDemOfDiff;
    QString bankErode, bedErode, bankDepo, bedDepo, dodPath, importDepo;

    bankErode = qsOutputPath + "/" + qsFloodName + "/GTIFF/BankErosion" + QString::number(nCurrentIteration+1) + ".tif";
    bedErode = qsOutputPath + "/" + qsFloodName + "/GTIFF/BedErosion" + QString::number(nCurrentIteration+1) + ".tif";
    bankDepo = qsOutputPath + "/" + qsFloodName + "/GTIFF/BankDeposition" + QString::number(nCurrentIteration+1) + ".tif";
    bedDepo = qsOutputPath + "/" + qsFloodName + "/GTIFF/BedDeposition" + QString::number(nCurrentIteration+1) + ".tif";
    dodPath = qsOutputPath + "/" + qsFloodName + "/GTIFF/DoD_" + QString::number(nCurrentIteration+1) + "_temp.tif";
    importDepo = qsOutputPath + "/" + qsFloodName + "/GTIFF/ImportDepo" + QString::number(nCurrentIteration+1) + ".tif";

    pDemOfDiff = pDriverTIFF->Create(dodPath.toStdString().c_str(), nCols, nRows, 1, GDT_Float32, NULL);
    pDemOfDiff->SetGeoTransform(transform);
    pDemOfDiff->GetRasterBand(1)->SetNoDataValue(noData);
    pDemOfDiff->GetRasterBand(1)->Fill(0.0);

    GDALClose(pDemOfDiff);

    MORPH_Raster Raster;

    Raster.subtract(dodPath.toStdString().c_str(), bedErode.toStdString().c_str());
    Raster.add(dodPath.toStdString().c_str(), bankDepo.toStdString().c_str());
    Raster.add(dodPath.toStdString().c_str(), bedDepo.toStdString().c_str());
    Raster.add(dodPath.toStdString().c_str(), importDepo.toStdString().c_str());

    fixDeposition(dodPath);
}

double MORPH_SedimentTransport::erodeBedFlow(double shearCrit, double shear, int row, int col)
{
    //declare variables
    double sediment, bedload, ustar, ustarc, bedvel, xCoord, yCoord, tempsed;
    bool erode = true;
    QVector<double> qvErode, qvResult(4);
    QVector<int> qvRow, qvCol;

    //allocate memory
    float *depVal = (float*) CPLMalloc(sizeof(float)*1);
    float *shrVal = (float*) CPLMalloc(sizeof(float)*1);
    float *nodVal = (float*) CPLMalloc(sizeof(float)*1);
    float *oldVal = (float*) CPLMalloc(sizeof(float)*1);
    float *newVal = (float*) CPLMalloc(sizeof(float)*1);

    //calculate amount of sediment to erode
    bedload = pow(shear-shearCrit, 1.5);
    ustar = sqrt(shear / RHO);
    ustarc = sqrt(shearCrit / RHO);
    bedvel = A * (ustar-ustarc);
    sediment = erosionFactor * (bedload / (bedvel * RHO_S * (1.0 - POROSITY)));

    //setup location variables
    xCoord = transform[0] + (col * cellWidth);
    yCoord = transform[3] - (row * cellWidth);
    qvResult[0] = row;
    qvResult[1] = col;
    qvResult[2] = xCoord;
    qvResult[3] = yCoord;
    qvRow.append(qvResult[0]), qvCol.append(qvResult[1]), qvErode.append(sediment);

    //find downstream cells to erode
    for (int i=1; i<11; i++)
    {
        qvResult = findNextCell(qvResult[2], qvResult[3], qvResult[0], qvResult[1]);

        if (qvResult[0] < nRows-2 && qvResult[0] > 1 && qvResult[1] < nCols-2 && qvResult[1] > 1)
        {
            pShearRaster->GetRasterBand(1)->RasterIO(GF_Read,qvResult[1],qvResult[0],1,1,shrVal,1,1,GDT_Float32,0,0);

            if (fabs(*shrVal) > 1.0)
            {
                qvRow.append(qvResult[0]), qvCol.append(qvResult[1]);
            }
        }

        pOldDem->GetRasterBand(1)->RasterIO(GF_Read,qvResult[1],qvResult[0],1,1,nodVal,1,1,GDT_Float32,0,0);

        if (*nodVal == noData)
        {
            erode = false;
        }
    }

    //reset locatoin variables
    qvResult[0] = row;
    qvResult[1] = col;
    qvResult[2] = xCoord;
    qvResult[3] = yCoord;

    //find upstream cells to erode
    for (int i=1; i<11; i++)
    {
        qvResult = findNextCell_Backward(qvResult[2], qvResult[3], qvResult[0], qvResult[1]);

        if (qvResult[0] < nRows-2 && qvResult[0] > 1 && qvResult[1] < nCols-2 && qvResult[1] > 1)
        {
            pShearRaster->GetRasterBand(1)->RasterIO(GF_Read,qvResult[1],qvResult[0],1,1,shrVal,1,1,GDT_Float32,0,0);

            if (fabs(*shrVal) > 1.0)
            {
                qvRow.append(qvResult[0]), qvCol.append(qvResult[1]);
            }
        }
        pOldDem->GetRasterBand(1)->RasterIO(GF_Read,qvResult[1],qvResult[0],1,1,nodVal,1,1,GDT_Float32,0,0);
        if (*nodVal == noData)
        {
            erode = false;
        }
    }

    //divide amount of sediment by number of cells it's being distributed to
    tempsed = sediment/(qvRow.size()*1.0);

    if (erode)
    {
        for (int i=0; i<qvRow.size(); i++)
        {
            //update new dem with erosion
            *newVal = tempsed;

            pErodeRaster->GetRasterBand(1)->RasterIO(GF_Read,qvCol[i],qvRow[i],1,1,oldVal,1,1,GDT_Float32,0,0);
            *newVal += *oldVal;
            pErodeRaster->GetRasterBand(1)->RasterIO(GF_Write,qvCol[i],qvRow[i],1,1,newVal,1,1,GDT_Float32,0,0);
        }
    }

    //clear vectors
    qvCol.clear();
    qvRow.clear();
    qvErode.clear();

    //free memory
    CPLFree(depVal);
    CPLFree(shrVal);
    CPLFree(nodVal);
    CPLFree(oldVal);
    CPLFree(newVal);

    return sediment;
}

void MORPH_SedimentTransport::findImportCells()
{
    GDALAllRegister();
    clearErosion();

    qvImportCol.clear();
    qvImportRow.clear();

    int row = 0, col = 0;
    double imported;

    float *fdirVal = (float*) CPLMalloc(sizeof(float)*1);

    openFdirRaster();

    if (nImportType == 1)
    {
        imported = import[nCurrentIteration];
    }
    else if (nImportType == 2)
    {
        imported = exported * import[nCurrentIteration];
    }

    exported = 0.0;

    if (nDirUSbound == 1)
    {
        row = 2;

        for (int i=0; i<nCols; i++)
        {
            pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read, i, row, 1, 1, fdirVal, 1, 1, GDT_Float32, 0, 0);

            if (*fdirVal > 0.0)
            {
                qvImportCol.append(i);
                qvImportRow.append(row);
            }
        }
    }
    else if (nDirUSbound == 2)
    {
        row = nRows - 3;

        for (int i=0; i<nCols; i++)
        {
            pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read, i, row, 1, 1, fdirVal, 1, 1, GDT_Float32, 0, 0);

            if (*fdirVal > 0.0)
            {
                qvImportCol.append(i);
                qvImportRow.append(row);
            }
        }
    }
    else if (nDirUSbound == 3)
    {
        col = nCols - 3;
    }
    else if (nDirUSbound == 4)
    {
        col = 2;
    }
    else
    {

    }

    GDALClose(pFdirRaster);

    openErosionRaster();

    *fdirVal = (imported / (qvImportRow.size()*1.0));

    for (int i=0; i<qvImportRow.size(); i++)
    {
        pErodeRaster->GetRasterBand(1)->RasterIO(GF_Write, qvImportCol[i], qvImportRow[i], 1, 1, fdirVal, 1, 1, GDT_Float32, 0, 0);
    }

    GDALClose(pErodeRaster);

    CPLFree(fdirVal);

    qDebug()<<"running import deposition";
    runDeposition(qsErodePath.toStdString().c_str());
}

QVector<double> MORPH_SedimentTransport::findNextCell(double startX, double startY, int prevRow, int prevCol)
{
    int tempRow, tempCol, startRow, startCol;
    QVector<double> address(4);
    double activeDirection, yAdd, xAdd, newX, newY, oldX, oldY;

    oldX = startX;
    oldY = startY;
    startRow = round((transform[3] - oldY)/cellWidth);
    startCol = round((oldX - transform[0])/cellWidth);

    float *fdir = (float*) CPLMalloc(sizeof(float)*1);

    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,startCol,startRow,1,1,fdir,1,1,GDT_Float32,0,0);

    activeDirection = *fdir;

    //convert angle from geographic degrees to arithmetic degrees
    if (activeDirection > 0 && activeDirection <= 90.0)
    {
        activeDirection = 90.0 - activeDirection;
    }
    else if (activeDirection > 90.0 && activeDirection <= 180.0)
    {
        activeDirection = 270 + (180.0 - activeDirection);
    }
    else if (activeDirection > 180.0 && activeDirection <= 270.0)
    {
        activeDirection = 180.0 + (270.0 - activeDirection);
    }
    else if (activeDirection > 270.0 && activeDirection <= 360.0)
    {
        activeDirection = 90.0 + (360.0 - activeDirection);
    }
    else
    {

    }

    activeDirection *= (PI/180.0);

    yAdd = cellWidth * sin(activeDirection);
    xAdd = cellWidth * cos(activeDirection);

    newX = oldX + xAdd;
    newY = oldY + yAdd;

    tempRow = round((transform[3] - newY)/cellWidth);
    tempCol = round((newX - transform[0])/cellWidth);

    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdir,1,1,GDT_Float32,0,0);

    if (*fdir > 0.0)
    {
        address[0] = tempRow;
        address[1] = tempCol;
        address[2] = newX;
        address[3] = newY;
    }
    else
    {
        QVector<int> nvAddress;
        nvAddress = findNextCell_D8(startRow, startCol, prevRow, prevCol);
        address[0] = nvAddress[0];
        address[1] = nvAddress[1];
        address[2] = transform[0] + (nvAddress[1]*cellWidth);
        address[3] = transform[3] - (nvAddress[0]*cellWidth);
    }

    CPLFree(fdir);

    return address;
}

QVector<double> MORPH_SedimentTransport::findNextCell_Backward(double startX, double startY, int prevRow, int prevCol)
{
    int tempRow, tempCol, startRow, startCol;
    QVector<double> address(4);
    double activeDirection, yAdd, xAdd, newX, newY, oldX, oldY;

    float *fdir = (float*) CPLMalloc(sizeof(float)*1);

    oldX = startX;
    oldY = startY;
    startRow = round((transform[3] - oldY)/cellWidth);
    startCol = round((oldX - transform[0])/cellWidth);

    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,startCol,startRow,1,1,fdir,1,1,GDT_Float32,0,0);

    activeDirection = *fdir;

    if (activeDirection > 360.0 && activeDirection < 361.0)
    {
        activeDirection = 359.9;
    }

    if (activeDirection <= 180.0)
    {
        activeDirection += 180.0;
    }
    else if(activeDirection > 180.0 && activeDirection <= 360.0)
    {
        activeDirection -= 180.0;
    }
    else
    {
        qDebug()<<"backward cell id problem: direction out of range "<<activeDirection;
    }

    //convert angle from geographic degrees to arithmetic degrees
    if (activeDirection > 0 && activeDirection <= 90.0)
    {
        activeDirection = 90.0 - activeDirection;
    }
    else if (activeDirection > 90.0 && activeDirection <= 180.0)
    {
        activeDirection = 270 + (180.0 - activeDirection);
    }
    else if (activeDirection > 180.0 && activeDirection <= 270.0)
    {
        activeDirection = 180.0 + (270.0 - activeDirection);
    }
    else if (activeDirection > 270.0 && activeDirection <= 360.0)
    {
        activeDirection = 90.0 + (360.0 - activeDirection);
    }

    activeDirection *= (PI/180.0);

    yAdd = cellWidth * sin(activeDirection);
    xAdd = cellWidth * cos(activeDirection);

    newX = oldX + xAdd;
    newY = oldY + yAdd;

    tempRow = round((transform[3] - newY)/cellWidth);
    tempCol = round((newX - transform[0])/cellWidth);

    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdir,1,1,GDT_Float32,0,0);

    if (*fdir > 0.0)
    {
        address[0] = tempRow;
        address[1] = tempCol;
        address[2] = newX;
        address[3] = newY;
    }
    else
    {
        QVector<int> nvAddress;
        nvAddress = findNextCell_D8(startRow, startCol, prevRow, prevCol);
        address[0] = nvAddress[0];
        address[1] = nvAddress[1];
        address[2] = transform[0] + (nvAddress[1]*cellWidth);
        address[3] = transform[3] - (nvAddress[0]*cellWidth);
    }

    CPLFree(fdir);

    return address;
}

QVector<int> MORPH_SedimentTransport::findNextCell_D8(int startRow, int startCol, int prevRow, int prevCol)
{
    int tempRow = startRow, tempCol = startCol;
    QVector<int> address(2);
    double activeDirection;

    float *fdirValue = (float*) CPLMalloc(sizeof(float)*1);

    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,startCol,startRow,1,1,fdirValue,1,1,GDT_Float32,0,0);

    activeDirection = *fdirValue;

    if (activeDirection>=67.5 && activeDirection<112.5)
    {
        tempCol++;
        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
        if(*fdirValue <= 0.0)
        {
            if(activeDirection > 0.0)
            {
                tempRow--;
                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                {
                    tempRow+=2;
                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                    {
                        tempRow-=2;
                        tempCol--;
                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                        {
                            tempRow+=2;
                            pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                            if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                            {
                                tempRow-=2;
                                tempCol--;
                                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                {
                                    tempRow+=2;
                                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                    {
                                        tempRow--;
                                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                        {
                                            tempRow = startRow;
                                            tempCol = startCol;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                tempRow++;
                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                {
                    tempRow-=2;
                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                    {
                        tempRow+=2;
                        tempCol--;
                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                        {
                            tempRow-=2;
                            pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                            if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                            {
                                tempRow+=2;
                                tempCol--;
                                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                {
                                    tempRow-=2;
                                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                    {
                                        tempRow++;
                                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                        {
                                            tempRow = startRow;
                                            tempCol = startCol;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        address[0] = tempRow;
        address[1] = tempCol;
    }
    else if ((activeDirection>0.0 && activeDirection<=22.5) || (activeDirection<=360.0 && activeDirection>=337.5))
    {
        tempRow--;
        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
        if(*fdirValue <= 0.0)
        {
            if(activeDirection > 90.0)
            {
                tempCol--;
                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                {
                    tempCol+=2;
                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                    {
                        tempCol-=2;
                        tempRow++;
                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                        {
                            tempCol+=2;
                            pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                            if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                            {
                                tempCol-=2;
                                tempRow++;
                                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                {
                                    tempCol+=2;
                                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                    {
                                        tempCol--;
                                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                        {
                                            tempRow = startRow;
                                            tempCol = startCol;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                tempCol++;
                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                {
                    tempCol-=2;
                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                    {
                        tempCol+=2;
                        tempRow++;
                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                        {
                            tempCol-=2;
                            pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                            if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                            {
                                tempCol+=2;
                                tempRow++;
                                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                {
                                    tempCol-=2;
                                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                    {
                                        tempCol++;
                                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                        {
                                            tempRow = startRow;
                                            tempCol = startCol;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        address[0] = tempRow;
        address[1] = tempCol;
    }
    else if (activeDirection>=247.5 && activeDirection<=292.5)
    {
        tempCol--;
        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
        if(*fdirValue <= 0.0)
        {
            if(activeDirection < 180.0)
            {
                tempRow--;
                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                {
                    tempRow+=2;
                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                    {
                        tempRow-=2;
                        tempCol++;
                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                        {
                            tempRow+=2;
                            pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                            if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                            {
                                tempRow-=2;
                                tempCol++;
                                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                {
                                    tempRow+=2;
                                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                    {
                                        tempRow--;
                                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                        {
                                            tempRow = startRow;
                                            tempCol = startCol;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                tempRow++;
                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                {
                    tempRow-=2;
                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                    {
                        tempRow+=2;
                        tempCol++;
                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                        {
                            tempRow-=2;
                            pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                            if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                            {
                                tempRow+=2;
                                tempCol++;
                                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                {
                                    tempRow-=2;
                                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                    {
                                        tempRow++;
                                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                        {
                                            tempRow = startRow;
                                            tempCol = startCol;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        address[0] = tempRow;
        address[1] = tempCol;
    }
    else if (activeDirection>=157.5 && activeDirection<=202.5)
    {
        tempRow++;
        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
        if(*fdirValue <= 0.0)
        {
            if(activeDirection < 270.0)
            {
                tempCol--;
                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                {
                    tempCol+=2;
                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                    {
                        tempCol-=2;
                        tempRow--;
                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                        {
                            tempCol+=2;
                            pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                            if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                            {
                                tempCol-=2;
                                tempRow--;
                                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                {
                                    tempCol+=2;
                                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                    {
                                        tempCol--;
                                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                        {
                                            tempRow = startRow;
                                            tempCol = startCol;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                tempCol++;
                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                {
                    tempCol-=2;
                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                    {
                        tempCol+=2;
                        tempRow--;
                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                        {
                            tempCol-=2;
                            pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                            if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                            {
                                tempCol+=2;
                                tempRow--;
                                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                {
                                    tempCol-=2;
                                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                    {
                                        tempCol++;
                                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                        {
                                            tempRow = startRow;
                                            tempCol = startCol;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        address[0] = tempRow;
        address[1] = tempCol;
    }
    else if (activeDirection>22.5 && activeDirection<67.5)
    {
        tempCol++;
        tempRow--;
        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
        if(*fdirValue <= 0.0)
        {
            if(activeDirection > 45.0)
            {
                tempCol--;
                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                {
                    tempCol++;
                    tempRow++;
                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                    {
                        tempRow--;
                        tempCol-=2;
                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                        {
                            tempRow+=2;
                            tempCol+=2;
                            pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                            if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                            {
                                tempRow--;
                                tempCol-=2;
                                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                {
                                    tempRow++;
                                    tempCol++;
                                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                    {
                                        tempCol--;
                                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                        {
                                            tempRow = startRow;
                                            tempCol = startCol;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                tempRow++;
                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                {
                    tempCol--;
                    tempRow--;
                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                    {
                        tempRow+=2;
                        tempCol++;
                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                        {
                            tempRow-=2;
                            tempCol-=2;
                            pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                            if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                            {
                                tempRow+=2;
                                tempCol++;
                                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                {
                                    tempRow--;
                                    tempCol--;
                                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                    {
                                        tempRow++;
                                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                        {
                                            tempRow = startRow;
                                            tempCol = startCol;
                                            pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        address[0] = tempRow;
        address[1] = tempCol;
    }
    else if (activeDirection>292.5 && activeDirection<337.5)
    {
        tempCol--;
        tempRow--;
        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
        if(*fdirValue <= 0.0)
        {
            if(activeDirection < 135.0)
            {
                tempCol++;
                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                {
                    tempCol--;
                    tempRow++;
                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                    {
                        tempRow--;
                        tempCol+=2;
                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                        {
                            tempRow+=2;
                            tempCol-=2;
                            pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                            if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                            {
                                tempRow--;
                                tempCol+=2;
                                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                {
                                    tempRow++;
                                    tempCol--;
                                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                    {
                                        tempCol++;
                                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                        {
                                            tempRow = startRow;
                                            tempCol = startCol;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                tempRow++;
                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                {
                    tempCol++;
                    tempRow--;
                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                    {
                        tempRow+=2;
                        tempCol--;
                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                        {
                            tempRow-=2;
                            tempCol+=2;
                            pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                            if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                            {
                                tempRow+=2;
                                tempCol--;
                                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                {
                                    tempRow--;
                                    tempCol++;
                                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                    {
                                        tempRow++;
                                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                        {
                                            tempRow = startRow;
                                            tempCol = startCol;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        address[0] = tempRow;
        address[1] = tempCol;
    }
    else if (activeDirection>202.5 && activeDirection<247.5)
    {
        tempCol--;
        tempRow++;
        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
        if(*fdirValue <= 0.0)
        {
            if(activeDirection < 225.0)
            {
                tempCol++;
                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                {
                    tempCol--;
                    tempRow--;
                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                    {
                        tempRow++;
                        tempCol+=2;
                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                        {
                            tempRow-=2;
                            tempCol-=2;
                            pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                            if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                            {
                                tempRow++;
                                tempCol+=2;
                                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                {
                                    tempRow--;
                                    tempCol--;
                                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                    {
                                        tempCol++;
                                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                        {
                                            tempRow = startRow;
                                            tempCol = startCol;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                tempRow--;
                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                {
                    tempCol++;
                    tempRow++;
                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                    {
                        tempRow-=2;
                        tempCol--;
                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                        {
                            tempRow+=2;
                            tempCol+=2;
                            pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                            if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                            {
                                tempRow-=2;
                                tempCol--;
                                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                {
                                    tempRow++;
                                    tempCol++;
                                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                    {
                                        tempRow--;
                                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                        {
                                            tempRow = startRow;
                                            tempCol = startCol;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        address[0] = tempRow;
        address[1] = tempCol;
    }
    else if (activeDirection>112.5 && activeDirection<157.5)
    {
        tempCol++;
        tempRow++;
        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
        if(*fdirValue <= 0.0)
        {
            if(activeDirection < 315.0)
            {
                tempCol--;
                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                {
                    tempCol++;
                    tempRow--;
                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                    {
                        tempRow++;
                        tempCol-=2;
                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                        {
                            tempRow-=2;
                            tempCol+=2;
                            pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                            if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                            {
                                tempRow++;
                                tempCol-=2;
                                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                {
                                    tempRow--;
                                    tempCol++;
                                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                    {
                                        tempCol--;
                                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                        {
                                            tempRow = startRow;
                                            tempCol = startCol;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                tempRow--;
                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                {
                    tempCol--;
                    tempRow++;
                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                    {
                        tempRow-=2;
                        tempCol++;
                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                        {
                            tempRow+=2;
                            tempCol-=2;
                            pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                            if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                            {
                                tempRow-=2;
                                tempCol++;
                                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                {
                                    tempRow++;
                                    tempCol--;
                                    pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                    if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                    {
                                        tempRow--;
                                        pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read,tempCol,tempRow,1,1,fdirValue,1,1,GDT_Float32,0,0);
                                        if(*fdirValue <= 0.0 || (prevRow == tempRow && prevCol == tempCol))
                                        {
                                            tempRow = startRow;
                                            tempCol = startCol;
                                        }
                                    }

                                }
                            }
                        }
                    }
                }
            }
        }
        address[0] = tempRow;
        address[1] = tempCol;
    }
    else if (activeDirection == 0)
    {
        //std::cout<<"Flow direction 0 at "<<startRow<<" "<<startCol<<std::endl;
    }
    else
    {
        qDebug()<<"Not sure what flow direction is, some problem here "<<*fdirValue;
    }

    CPLFree(fdirValue);

    return address;
}

void MORPH_SedimentTransport::fixDeposition(QString dodPath)
{
    openDepthRaster();

    GDALDataset *pDemOfDiff;
    pDemOfDiff = (GDALDataset*) GDALOpen(dodPath.toStdString().c_str(), GA_Update);

    float *dodRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *depRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *dodWin = (float*) CPLMalloc(sizeof(float)*25);
    float *depWin = (float*) CPLMalloc(sizeof(float)*25);
    float *newDodWin = (float*) CPLMalloc(sizeof(float)*25);

    int count = 0, changed = 1;
    double excess, addAmt;
    int addCells;

    while (count < 20 && changed > 0)
    {
        changed = 0;

        for (int i=0; i<nRows-2; i++)
        {
            pDemOfDiff->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, dodRow, nCols, 1, GDT_Float32, 0, 0);
            pDepthRaster->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, depRow, nCols, 1, GDT_Float32, 0, 0);

            for (int j=0; j<nCols-2; j++)
            {
                if (dodRow[j] != noData && depRow[j] != noData)
                {
                    if (dodRow[j] > depRow[j])
                    {
                        excess = dodRow[j] - depRow[j];

                        pDemOfDiff->GetRasterBand(1)->RasterIO(GF_Read, j-2, i-2, 5, 5, dodWin, 5, 5, GDT_Float32, 0, 0);
                        pDepthRaster->GetRasterBand(1)->RasterIO(GF_Read, j-2, i-2, 5, 5, depWin, 5, 5, GDT_Float32, 0, 0);

                        addCells = 0;

                        for (int k=0; k<25; k++)
                        {
                            if(depWin[k] > dodWin[k])
                            {
                                addCells++;
                            }
                        }

                        addAmt = excess / (addCells*1.0);

                        for (int k=0; k<25; k++)
                        {
                            if(depWin[k] > dodWin[k])
                            {
                                newDodWin[k] = dodWin[k] + addAmt;
                            }
                            else
                            {
                                newDodWin[k] = dodWin[k];
                            }
                        }

                        newDodWin[12] = dodWin[12] - excess;
                        pDemOfDiff->GetRasterBand(1)->RasterIO(GF_Write, j-2, i-2, 5, 5, newDodWin, 5, 5, GDT_Float32, 0, 0);

                        changed++;
                    }
                }
            }
        }
        count++;
    }


    GDALClose(pDepthRaster);
    GDALClose(pDemOfDiff);

    CPLFree(dodRow);
    CPLFree(depRow);
    CPLFree(dodWin);
    CPLFree(depWin);
    CPLFree(newDodWin);
}

void MORPH_SedimentTransport::openDepositionRaster()
{
    pDepositRaster = (GDALDataset*) GDALOpen(qsDepoPath.toStdString().c_str(), GA_Update);
}

void MORPH_SedimentTransport::openDepthRaster()
{
    pDepthRaster = (GDALDataset*) GDALOpen(qsDepthPath.toStdString().c_str(), GA_Update);
}

void MORPH_SedimentTransport::openErosionRaster()
{
    pErodeRaster = (GDALDataset*) GDALOpen(qsErodePath.toStdString().c_str(), GA_Update);
}

void MORPH_SedimentTransport::openFdirRaster()
{
    pFdirRaster = (GDALDataset*) GDALOpen(qsFdirPath.toStdString().c_str(), GA_ReadOnly);
}

void MORPH_SedimentTransport::openNewDem()
{
    pNewDem = (GDALDataset*) GDALOpen(qsNewDemPath.toStdString().c_str(), GA_Update);
}

void MORPH_SedimentTransport::openOldDem()
{
    pOldDem = (GDALDataset*) GDALOpen(qsOldDemPath.toStdString().c_str(), GA_ReadOnly);
}

void MORPH_SedimentTransport::openShearRaster()
{
    pShearRaster = (GDALDataset*) GDALOpen(qsShearPath.toStdString().c_str(), GA_ReadOnly);
}

void MORPH_SedimentTransport::openSlopeRaster()
{
    pSlopeRaster = (GDALDataset*) GDALOpen(qsSlopePath.toStdString().c_str(), GA_ReadOnly);
}

void MORPH_SedimentTransport::printDelftDem()
{
    GDALDataset *pTemp;

    openNewDem();

    pTemp = pDriverTIFF->CreateCopy(qsDelftDemPath.toStdString().c_str(), pNewDem, FALSE, NULL, NULL, NULL);

    GDALClose(pNewDem);
    GDALClose(pTemp);
}

void MORPH_SedimentTransport::setPaths()
{
    //QString qsRasterLocation;

    //qsRasterLocation = qsOutputPath + "/" + qsFloodName + "/GTIFF";

    qsShearPath = qsTempPath + "/BedShearStress.tif";
    qsDepthPath = qsTempPath + "/WaterDepth.tif";
    qsXvelPath = qsTempPath + "/Xvelocity.tif";
    qsYvelPath = qsTempPath + "/Yvelocity.tif";
    qsFdirPath = qsTempPath + "/FlowDirection.tif";
    qsDepoPath = qsTempPath + "/Deposition.tif";
    qsErodePath = qsTempPath + "/Erosion.tif";
    qsErodeFilterPath = qsTempPath + "/Erosion_filt.tif";
    qsSlopePath = qsTempPath + "/Slope.tif";
    qsSlopeFiltPath = qsTempPath + "/Slope_filt.tif";
    qsRegionsPath = qsTempPath + "/Regions.tif";
    qsBankErodeInput = qsTempPath + "/BankIn.tif";
    qsBankShear = qsTempPath + "/BankShear.tif";
    qsBankErode = qsTempPath + "/BankErode.tif";
    qsLateralErode = qsTempPath + "/LateralErode.tif";
    qsRegShearPath = qsTempPath + "/RegionShear.tif";
    qsInterRegPath = qsTempPath + "/BankIntermediate.tif";
    qsAspectPath = qsTempPath + "/Aspect.tif";
}

void MORPH_SedimentTransport::runDeposition(const char *erosionRasterPath)
{
    GDALDataset *pRaster;

    pRaster = (GDALDataset*) GDALOpen(erosionRasterPath, GA_ReadOnly);

    openFdirRaster();
    openDepthRaster();

    qvSeedRow.clear();
    qvSeedCol.clear();
    qvSeedAmt.clear();

    double activeDirection, cellSed;
    int wetCells;
    QVector<int> rows, cols;
    bool calc;

    float *erdRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *dirWin = (float*) CPLMalloc(sizeof(float)*9);

    for (int i=2; i<nRows-2; i++)
    {
        pRaster->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, erdRow, nCols, 1, GDT_Float32, 0, 0);

        for (int j=2; j<nCols-2; j++)
        {
            if (erdRow[j] != noData && erdRow[j] != 0.0)
            {
                pFdirRaster->GetRasterBand(1)->RasterIO(GF_Read, j-1, i-1, 3, 3, dirWin, 3, 3, GDT_Float32, 0, 0);

                activeDirection = dirWin[4];
                wetCells = 0;
                calc = true;

                if (activeDirection == 0.0)
                {
                    for (int k=0; k<9; k++)
                    {
                        if (dirWin[k] > 0.0)
                        {
                            wetCells++;
                            rows.append(i+ROW_OFFSET[k]);
                            cols.append(j+COL_OFFSET[k]);
                        }
                    }
                }

                else if (activeDirection>=67.5 && activeDirection<112.5)
                {
                    for (int k=0; k<9; k++)
                    {
                        if (k==2 || k==5 || k==8 )
                        {
                            if (dirWin[k] > 0.0)
                            {
                                wetCells++;
                                rows.append(i + ROW_OFFSET[k]);
                                cols.append(j + COL_OFFSET[k]);
                            }
                        }
                    }

                }
                else if ((activeDirection>0.0 && activeDirection<=22.5) || (activeDirection<=360.0 && activeDirection>=337.5))
                {
                    for (int k=0; k<9; k++)
                    {
                        if (k==1 || k==2 || k==0)
                        {
                            if (dirWin[k] > 0.0)
                            {
                                wetCells++;
                                rows.append(i + ROW_OFFSET[k]);
                                cols.append(j + COL_OFFSET[k]);
                            }
                        }
                    }
                }
                else if (activeDirection>=247.5 && activeDirection<=292.5)
                {
                    for (int k=0; k<9; k++)
                    {
                        if (k==0 || k==3 ||  k==6 )
                        {
                            if (dirWin[k] > 0.0)
                            {
                                wetCells++;
                                rows.append(i + ROW_OFFSET[k]);
                                cols.append(j + COL_OFFSET[k]);
                            }
                        }
                    }
                }
                else if (activeDirection>=157.5 && activeDirection<=202.5)
                {
                    for (int k=0; k<9; k++)
                    {
                        if (k==6 || k==7 || k==8 )
                        {
                            if (dirWin[k] > 0.0)
                            {
                                wetCells++;
                                rows.append(i + ROW_OFFSET[k]);
                                cols.append(j + COL_OFFSET[k]);
                            }
                        }
                    }
                }
                else if (activeDirection>22.5 && activeDirection<67.5)
                {
                    for (int k=0; k<9; k++)
                    {
                        if (k==1 || k==2 || k==5)
                        {
                            if (dirWin[k] > 0.0)
                            {
                                wetCells++;
                                rows.append(i + ROW_OFFSET[k]);
                                cols.append(j + COL_OFFSET[k]);
                            }
                        }
                    }
                }
                else if (activeDirection>292.5 && activeDirection<337.5)
                {
                    for (int k=0; k<9; k++)
                    {
                        if (k==1 || k==0 || k==3)
                        {
                            if (dirWin[k] > 0.0)
                            {
                                wetCells++;
                                rows.append(i + ROW_OFFSET[k]);
                                cols.append(j + COL_OFFSET[k]);
                            }
                        }
                    }
                }
                else if (activeDirection>202.5 && activeDirection<247.5)
                {
                    for (int k=0; k<9; k++)
                    {
                        if (k==3 || k==6 || k==7)
                        {
                            if (dirWin[k] > 0.0)
                            {
                                wetCells++;
                                rows.append(i + ROW_OFFSET[k]);
                                cols.append(j + COL_OFFSET[k]);
                            }
                        }
                    }
                }
                else if (activeDirection>112.5 && activeDirection<157.5)
                {

                    for (int k=0; k<9; k++)
                    {
                        if (k==5 || k==7 || k==8 )
                        {
                            if (dirWin[k] > 0.0)
                            {
                                wetCells++;
                                rows.append(i + ROW_OFFSET[k]);
                                cols.append(j + COL_OFFSET[k]);
                            }
                        }
                    }
                }
                else
                {

                }

                if (wetCells == 0)
                {
                    if (dirWin[4] > 0.0)
                    {
                        wetCells++;
                        rows.append(i);
                        cols.append(j);
                    }
                    else
                    {
                        for (int k=0; k<9; k++)
                        {
                            if (dirWin[k] > 0.0)
                            {
                                wetCells++;
                                rows.append(i + ROW_OFFSET[k]);
                                cols.append(j + COL_OFFSET[k]);
                            }
                        }
                        if (wetCells == 0)
                        {
                            unaccounted += fabs(erdRow[j]);
                            calc = false;
                            qDebug()<<"unaccounted "<<fabs(erdRow[j])<<" at "<<i<< j;
                        }
                    }
                }

                if (calc)
                {
                    cellSed = fabs(erdRow[j]) / (wetCells*1.0);

                    for (int k=0; k<rows.size(); k++)
                    {
                        findDepositionCells(rows[k], cols[k], cellSed);
                        addDeposition();
                    }
                }

                rows.clear();
                cols.clear();
            }
            else
            {

            }
        }
    }

    GDALClose(pRaster);
    GDALClose(pFdirRaster);
    GDALClose(pDepthRaster);

    CPLFree(erdRow);
    CPLFree(dirWin);
    qDebug()<<"done depositing";
}
