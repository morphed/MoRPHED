#include "morph_raster.h"

MORPH_Raster::MORPH_Raster()
{
    loadDrivers();
}

MORPH_Raster::MORPH_Raster(const char *rasterPath)
{
    loadDrivers();
    setProperties(rasterPath);
}

MORPH_Raster::~MORPH_Raster()
{
    GDALDestroyDriverManager();
}

void MORPH_Raster::add(const char *addPath)
{
    GDALDataset *pSourceDS, *pAddDS;

    pSourceDS = (GDALDataset*) GDALOpen(m_rasterPath, GA_Update);
    pAddDS = (GDALDataset*) GDALOpen(addPath, GA_ReadOnly);

    float *srcRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *addRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *newRow = (float*) CPLMalloc(sizeof(float)*nCols);

    for (int i=0; i<nRows; i++)
    {
        pSourceDS->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, srcRow, nCols, 1, GDT_Float32, 0, 0);
        pAddDS->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, addRow, nCols, 1, GDT_Float32, 0, 0);

        for (int j=0; j<nCols; j++)
        {
            if (srcRow[j] == noData)
            {
                newRow[j] = noData;
            }
            else if (addRow[j] == noData)
            {
                newRow[j] = srcRow[j];
            }
            else
            {
                newRow[j] = srcRow[j] + addRow[j];
            }
        }

        pSourceDS->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, newRow, nCols, 1, GDT_Float32, 0, 0);
    }

    GDALClose(pSourceDS);
    GDALClose(pAddDS);

    CPLFree(srcRow);
    CPLFree(addRow);
    CPLFree(newRow);
}

void MORPH_Raster::add(const char *sourcePath, const char *addPath)
{
    setProperties(sourcePath);
    add(addPath);
}

void MORPH_Raster::aspect(const char *aspectPath)
{
    GDALDataset *pSourceDS, *pAspectDS;

    pSourceDS = (GDALDataset*) GDALOpen(m_rasterPath, GA_ReadOnly);

    pAspectDS = pDriverTiff->Create(aspectPath, nCols, nRows, 1, GDT_Float32, NULL);
    pAspectDS->SetGeoTransform(transform);
    pAspectDS->GetRasterBand(1)->SetNoDataValue(noData);
    pAspectDS->GetRasterBand(1)->Fill(noData);

    bool calc;
    double dx, dy;

    float *eVals = (float*) CPLMalloc(sizeof(float)*9);
    float *aVal = (float*) CPLMalloc(sizeof(float)*1);

    for (int i=1; i<nRows-1; i++)
    {
        for (int j=1; j<nCols-1; j++)
        {
            pSourceDS->GetRasterBand(1)->RasterIO(GF_Read, j-1, i-1, 3, 3, eVals, 3, 3, GDT_Float32, 0, 0);

            calc = true;
            for (int k=0; k<9; k++)
            {
                if (eVals[k] == noData)
                {
                    *aVal = noData;
                    calc = false;
                    break;
                }
            }

            if (calc)
            {
                dx = ((eVals[2] + eVals[5] + eVals[5] + eVals[8]) - (eVals[0] + eVals[3] + eVals[3] + eVals[6]));
                dy = ((eVals[6] + eVals[7] + eVals[7] + eVals[8]) - (eVals[0] + eVals[1] + eVals[1] + eVals[2]));

                *aVal = atan2(dy/8.0, ((-1.0)*dx/8.0)) * (180.0/PI);

                if (dx == 0.0)
                {
                    if (*aVal < 0.0)
                    {
                        *aVal = 90.0 - *aVal;
                    }
                    else if (*aVal > 90.0)
                    {
                        *aVal = 360.0 - *aVal + 90.0;
                    }
                    else
                    {
                        *aVal = 90.0 - *aVal;
                    }
                }
                else
                {
                    if (*aVal > 90.0)
                    {
                        *aVal = 450.0 - *aVal;
                    }
                    else
                    {
                        *aVal = 90.0 - *aVal;
                    }
                }

                if (*aVal == 0.0)
                {
                    *aVal = 0.001;
                }
            }

            pAspectDS->GetRasterBand(1)->RasterIO(GF_Write, j, i, 1, 1, aVal, 1, 1, GDT_Float32, 0, 0);
        }
    }

    GDALClose(pSourceDS);
    GDALClose(pAspectDS);

    CPLFree(eVals);
    CPLFree(aVal);
}

void MORPH_Raster::aspect(const char *sourcePath, const char *aspectPath)
{
    setProperties(sourcePath);

    aspect(aspectPath);
}

double MORPH_Raster::averageSlope_ColAve(int col1, int col2)
{
    double col1Ave, col2Ave, aveSlope, distance;

    distance = col1 - col2;

    if (distance < 0)
    {
        distance *= (-1.0);
    }

    col1Ave = averageCol(col1);
    col2Ave = averageCol(col2);

    aveSlope = (col1Ave - col2Ave) / (transform[1]*(distance*1.0));

    if (aveSlope < 0.0)
    {
        aveSlope *= (-1.0);
    }

    return aveSlope;
}

double MORPH_Raster::averageSlope_ColAve(const char *rasterPath, int col1, int col2)
{
    double slope;
    setProperties(rasterPath);
    slope = averageSlope_ColAve(col1, col2);
    return slope;
}

double MORPH_Raster::averageSlope_RowAve(int row1, int row2)
{
    double row1Ave, row2Ave, aveSlope, distance;

    distance = row1 - row2;

    if (distance < 0)
    {
        distance *= (-1.0);
    }

    row1Ave = averageRow(row1);
    row2Ave = averageRow(row2);

    aveSlope = (row1Ave - row2Ave) / (transform[1]*distance);

    if (aveSlope < 0.0)
    {
        aveSlope *= (-1.0);
    }

    return aveSlope;
}

double MORPH_Raster::averageSlope_RowAve(const char *rasterPath, int row1, int row2)
{
    double slope;
    setProperties(rasterPath);
    slope = averageSlope_RowAve(row1, row2);
    return slope;
}

void MORPH_Raster::copyBoundary(int boundLoc)
{
    GDALDataset *pRaster;

    pRaster = (GDALDataset*) GDALOpen(m_rasterPath, GA_Update);

    float *newRow = (float*) CPLMalloc(sizeof(float));

    if (boundLoc == 1)
    {
        newRow = (float*) CPLMalloc(sizeof(float)*nCols);

        pRaster->GetRasterBand(1)->RasterIO(GF_Read,0,2,nCols,1,newRow,nCols,1,GDT_Float32,0,0);
        pRaster->GetRasterBand(1)->RasterIO(GF_Write,0,1,nCols,1,newRow,nCols,1,GDT_Float32,0,0);
        pRaster->GetRasterBand(1)->RasterIO(GF_Write,0,0,nCols,1,newRow,nCols,1,GDT_Float32,0,0);
    }
    else if (boundLoc == 2)
    {
        newRow = (float*) CPLMalloc(sizeof(float)*nCols);

        pRaster->GetRasterBand(1)->RasterIO(GF_Read,0,nRows-3,nCols,1,newRow,nCols,1,GDT_Float32,0,0);
        pRaster->GetRasterBand(1)->RasterIO(GF_Write,0,nRows-2,nCols,1,newRow,nCols,1,GDT_Float32,0,0);
        pRaster->GetRasterBand(1)->RasterIO(GF_Write,0,nRows-1,nCols,1,newRow,nCols,1,GDT_Float32,0,0);
    }
    else if (boundLoc == 3)
    {
        newRow = (float*) CPLMalloc(sizeof(float)*nRows);

        pRaster->GetRasterBand(1)->RasterIO(GF_Read,nCols-3,0,1,nRows,newRow,1,nRows,GDT_Float32,0,0);
        pRaster->GetRasterBand(1)->RasterIO(GF_Write,nCols-2,0,1,nRows,newRow,1,nRows,GDT_Float32,0,0);
        pRaster->GetRasterBand(1)->RasterIO(GF_Write,nCols-1,0,1,nRows,newRow,1,nRows,GDT_Float32,0,0);
    }
    else if (boundLoc == 4)
    {
        newRow = (float*) CPLMalloc(sizeof(float)*nRows);

        pRaster->GetRasterBand(1)->RasterIO(GF_Read,2,0,1,nRows,newRow,1,nRows,GDT_Float32,0,0);
        pRaster->GetRasterBand(1)->RasterIO(GF_Write,1,0,1,nRows,newRow,1,nRows,GDT_Float32,0,0);
        pRaster->GetRasterBand(1)->RasterIO(GF_Write,0,0,1,nRows,newRow,1,nRows,GDT_Float32,0,0);
    }

    GDALClose(pRaster);

    CPLFree(newRow);
}

void MORPH_Raster::copyBoundary(const char *rasterPath, int boundLoc)
{
    setProperties(rasterPath);
    copyBoundary(boundLoc);
}

void MORPH_Raster::demOfDifference(const char *oldDem, const char *newDem, const char *dodRaster)
{
    setProperties(oldDem);

    GDALDataset *pOldDem, *pNewDem, *pDod;

    pOldDem = (GDALDataset*) GDALOpen(oldDem, GA_ReadOnly);
    pNewDem = (GDALDataset*) GDALOpen(newDem, GA_ReadOnly);
    pDod = pDriverTiff->Create(dodRaster, nCols, nRows, 1, GDT_Float32, NULL);

    pDod->SetGeoTransform(transform);
    pDod->GetRasterBand(1)->SetNoDataValue(noData);
    pDod->GetRasterBand(1)->Fill(noData);

    float *oldRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *newRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *dodRow = (float*) CPLMalloc(sizeof(float)*nCols);

    for (int i=0; i<nRows; i++)
    {

        pOldDem->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, oldRow, nCols, 1, GDT_Float32, 0, 0);
        pNewDem->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, newRow, nCols, 1, GDT_Float32, 0, 0);

        for (int j=0; j<nCols; j++)
        {
            if (oldRow[j] != noData && newRow[j] != noData)
            {
                dodRow[j] = newRow[j] - oldRow[j];
            }
            else
            {
                dodRow[j] = noData;
            }
        }

        pDod->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, dodRow, nCols, 1, GDT_Float32, 0, 0);

    }

    GDALClose(pOldDem);
    GDALClose(pNewDem);
    GDALClose(pDod);

    CPLFree(oldRow);
    CPLFree(newRow);
    CPLFree(dodRow);
}

void MORPH_Raster::filterLowPass(const char *filterRaster)
{
    GDALDataset *pSource, *pFilter;

    pSource = (GDALDataset*) GDALOpen(m_rasterPath, GA_ReadOnly);
    pFilter = pDriverTiff->Create(filterRaster, nCols, nRows, 1, GDT_Float32, NULL);

    pFilter->SetGeoTransform(transform);
    pFilter->GetRasterBand(1)->Fill(noData);
    pFilter->GetRasterBand(1)->SetNoDataValue(noData);

    int count;
    double average;

    float *value = (float*) CPLMalloc(sizeof(float)*1);
    float *window = (float*) CPLMalloc(sizeof(float)*9);

    for (int i=1; i<nRows-1; i++)
    {
        for (int j=1; j<nCols-1; j++)
        {
            count = 0;
            average = 0.0;

            pSource->GetRasterBand(1)->RasterIO(GF_Read, j-1, i-1, 3, 3, window, 3, 3, GDT_Float32, 0, 0);

            if (window[4] == noData)
            {
                *value = noData;
            }
            else
            {
                for (int k=0; k<9; k++)
                {
                    if (window[k] != noData)
                    {
                        count++;
                        average += window[k];
                    }
                }

                *value = average / (count*1.0);

            }

            pFilter->GetRasterBand(1)->RasterIO(GF_Write, j, i, 1, 1, value, 1, 1, GDT_Float32, 0, 0);

        }
    }

    GDALClose(pSource);
    GDALClose(pFilter);

    CPLFree(window);
    CPLFree(value);
}

void MORPH_Raster::filterLowPass(const char *sourceRaster, const char *filterRaster)
{
    setProperties(sourceRaster);
    filterLowPass(filterRaster);
}

double MORPH_Raster::findMax(const char *rasterPath)
{
    setProperties(rasterPath);

    GDALDataset *pRaster = (GDALDataset*) GDALOpen(rasterPath, GA_ReadOnly);

    double max = noData;
    bool firstVal = false;

    float *row = (float*) CPLMalloc(sizeof(float)*nCols);

    for (int i=0; i<nRows; i++)
    {
        pRaster->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, row, nCols, 1, GDT_Float32, 0, 0);

        for (int j=0; j<nCols; j++)
        {
            if (row[j] != noData)
            {
                if (firstVal)
                {
                    if (row[j] > max)
                    {
                        max = row[j];
                    }
                }
                else
                {
                    if (row[j] != noData)
                    {
                        max = row[j];
                        firstVal = true;
                    }
                }
            }
        }
    }

    GDALClose(pRaster);

    CPLFree(row);

    return max;
}

void MORPH_Raster::flowDirection(const char *xVelPath, const char *yVelPath, const char *rasterPath)
{
    setProperties(xVelPath);

    GDALDataset *pXvel, *pYvel, *pRaster;
    pXvel = (GDALDataset*) GDALOpen(xVelPath, GA_ReadOnly);
    pYvel = (GDALDataset*) GDALOpen(yVelPath, GA_ReadOnly);

    pRaster = pDriverTiff->Create(rasterPath, nCols, nRows, 1, GDT_Float32, NULL);
    pRaster->GetRasterBand(1)->Fill(noData);
    pRaster->SetGeoTransform(transform);
    pRaster->GetRasterBand(1)->SetNoDataValue(noData);

    double angleDegree;

    float *xvel = (float*) CPLMalloc(sizeof(float)*nCols);
    float *yvel = (float*) CPLMalloc(sizeof(float)*nCols);
    float *fdir = (float*) CPLMalloc(sizeof(float)*nCols);

    for (int i=0; i<nRows; i++)
    {

        pXvel->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, xvel, nCols, 1, GDT_Float32, 0, 0);
        pYvel->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, yvel, nCols, 1, GDT_Float32, 0, 0);

        for (int j=0; j<nCols; j++)
        {
            if (xvel[j] == noData || yvel[j] == noData)
            {
                fdir[j] = 0.0;
            }
            else
            {

                angleDegree = qAtan(fabs(xvel[j] / yvel[j])) * (180.0/PI);

                if(xvel[j] > 0.0 && yvel[j] > 0.0)
                {
                    fdir[j] = 0.0 + angleDegree;
                }
                else if (yvel[j] < 0.0 && xvel[j] > 0.0)
                {
                    fdir[j] = 180.0 - angleDegree;
                }
                else if (xvel[j] < 0.0 && yvel[j] < 0.0)
                {
                    fdir[j] = 180.0 + angleDegree;
                }
                else if (xvel[j] < 0.0 && yvel[j] > 0.0)
                {
                    fdir[j] = 360.0 - angleDegree;
                }
                else if (xvel[j] > 0.0 && yvel[j] == 0.0)
                {
                    fdir[j] = 270.0;
                }
                else if (xvel[j] < 0.0 && yvel[j] == 0.0)
                {
                    fdir[j] = 90.0;
                }
                else if (xvel[j] == 0.0 && yvel[j] > 0.0)
                {
                    fdir[j] = 360.0;
                }
                else if (xvel[j] == 0.0 && yvel[j] < 0.0)
                {
                    fdir[j] = 180.0;
                }
                else if (xvel[j] == 0.0 && yvel[j] == 0.0)
                {
                    fdir[j] = 0.0;
                }
                else
                {
                    qDebug()<<"Flow direction velocity assignment error "<<i<<" "<<j;
                    fdir[j] = 0.0;
                }

                if ((fdir[j] > 360.0 || fdir[j] < 0.0) && fdir[j] != noData)
                {
                    qDebug()<<"Flow direction range error "<<i<<" "<<j<<" "<<fdir[j]<<" "<<xvel[j]<<" "<<yvel[j];
                    fdir[j] = 0.0;
                }
                else
                {

                }
            }
        }

        pRaster->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, fdir, nCols, 1, GDT_Float32, 0, 0);

    }

    GDALClose(pRaster);
    GDALClose(pXvel);
    GDALClose(pYvel);

    CPLFree(xvel);
    CPLFree(yvel);
    CPLFree(fdir);
}

void MORPH_Raster::fromXYZ(const char *rasterPath, const char *xyzPath, int cols, int rows, double noDataValue, double inTransform[6], int headerRows)
{
    int i, j;
    double x, y, yTLCenter, xTLCenter;
    float z;
    QString qsDummy, qsX, qsY, qsZ;

    GDALDataset *pDatasetNew;
    pDatasetNew = pDriverTiff->Create(rasterPath, cols, rows, 1, GDT_Float32, NULL);
    pDatasetNew->SetGeoTransform(inTransform);
    pDatasetNew->GetRasterBand(1)->Fill(noDataValue);
    pDatasetNew->GetRasterBand(1)->SetNoDataValue(noDataValue);

    xTLCenter = inTransform[0] + (inTransform[1] / 2.0);
    yTLCenter = inTransform[3] - (inTransform[1] / 2.0);

    float *rasVal = (float*) CPLMalloc(sizeof(float)*1);

    QFile inFile (QString::fromUtf8(xyzPath));

    if (inFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {

        QTextStream stream(&inFile);

        int count = 0;

        while(!stream.atEnd())
        {
            if (count < headerRows)
            {
                qsDummy = stream.readLine();
                count++;
            }
            else
            {
                stream >> qsX;
                x = qsX.toDouble();
                stream >> qsY;
                y = qsY.toDouble();
                stream >> qsZ;
                z = qsZ.toDouble();

                *rasVal = z;

                i = (yTLCenter - y) / inTransform[1];
                j = (x - xTLCenter) / inTransform[1];

                if ((i>=0 && i<rows) && (j>=0 && j<cols))
                {
                    pDatasetNew->GetRasterBand(1)->RasterIO(GF_Write, j, i, 1, 1, rasVal, 1, 1, GDT_Float32, 0, 0);
                }

                count++;

            }
        }
    }

    setProperties(rasterPath);

    GDALClose(pDatasetNew);
    CPLFree(rasVal);
}

void MORPH_Raster::hillshade(const char *hlsdPath)
{
    GDALDataset *pSourceDS, *pHlsdDS;

    pSourceDS = (GDALDataset*) GDALOpen(m_rasterPath, GA_ReadOnly);
    pHlsdDS = pDriverTiff->Create(hlsdPath, nCols, nRows, 1, GDT_Byte, NULL);
    pHlsdDS->SetGeoTransform(transform);
    pHlsdDS->GetRasterBand(1)->SetNoDataValue(0);
    pHlsdDS->GetRasterBand(1)->Fill(0);

    double altDeg = 45.0, azimuth = 315.0, zFactor = 1.0,
            zenDeg = 90.0 - altDeg, zenRad = zenDeg *PI / 180.0,
            azimuthMath = 360.0 - azimuth + 90.0,
            azimuthRad, dzdx, dzdy, slopeRad, aspectRad, hlsdByte;

    if (azimuthMath > 360.0)
    {
        azimuthMath = azimuthMath - 360.0;
    }

    azimuthRad = azimuthMath * PI / 180.0;

    float *elevWin = (float*) CPLMalloc(sizeof(float)*9);
    float *hlsdRow = (float*) CPLMalloc(sizeof(float)*nCols);

    bool calculate;

    for (int i=1; i<nRows-1; i++)
    {
        for (int j=1; j<nCols-1; j++)
        {
            pSourceDS->GetRasterBand(1)->RasterIO(GF_Read, j-1, i-1, 3, 3, elevWin, 3, 3, GDT_Float32, 0, 0);

            calculate = true;

            for (int k=0; k<9; k++)
            {
                if (elevWin[k] == noData)
                {
                    hlsdRow[j] = 0;
                    calculate = false;
                    break;
                }
            }

            if (calculate)
            {
                dzdx = ((elevWin[2]+(2*elevWin[5])+elevWin[8]) - (elevWin[0]+(2*elevWin[3])+elevWin[6])) / (8*transform[1]);
                dzdy = ((elevWin[6]+(2*elevWin[7])+elevWin[8]) - (elevWin[0]+(2*elevWin[1])+elevWin[2])) / (8*transform[1]);
                slopeRad = atan(zFactor * sqrt(pow(dzdx,2)+pow(dzdy,2)));

                if (dzdx != 0.0)
                {
                    aspectRad = atan2(dzdy, (dzdx * (-1.0)));

                    if ( aspectRad < 0.0)
                    {
                        aspectRad = 2.0 * PI + aspectRad;
                    }
                }
                else
                {
                    if (dzdy > 0.0)
                    {
                        aspectRad = PI / 2.0;
                    }
                    else if (dzdy < 0.0)
                    {
                        aspectRad = (2.0 * PI) - (PI / 2.0);
                    }
                    else
                    {
                        aspectRad = aspectRad;
                    }
                }

                hlsdByte = round(254 * ((cos(zenRad) * cos(slopeRad)) + (sin(zenRad) * sin(slopeRad) * cos(azimuthRad - aspectRad)))) + 1.0;
                hlsdRow[j] = hlsdByte;

            }
        }

        pHlsdDS->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, hlsdRow, nCols, 1, GDT_Byte, 0, 0);

    }

    GDALClose(pHlsdDS);
    GDALClose(pSourceDS);

    CPLFree(elevWin);
    CPLFree(hlsdRow);
}

void MORPH_Raster::hillshade(const char *rasterPath, const char *hlsdPath)
{
    setProperties(rasterPath);
    hillshade(hlsdPath);
}

int MORPH_Raster::regions(const char *regionsRaster)
{
    GDALDataset *pInputRaster, *pRegionsRaster;

    pInputRaster = (GDALDataset*) GDALOpen(m_rasterPath, GA_ReadOnly);

    pRegionsRaster = pDriverTiff->CreateCopy(regionsRaster, pInputRaster, FALSE, NULL, NULL, NULL);

    float *inpVal = (float*) CPLMalloc(sizeof(float)*1);
    float *regVal = (float*) CPLMalloc(sizeof(float)*1);
    float *inpWin = (float*) CPLMalloc(sizeof(float)*9);
    float *regWin = (float*) CPLMalloc(sizeof(float)*9);

    int regionCount, changedCount, regionValue, valueCount, totalCount;
    bool regionDone, rasterDone;

    rasterDone = false;
    regionValue = 0;
    totalCount = 0;

    while (!rasterDone && totalCount < 4)
    {
        changedCount = 0;

        for (int i=1; i<nRows-1; i++)
        {
            for (int j=1; j<nCols-1; j++)
            {
                pInputRaster->GetRasterBand(1)->RasterIO(GF_Read, j, i, 1, 1, inpVal, 1, 1, GDT_Float32, 0, 0);
                pRegionsRaster->GetRasterBand(1)->RasterIO(GF_Read, j, i, 1, 1, regVal, 1, 1, GDT_Float32, 0, 0);

                if (*inpVal == 0 && *regVal == 0)
                {
                    regionValue++;
                    changedCount++;
                    *regVal = regionValue;
                    pRegionsRaster->GetRasterBand(1)->RasterIO(GF_Write, j, i, 1, 1, regVal, 1, 1, GDT_Float32, 0, 0);
                    regionDone = false;

                    valueCount = 0;

                    while (!regionDone)
                    {
                        regionCount = 0;

                        for (int k=1; k<nRows-1; k++)
                        {
                            for (int l=1; l<nCols-1; l++)
                            {
                                pRegionsRaster->GetRasterBand(1)->RasterIO(GF_Read, l-1, k-1, 3, 3, regWin, 3, 3, GDT_Float32, 0, 0);
                                pInputRaster->GetRasterBand(1)->RasterIO(GF_Read, l-1, k-1, 3, 3, inpWin, 3, 3, GDT_Float32, 0, 0);

                                if (inpWin[4] == 0.0 && regWin[4] == *regVal)
                                {
                                    for (int m=0; m<9; m++)
                                    {
                                        if (regWin[m] == 0.0)
                                        {
                                            pRegionsRaster->GetRasterBand(1)->RasterIO(GF_Write, l+COL_OFFSET[m], k+ROW_OFFSET[m], 1, 1, regVal, 1, 1, GDT_Float32, 0, 0);
                                            regionCount++;
                                            valueCount++;
                                        }
                                    }
                                }
                            }
                        }

                        if (regionCount == 0)
                        {
                            regionDone = true;
                        }
                    }
                }
            }
        }

        if (changedCount == 0)
        {
            rasterDone = true;
        }

        totalCount++;
    }

    GDALClose(pInputRaster);
    GDALClose(pRegionsRaster);

    CPLFree(inpVal);
    CPLFree(regVal);
    CPLFree(inpWin);
    CPLFree(regWin);

    return regionValue;
}

int MORPH_Raster::regions(const char *inputRaster, const char *regionsRaster)
{
    int regionsValue;
    setProperties(inputRaster);
    regionsValue = regions(regionsRaster);

    return regionsValue;
}

void MORPH_Raster::setProperties(const char *rasterPath)
{
    loadDrivers();

    m_rasterPath = rasterPath;
    GDALDataset *pRaster = (GDALDataset*) GDALOpen(m_rasterPath, GA_ReadOnly);

    nRows = pRaster->GetRasterBand(1)->GetYSize();
    nCols = pRaster->GetRasterBand(1)->GetXSize();
    pRaster->GetGeoTransform(transform);
    noData = pRaster->GetRasterBand(1)->GetNoDataValue();

    GDALClose(pRaster);
}

void MORPH_Raster::slopeTOF(const char *slopePath)
{
    GDALDataset *pSourceDS, *pSlopeDS;

    pSourceDS = (GDALDataset*) GDALOpen(m_rasterPath, GA_ReadOnly);
    pSlopeDS = pDriverTiff->Create(slopePath, nCols, nRows, 1, GDT_Float32, NULL);
    pSlopeDS->SetGeoTransform(transform);
    pSlopeDS->GetRasterBand(1)->SetNoDataValue(noData);
    pSlopeDS->GetRasterBand(1)->Fill(noData);

    double xslope, yslope, xyslope, xypow;

    float* eVals = (float*) CPLMalloc(sizeof(float)*9);
    float* sVals = (float*) CPLMalloc(sizeof(float)*nCols);

    for (int i=0; i<nRows-1; i++)
    {
        for (int j=0; j<nCols-1; j++)
        {
            if (i == 0 || i == nRows || j == 0 || j == nCols)
            {
                sVals[j] = noData;
            }
            else
            {
                pSourceDS->GetRasterBand(1)->RasterIO(GF_Read,j-1,i-1,3,3,eVals,3,3,GDT_Float32,0,0);
                if (eVals[4] == noData || eVals[0] == noData || eVals[1] == noData || eVals[2] == noData || eVals[3] == noData || eVals[5] == noData || eVals[6] == noData || eVals[7] == noData || eVals[8] == noData)
                {
                    sVals[j] = noData;
                }
                else
                {
                    xslope = ((eVals[2]-eVals[0]) + ((2*eVals[5])-(2*eVals[3])) + (eVals[8]-eVals[6])) / (8*transform[1]);
                    yslope = ((eVals[0]-eVals[6]) + ((2*eVals[1])-(2*eVals[7])) + (eVals[2]-eVals[8]))/(8*transform[1]);
                    xyslope = pow(xslope,2.0) + pow(yslope,2.0);
                    xypow = pow(xyslope,0.5);
                    sVals[j] = (atan(xypow)*180.0/PI);
                }
            }
        }
        pSlopeDS->GetRasterBand(1)->RasterIO(GF_Write,0,i,nCols,1,sVals,nCols,1,GDT_Float32,0,0);
    }

    GDALClose(pSourceDS);
    GDALClose(pSlopeDS);

    CPLFree(eVals);
    CPLFree(sVals);
}

void MORPH_Raster::slopeTOF(const char *sourcePath, const char *slopePath)
{
    setProperties(sourcePath);

    slopeTOF(slopePath);
}

void MORPH_Raster::subtract(const char *subtractPath)
{
    GDALDataset *pSourceDS, *pSubtractDS;

    loadDrivers();

    pSourceDS = (GDALDataset*) GDALOpen(m_rasterPath, GA_Update);
    pSubtractDS = (GDALDataset*) GDALOpen(subtractPath, GA_ReadOnly);

    float *srcRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *subRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *newRow = (float*) CPLMalloc(sizeof(float)*nCols);

    for (int i=0; i<nRows; i++)
    {
        pSourceDS->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, srcRow, nCols, 1, GDT_Float32, 0, 0);
        pSubtractDS->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, subRow, nCols, 1, GDT_Float32, 0, 0);

        for (int j=0; j<nCols; j++)
        {
            if (srcRow[j] == noData)
            {
                newRow[j] = noData;
            }
            else if (subRow[j] == noData)
            {
                newRow[j] = srcRow[j];
            }
            else
            {
                newRow[j] = srcRow[j] - subRow[j];
            }
        }

        pSourceDS->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, newRow, nCols, 1, GDT_Float32, 0, 0);
    }

    GDALClose(pSourceDS);
    GDALClose(pSubtractDS);

    CPLFree(srcRow);
    CPLFree(subRow);
    CPLFree(newRow);
}

void MORPH_Raster::subtract(const char *sourcePath, const char *subtractPath)
{
    setProperties(sourcePath);
    subtract(subtractPath);
}

void MORPH_Raster::subtract(const char *sourcePath, const char *subtractPath, const char *outputPath)
{
    setProperties(sourcePath);

    GDALDataset *pSourceDS, *pSubtractDS, *pOutDS;

    pSourceDS = (GDALDataset*) GDALOpen(m_rasterPath, GA_Update);
    pSubtractDS = (GDALDataset*) GDALOpen(subtractPath, GA_ReadOnly);
    pOutDS = pDriverTiff->Create(outputPath, nCols, nRows, 1, GDT_Float32, NULL);

    pOutDS->GetRasterBand(1)->Fill(noData);
    pOutDS->GetRasterBand(1)->SetNoDataValue(noData);
    pOutDS->SetGeoTransform(transform);

    float *srcRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *subRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *newRow = (float*) CPLMalloc(sizeof(float)*nCols);

    for (int i=0; i<nRows; i++)
    {
        pSourceDS->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, srcRow, nCols, 1, GDT_Float32, 0, 0);
        pSubtractDS->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, subRow, nCols, 1, GDT_Float32, 0, 0);

        for (int j=0; j<nCols; j++)
        {
            if (srcRow[j] == noData)
            {
                newRow[j] = noData;
            }
            else if (subRow[j] == noData)
            {
                newRow[j] = srcRow[j];
            }
            else
            {
                newRow[j] = srcRow[j] - subRow[j];
            }
        }

        pOutDS->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, newRow, nCols, 1, GDT_Float32, 0, 0);
    }

    GDALClose(pSourceDS);
    GDALClose(pSubtractDS);
    GDALClose(pOutDS);

    CPLFree(srcRow);
    CPLFree(subRow);
    CPLFree(newRow);
}

double MORPH_Raster::sum()
{
    GDALDataset *pRaster;

    pRaster = (GDALDataset*) GDALOpen(m_rasterPath, GA_ReadOnly);

    double sum = 0.0;

    float *row = (float*) CPLMalloc(sizeof(float)*nCols);

    for (int i=0; i<nRows; i++)
    {
        pRaster->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, row, nCols, 1, GDT_Float32, 0, 0);

        for (int j=0; j<nCols; j++)
        {
            if (row[i] != noData)
            {
                sum += row[i];
            }
        }
    }

    GDALClose(pRaster);

    CPLFree(row);

    return sum;
}

double MORPH_Raster::sum(const char *rasterPath)
{
    setProperties(rasterPath);

    return(sum());
}

void MORPH_Raster::zeroToNoData(const char *sourcePath, double noDataValue)
{
    setProperties(sourcePath);
    noData = noDataValue;

    GDALDataset *pSourceRaster;

    pSourceRaster = (GDALDataset*) GDALOpen(sourcePath, GA_Update);

    float *oldRow = (float*) CPLMalloc(sizeof(float)*nCols);
    float *newRow = (float*) CPLMalloc(sizeof(float)*nCols);

    GDALClose(pSourceRaster);

    for (int i=0; i<nRows; i++)
    {
        pSourceRaster->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, oldRow, nCols, 1, GDT_Float32, 0, 0);

        for (int j=0; j<nCols; j++)
        {
            if (oldRow[j] == 0.0 || oldRow[j] == noData)
            {
                newRow[j] = noData;
            }
            else
            {
                newRow[j] = oldRow[j];
            }
        }

        pSourceRaster->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, newRow, nCols, 1, GDT_Float32, 0, 0);
    }

    GDALClose(pSourceRaster);

    CPLFree(oldRow);
    CPLFree(newRow);
}

double MORPH_Raster::averageCol(int col)
{
    GDALDataset *pRaster = (GDALDataset*) GDALOpen(m_rasterPath, GA_ReadOnly);

    double sum = 0.0;
    double count = 0.0;

    float* value = (float*) CPLMalloc(sizeof(float)*nRows);
    pRaster->GetRasterBand(1)->RasterIO(GF_Read, col, 0, 1, nRows, value, 1, nRows, GDT_Float32, 0, 0);

    for (int i=0; i<nRows; i++)
    {
        if(value[i] != noData)
        {
            sum += value[i];
            count++;
        }
    }

    GDALClose(pRaster);

    CPLFree(value);

    return (sum / (count*1.0));
}

double MORPH_Raster::averageRow(int row)
{
    GDALDataset *pRaster;
    pRaster = (GDALDataset*) GDALOpen(m_rasterPath, GA_ReadOnly);

    double sum = 0.0;
    double count = 0.0;

    float* value = (float*) CPLMalloc(sizeof(float)*nCols);
    pRaster->GetRasterBand(1)->RasterIO(GF_Read, 0, row, nCols, 1, value, nCols, 1, GDT_Float32, 0, 0);

    for (int i=0; i<nCols; i++)
    {
        if(value[i] != noData)
        {
            sum += value[i];
            count++;
        }
    }

    GDALClose(pRaster);

    CPLFree(value);

    return (sum / (count*1.0));
}

void MORPH_Raster::loadDrivers()
{
    GDALAllRegister();

    pDriverTiff = GetGDALDriverManager()->GetDriverByName("GTiff");
}
