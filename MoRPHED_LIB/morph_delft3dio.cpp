#include "morph_delft3dio.h"


MORPH_Delft3DIO::MORPH_Delft3DIO(QString xmlPath) : MORPH_Base(xmlPath)
{
    setDelftData();
}


MORPH_Delft3DIO::~MORPH_Delft3DIO()
{
    processDelft.kill();
    QString command = "taskkill /pid " + QString::number(nPID);
    processDelft.execute(command);
}

void MORPH_Delft3DIO::setDelftData()
{
    qsExtendedDemPath = qsTempPath + "/extended.tif";
    setOriginPoint();
}

void MORPH_Delft3DIO::calculateDSWSE()
{
    MORPH_Raster Raster;

    double meanDepth, area, chezy, conveyance, depth, diffQ;
    double conveyanceTotal = 0, estQ = 0;
    int count = 0;
    bool stop = false;

    float *row = (float*) CPLMalloc(sizeof(float)*nCols);

    depth = Raster.findMax(qsDelftDemPath.toStdString().c_str()) + dswe[nCurrentIteration];
    depth = newDemMax;

    openSourceDEM();

    if (nDirDSbound == 1)
    {
        pSourceDEM->GetRasterBand(1)->RasterIO(GF_Read,0,1,nCols,1,row,nCols,1,GDT_Float32,0,0);
    }
    else if (nDirDSbound == 2)
    {
        pSourceDEM->GetRasterBand(1)->RasterIO(GF_Read,0,nRows-2,nCols,1,row,nCols,1,GDT_Float32,0,0);
    }
    else if (nDirDSbound == 3)
    {

    }
    else if (nDirDSbound == 4)
    {

    }

    while (!stop && count < 1000000)
    {
        conveyanceTotal = 0.0;
        for (int i=0; i<nCols; i++)
        {
            meanDepth = depth - row[i];

            if (meanDepth > 0 && row[i] != noData)
            {
                area = cellWidth * meanDepth;
                chezy = 18.0*log10((12.0*meanDepth)/roughness);
                conveyance = chezy * sqrt(meanDepth) * area;
                conveyanceTotal += conveyance;
            }
        }

        estQ = conveyanceTotal * sqrt(dSlope);
        diffQ = estQ - q[nCurrentIteration];

        if (diffQ > (0.05*q[nCurrentIteration]) || diffQ < (-0.05*q[nCurrentIteration]))
        {
            if (diffQ > 0.0)
            {
                depth -= 0.0001;
            }
            else
            {
                depth += 0.0001;
            }
        }
        else
        {
            stop = true;
            depth = depth - newDemMax;
            qDebug()<<"NEW DSWSE: "<<depth<<" "<<dswe[nCurrentIteration];
            dswe[nCurrentIteration] = depth;
        }

        if (count == 999999)
        {
            qDebug()<<"too many iterations to find correct dswe, using provided value "<<depth<<" "<<q[nCurrentIteration]<<" "<<estQ<<" "<<meanDepth<<" "<<chezy<<" "<<conveyanceTotal;
        }

        count++;
    }

    GDALClose(pSourceDEM);

    CPLFree(row);
}

void MORPH_Delft3DIO::closeDelftDatasets()
{
    if (pExtendedDEM != NULL)
    {
        GDALClose(pExtendedDEM);
        QFile::remove(qsExtendedDemPath);
        pExtendedDEM = NULL;
    }


    xUSCoords.clear();
    yUSCoords.clear();
    xObsCoords.clear();
    yObsCoords.clear();
    dsCoords.clear();
}

void MORPH_Delft3DIO::extendDEMBoundary()
{
    MORPH_Raster Raster;

    double transAmt, extSlope;
    char **papszOptions = NULL;
    float *oldValue = (float*) CPLMalloc(sizeof(float)*nCols);
    float *newValue = (float*) CPLMalloc(sizeof(float)*nCols);
    pDriverTIFF = GetGDALDriverManager()->GetDriverByName("GTiff");

    if (nDirUSbound == 1)
    {
        nRowsExt = nRows + nAddCells;
        nColsExt = nCols;

        pExtendedDEM = pDriverTIFF->Create(qsExtendedDemPath.toStdString().c_str(), nColsExt, nRowsExt, 1, GDT_Float32, papszOptions);
        pExtendedDEM->GetRasterBand(1)->Fill(noData);
        transAmt = nAddCells*cellWidth;

        if (transAmt < 0)
        {
            transAmt *= (-1);
        }

        extTransform[3] = extTransform[3] + transAmt;
        pExtendedDEM->SetGeoTransform(extTransform);

        dSlope = Raster.averageSlope_RowAve(qsDelftDemPath.toStdString().c_str(), nRows-2, 2);
        extSlope = dSlope/4.0;

        //copy data from DEM;
        openSourceDEM();
        for (int i=0; i<nRows; i++)
        {
            pSourceDEM->GetRasterBand(1)->RasterIO(GF_Read,0,i,nCols,1,oldValue,nCols,1,GDT_Float32,0,0);
            pExtendedDEM->GetRasterBand(1)->RasterIO(GF_Write,0,i+nAddCells,nCols,1,oldValue,nCols,1,GDT_Float32,0,0);
        }

        //add new data to extended DEM
        pExtendedDEM->GetRasterBand(1)->RasterIO(GF_Read,0,nAddCells+3,nCols,1,oldValue,nCols,1,GDT_Float32,0,0);
        for (int i=nAddCells+2; i>=0; i--)
        {
            for (int j=0; j<nCols; j++)
            {   
                if (oldValue[j] != noData)
                {
                    newValue[j] = oldValue[j] + (extSlope * cellWidth * ((nAddCells+3)-i));
                }
                else
                {
                    newValue[j] = noData;
                }
            }
            pExtendedDEM->GetRasterBand(1)->RasterIO(GF_Write,0,i,nCols,1,newValue,nCols,1,GDT_Float32,0,0);
        }
    }
    else if (nDirUSbound == 2)
    {
        nRowsExt = nRows + nAddCells;
        nColsExt = nCols;

        pExtendedDEM = pDriverTIFF->Create(qsExtendedDemPath.toStdString().c_str(), nColsExt, nRowsExt, 1, GDT_Float32, papszOptions);
        pExtendedDEM->SetGeoTransform(extTransform);

        dSlope = Raster.averageSlope_RowAve(qsDelftDemPath.toStdString().c_str(), nRows-2, 2);
        extSlope = dSlope/4.0;

        //copy data from DEM
        openSourceDEM();

        for (int i=0; i<nRows; i++)
        {
            pSourceDEM->GetRasterBand(1)->RasterIO(GF_Read,0,i,nCols,1,oldValue,nCols,1,GDT_Float32,0,0);
            pExtendedDEM->GetRasterBand(1)->RasterIO(GF_Write,0,i,nCols,1,oldValue,nCols,1,GDT_Float32,0,0);
        }

        //add new data to extended DEM
        pExtendedDEM->GetRasterBand(1)->RasterIO(GF_Read,0,nRows-2,nCols,1,oldValue,nCols,1,GDT_Float32,0,0);
        for (int i=nRows-2; i<nRowsExt; i++)
        {
            for (int j=0; j<nCols; j++)
            {
                if (oldValue[j] != noData)
                {
                    newValue[j] = oldValue[j] + (extSlope * cellWidth * (i-(nRows-3)));
                }
                else
                {
                    newValue[j] = noData;
                }
            }
            pExtendedDEM->GetRasterBand(1)->RasterIO(GF_Write,0,i,nCols,1,newValue,nCols,1,GDT_Float32,0,0);
        }
    }
    else if (nDirUSbound == 3)
    {
        nRowsExt = nRows;
        nColsExt = nCols + nAddCells;
        pExtendedDEM = pDriverTIFF->Create(qsExtendedDemPath.toStdString().c_str(), nColsExt, nRowsExt, 1, GDT_Float32, papszOptions);
        pExtendedDEM->GetRasterBand(1)->Fill(noData);

        dSlope = Raster.averageSlope_ColAve(qsDelftDemPath.toStdString().c_str(), nCols-1, 0);
        extSlope = dSlope/2.0;

        //copy data from DEM
        openSourceDEM();

        pSourceDEM->GetGeoTransform(extTransform);
        pExtendedDEM->SetGeoTransform(extTransform);

        for (int i=0; i<nRows; i++)
        {
            for (int j=0; j<nCols; j++)
            {
                pSourceDEM->GetRasterBand(1)->RasterIO(GF_Read,j,i,1,1,oldValue,1,1,GDT_Float32,0,0);
                pExtendedDEM->GetRasterBand(1)->RasterIO(GF_Write,j,i,1,1,oldValue,1,1,GDT_Float32,0,0);
            }
        }

        //add new data to extended DEM
        for (int i=0; i<nRows; i++)
        {
            for (int j=nCols; j<nCols; j++)
            {
                pExtendedDEM->GetRasterBand(1)->RasterIO(GF_Read,j-1,i,1,1,oldValue,1,1,GDT_Float32,0,0);
                *newValue = *oldValue + extSlope * cellWidth;
                if (*newValue < 0)
                {
                    *newValue *= (-1);
                }
                if (*oldValue != noData)
                {
                    pExtendedDEM->GetRasterBand(1)->RasterIO(GF_Write,j,i,1,1,newValue,1,1,GDT_Float32,0,0);
                }
            }
        }
    }
    else if (nDirUSbound == 4)
    {
        nRowsExt = nRows;
        nColsExt = nCols + nAddCells;
        pExtendedDEM = pDriverTIFF->Create(qsExtendedDemPath.toStdString().c_str(), nColsExt, nRowsExt, 1, GDT_Float32, papszOptions);
        pExtendedDEM->GetRasterBand(1)->Fill(noData);

        dSlope = Raster.averageSlope_ColAve(qsDelftDemPath.toStdString().c_str(), nCols-1, 0);
        extSlope = dSlope/2.0;

        openSourceDEM();

        pSourceDEM->GetGeoTransform(extTransform);
        transAmt = nAddCells*cellWidth;
        if (transAmt < 0)
        {
            transAmt *= (-1);
        }
        extTransform[0] -= transAmt;
        pExtendedDEM->SetGeoTransform(extTransform);

        //copy data from DEM
        for (int i=0; i<nRows; i++)
        {
            for (int j=0; j<nCols; j++)
            {
                pSourceDEM->GetRasterBand(1)->RasterIO(GF_Read,j,i,1,1,oldValue,1,1,GDT_Float32,0,0);
                pExtendedDEM->GetRasterBand(1)->RasterIO(GF_Write,j+nAddCells,i,1,1,oldValue,1,1,GDT_Float32,0,0);
            }
        }

        //add new data to extended DEM
        for (int i=0; i<nRows; i++)
        {
            for (int j=nAddCells-1; j>=0; j--)
            {
                pExtendedDEM->GetRasterBand(1)->RasterIO(GF_Read,j+1,i,1,1,oldValue,1,1,GDT_Float32,0,0);
                *newValue = *oldValue + extSlope * cellHeight;
                if (*newValue < 0)
                {
                    *newValue *= (-1);
                }
                if (*oldValue != noData)
                {
                    pExtendedDEM->GetRasterBand(1)->RasterIO(GF_Write,j,i,1,1,newValue,1,1,GDT_Float32,0,0);
                }
            }
        }
    }
    else
    {

    }

    GDALClose(pSourceDEM);
    GDALClose(pExtendedDEM);

    CPLFree(oldValue);
    CPLFree(newValue);
    oldValue = NULL;
    newValue = NULL;
}

QVector<int> MORPH_Delft3DIO::getDisColVector()
{
    return xUSCoords;
}

QVector<int> MORPH_Delft3DIO::getDisRowVector()
{
    return yUSCoords;
}

void MORPH_Delft3DIO::run()
{
    QFile xvt(qsInputPath + "/" + qsFloodName + "/Hydraulics/Xvelocity.xyz"),
            yvt(qsInputPath + "/" + qsFloodName + "/Hydraulics/Yvelocity.xyz"),
            sst(qsInputPath + "/" + qsFloodName + "/Hydraulics/BedShearStress.xyz"),
            wdt(qsInputPath + "/" + qsFloodName + "/Hydraulics/WaterDepth.xyz"),
            data(qsInputPath + "/" + qsFloodName + "/Delft3D/trim-" + qsFloodName + ".dat");
    bool exist1, exist2, exist3, exist4, existData;
    int count;

    MORPH_Raster Raster;

    extendDEMBoundary();

    newDemMax = Raster.findMax(qsExtendedDemPath.toStdString().c_str());

    calculateDSWSE();

    GDALAllRegister();

    setDownstreamBoundary();
    setDischargePoints();
    setObservationPoints();
    setOutputPaths();
    writeXYZ();
    writeBCQ();
    writeBND();
    writeDEP();
    writeDIS();
    writeENC();
    writeFIL();
    writeGRD();
    writeINI();
    writeMDF();
    writeOBS();
    writeOutputMacro();
    writeSRC();

    QString delftName, qpName;
    QStringList delftParams, qpParams;
    delftName = "deltares_hydro.exe";
    qpName = "d3d_qp.exe";
    delftParams << qsFloodName + ".ini";
    qpParams << "run" << qsFloodName + ".m";

    qDebug()<<"starting delft";
    QDir::setCurrent(qsInputPath + "/" + qsFloodName + "/Delft3D");
    processDelft.start(delftName, delftParams);
    processDelft.waitForStarted(-1);
    nPID = processDelft.processId();
    processDelft.waitForFinished(-1);
    qDebug()<<"delft done";
    QString qpQuit = "TASKKILL /F /IM d3d_qp.exec";

    //QThread::currentThread()->sleep(2);

    processQp.start(qpName, qpParams);
    processQp.waitForStarted(-1);
    int qpPid = processQp.processId();
    qpQuit = "TASKKILL /pid " + QString::number(qpPid);
    processQp.waitForFinished(-1);
    QThread::currentThread()->sleep(2);

    existData = data.exists();

    if (existData)
    {

        exist1 = xvt.exists(), exist2 = yvt.exists(), exist3 = sst.exists(), exist4 = wdt.exists();

        if (!exist1 || !exist2 || !exist3 || !exist4)
        {
            count = 0;

            while ((!exist1 || !exist2 || !exist3 || !exist4) && count < 500)
            {
                processQp.start(qpName, qpParams);
                processQp.waitForStarted(-1);
                qpQuit = "TASKKILL /pid " + QString::number(qpPid);
                processQp.waitForFinished(-1);
                QThread::currentThread()->sleep(2);
                exist1 = xvt.exists(), exist2 = yvt.exists(), exist3 = sst.exists(), exist4 = wdt.exists();

                if (!exist1 || !exist2 || !exist3 || !exist4)
                {
                    QThread::currentThread()->sleep(1);
                    exist1 = xvt.exists(), exist2 = yvt.exists(), exist3 = sst.exists(), exist4 = wdt.exists();
                    if (!exist1 || !exist2 || !exist3 || !exist4)
                    {
                        QThread::currentThread()->sleep(1);
                    }
                    else
                    {
                        processQp.execute(qpQuit);
                    }
                }
                else
                {
                    processQp.execute(qpQuit);
                }
                count++;
                exist1 = xvt.exists(), exist2 = yvt.exists(), exist3 = sst.exists(), exist4 = wdt.exists();
                if ((!exist1 || !exist2 || !exist3 || !exist4) && count == 499)
                {
                    qDebug()<<"ERROR: Hydraulics files do not exist after 500 attempts\n";
                }
                else
                {
                    processQp.execute(qpQuit);
                }
                qDebug()<<"count "<<count;
            }
        }
        else
        {
            processQp.execute(qpQuit);
        }
    }
    else
    {
        qDebug()<<"Delft3D data file does not exist";
    }
}

void MORPH_Delft3DIO::setDischargePoints()
{
    MORPH_Raster Raster;

    double meanDepth, area, chezy, conveyance, depth, diffQ;
    double conveyanceTotal = 0, estQ = 0;
    int xAdd, yAdd, nCells;
    int count = 0;
    bool stop = false;

    float *row;

    depth = newDemMax + dswe[nCurrentIteration];

    openSourceDEM();

    if (nDirUSbound == 1)
    {
        nCells = nCols;
        row = (float*) CPLMalloc(sizeof(float)*nCells);
        pSourceDEM->GetRasterBand(1)->RasterIO(GF_Read,0,3,nCols,1,row,nCols,1,GDT_Float32,0,0);
        xAdd = 0, yAdd = nRows + (nAddCells-3);
    }
    else if (nDirUSbound == 2)
    {
        nCells = nCols;
        row = (float*) CPLMalloc(sizeof(float)*nCells);
        pSourceDEM->GetRasterBand(1)->RasterIO(GF_Read,0,nRows-3,nCols,1,row,nCols,1,GDT_Float32,0,0);
        xAdd = 0, yAdd = 4;
    }
    else if (nDirUSbound == 3)
    {

    }
    else if (nDirUSbound == 4)
    {

    }
    else
    {

    }

    while (!stop && count < 10000000)
    {
        conveyanceTotal = 0.0;
        for (int i=0; i<nCells; i++)
        {
            meanDepth = depth - row[i];

            if (meanDepth > 0 && row[i] != noData)
            {
                area = cellWidth * meanDepth;
                chezy = 18.0*log10((12.0*meanDepth)/roughness);
                conveyance = chezy * sqrt(meanDepth) * area;
                conveyanceTotal += conveyance;
            }
        }

        estQ = conveyanceTotal * sqrt(dSlope);
        diffQ = estQ - q[nCurrentIteration];

        if (diffQ > (0.05*q[nCurrentIteration]) || diffQ < (-0.05*q[nCurrentIteration]))
        {
            if (diffQ > 0.0)
            {
                depth -= 0.0001;
            }
            else
            {
                depth += 0.0001;
            }
        }
        else
        {
            stop = true;
        }

        if (count == 9999999)
        {
            qDebug()<<"DISCHARGE CELLS: too many iterations to find correct dswe, using provided value "<<depth<<" "<<q[nCurrentIteration]<<" "<<estQ<<" "<<meanDepth<<" "<<chezy<<" "<<conveyanceTotal;
        }
        count++;
    }

    xUSCoords.clear();
    yUSCoords.clear();
    disCount = 0, count = 0;
    disMin = 5;

    for (int i=0; i<nCells; i++)
    {
        if (row[i] != noData)
        {
            count++;

            if (row[i] < depth && count > disMin && count < (disMax-disMin))
            {
                if(nDirUSbound == 1 || nDirUSbound == 2)
                {
                    xUSCoords.append(i);
                    yUSCoords.append(yAdd);
                    disCount++;
                }
                else
                {

                }
            }
        }
    }

    disCount -= 3;
    qDebug()<<"DIS COUNT: "<<disCount;

    GDALClose(pSourceDEM);

    CPLFree(row);
    row = NULL;
}

void MORPH_Delft3DIO::setDownstreamBoundary()
{
    openSourceDEM();

    int staticIndex, index1 = 0, index2 = 0, nCells;
    bool found1 = false, found2 = false, found3 = false, found4 = false, found5 = false, found6 = false;

    float *row, *row2;

    if (nDirDSbound == 1)
    {
        nCells = nCols;
        row = (float*) CPLMalloc(sizeof(float)*nCells);
        row2 = (float*) CPLMalloc(sizeof(float)*nCells);
        pSourceDEM->GetRasterBand(1)->RasterIO(GF_Read,0,0,nCells,1,row,nCells,1,GDT_Float32,0,0);
        pSourceDEM->GetRasterBand(1)->RasterIO(GF_Read,0,0,nCells,1,row2,nCells,1,GDT_Float32,0,0);
        staticIndex = nRowsExt + 2;
    }
    else if (nDirDSbound == 2)
    {
        nCells = nCols;
        row = (float*) CPLMalloc(sizeof(float)*nCells);
        row2 = (float*) CPLMalloc(sizeof(float)*nCells);
        pSourceDEM->GetRasterBand(1)->RasterIO(GF_Read,0,nRows-1,nCells,1,row,nCells,1,GDT_Float32,0,0);
        pSourceDEM->GetRasterBand(1)->RasterIO(GF_Read,0,nRows-1,nCells,1,row2,nCells,1,GDT_Float32,0,0);
        staticIndex = 1;
    }
    else if (nDirDSbound == 3)
    {

    }
    else if (nDirDSbound == 4)
    {

    }

    dsCoords.clear();

    int count = 0;

    while ((!found3 || !found6) && count<nCells)
    {
        if(!found1 || !found4)
        {
            if (row[count] != noData)
            {
                index1 = count;
                found1 = true;
            }
            if (row2[count] != noData)
            {
                disMin = count;
                found4 = true;
            }
        }
        else
        {
            if (row[count] == noData)
            {
                index2 = count-1;
                found2 = true;
            }
            else if (count == nCells - 1)
            {
                index2 = count;
                found2 = true;
            }
            if (row2[count] == noData)
            {
                disMax = count-1;
                found5 = true;
            }
            else if (count == nCells - 1)
            {
                disMax = count;
                found5 = true;
            }
        }
        if (found1 && found2)
        {
            found3 = true;
        }
        if (found4 && found5)
        {
            found6 = true;
        }
        count++;
    }

    dsCoords.append(index1);
    dsCoords.append(staticIndex);
    dsCoords.append(index2);
    dsCoords.append(staticIndex);
    disMax -= 5;
    disMin += 5;

    GDALClose(pSourceDEM);

    CPLFree(row);
    CPLFree(row2);
}

void MORPH_Delft3DIO::setObservationPoints()
{
    yObsCoords.resize(3);
    xObsCoords.resize(3);
    for (int i=0; i<3; i++)
    {
        yObsCoords[i] = 30 + i;
        xObsCoords[i] = 110 + i;
    }
}

void MORPH_Delft3DIO::setOriginPoint()
{
    if (nDirUSbound == 1)
    {
        xOrigin = topLeftX;
        yOrigin = topLeftY - (nRows*cellWidth);
    }
    else if (nDirUSbound == 2)
    {
        xOrigin = topLeftX;
        yOrigin = topLeftY - ((nRows+nAddCells)*cellWidth);
    }
    else if (nDirUSbound == 3)
    {
        xOrigin = topLeftX;
        yOrigin = topLeftY - (nRows*cellWidth);
    }
    else if (nDirUSbound == 4)
    {
        xOrigin = topLeftX - (nAddCells*cellWidth);
        yOrigin = topLeftY - (nRows*cellWidth);
    }
    else
    {

    }
}

void MORPH_Delft3DIO::setOutputPaths()
{
    qsBnd = qsInputPath + "/" + qsFloodName + "/Delft3D/" + qsFloodName + ".bnd";
    qsBcq = qsInputPath + "/" + qsFloodName + "/Delft3D/" + qsFloodName + ".bcq";
    qsDep = qsInputPath + "/" + qsFloodName + "/Delft3D/" + qsFloodName + ".dep";
    qsDis = qsInputPath + "/" + qsFloodName + "/Delft3D/" + qsFloodName + ".dis";
    qsEnc = qsInputPath + "/" + qsFloodName + "/Delft3D/" + qsFloodName + ".enc";
    qsFil = qsInputPath + "/" + qsFloodName + "/Delft3D/" + qsFloodName + ".fil";
    qsGrd = qsInputPath + "/" + qsFloodName + "/Delft3D/" + qsFloodName + ".grd";
    qsIni = qsInputPath + "/" + qsFloodName + "/Delft3D/" + qsFloodName + ".ini";
    qsMdf = qsInputPath + "/" + qsFloodName + "/Delft3D/" + qsFloodName + ".mdf";
    qsObs = qsInputPath + "/" + qsFloodName + "/Delft3D/" + qsFloodName + ".obs";
    qsMacro = qsInputPath + "/" + qsFloodName + "/Delft3D/" + qsFloodName + ".m";
    qsSrc = qsInputPath + "/" + qsFloodName + "/Delft3D/" + qsFloodName + ".src";
    qsXyz = qsInputPath + "/" + qsFloodName + "/Delft3D/" + qsFloodName + ".xyz";
}

void MORPH_Delft3DIO::setXDischargeAddresses()
{
    int addedCells = 0, count = 0, addedRows = 0;

    //find which address has the higher value and assign it as start
    if(nusx1>nusx2)
    {
        //move edge cells 1 cell into channel to prevent discharge on dry cell
        xstart = nusx2+1;
        xend = nusx1-1;
    }
    else
    {
        xstart = nusx1+1;
        xend = nusx2-1;
    }
    //determine number of cells along boundary
    count = xend-xstart;
    //if less than 10 boundary cells, add cells to boundary
    if (count < 10)
    {
        //add a row of count-2 cells, subract 2 in case channel turns
        addedCells = count-2;
        count += addedCells;
        //update added rows
        addedRows = 1;
        //check if count is still less than 10
        if (count < 10)
        {
            //add another row of the same size
            count += addedCells;
            addedRows = 2;
            //update added cells
            addedCells *= 2;
        }
    }

    //assign x address for all discharge elements and add to vector
    if (nDirUSbound == 1)
    {
        for (int i=0; i<count; i++)
        {
            //first row of cells
            if (i < count-addedCells || addedRows == 0)
            {
                xUSCoords.append(xstart+i);
                yUSCoords.append(nusy1);
            }
            else if (i < count - (addedCells/addedRows) || addedRows == 1)
            {
                xUSCoords.append(xstart+(i-(addedCells/addedRows))+1);
                yUSCoords.append(nusy1+1);
            }
            else
            {
                xUSCoords.append(xstart+(i-addedCells)+1);
                yUSCoords.append(nusy1+2);
            }
        }
    }
    else if (nDirUSbound == 2)
    {
        for (int i=0; i<count; i++)
        {
            //first row of cells
            if (i < count-addedCells || addedRows == 0)
            {
                xUSCoords.append(xstart+i);
                yUSCoords.append(nusy1-nAddCells);
            }
            else if (i < count - (addedCells/addedRows) || addedRows == 1)
            {
                xUSCoords.append(xstart+(i-(addedCells/addedRows))-1);
                yUSCoords.append(nusy1+1-nAddCells);
            }
            else
            {
                xUSCoords.append(xstart+(i-addedCells)-1);
                yUSCoords.append(nusy1+2-nAddCells);
            }
        }
    }
    else if (nDirUSbound == 3)
    {

    }
    else if (nDirUSbound == 4)
    {

    }
    else
    {

    }
    disCount = count;
}

void MORPH_Delft3DIO::setYDischargeAddresses()
{
    int addedCells = 0, count = 0, addedCols = 0;

    if (nusy1 > nusy2)
    {
        ystart = nusy1+1;
        yend = nusy2-1;
    }
    else
    {
        ystart = nusy2+1;
        yend = nusy1-1;
    }
    count = yend-ystart;

    if (count < 10)
    {
        addedCells = count-2;
        count += addedCells;
        addedCols = 1;
        if (count < 10)
        {
            count += addedCells;
            addedCols = 2;
            addedCells *= 2;
        }
    }

    if (nDirUSbound == 3)
    {
        for (int i=0; i<count; i++)
        {
            if (i < count-addedCells || addedCols==0)
            {
                yUSCoords.append(ystart-i);
                xUSCoords.append(nusx1);
            }
            else if (i < count - (addedCells/addedCols))
            {
                yUSCoords.append(ystart-i-1);
                xUSCoords.append(nusx1-1);
            }
            else
            {
                yUSCoords.append(ystart-i-1);
                xUSCoords.append(nusx1-2);
            }
        }
    }
    else if (nDirUSbound == 4)
    {
        for (int i=0; i<count; i++)
        {
            if (i < count-addedCells || addedCols==0)
            {
                yUSCoords.append(ystart-i);
                xUSCoords.append(nusx1);
            }
            else if (i < count - (addedCells/addedCols))
            {
                yUSCoords.append(ystart-i-1);
                xUSCoords.append(nusx1+1);
            }
            else
            {
                yUSCoords.append(ystart-i-1);
                xUSCoords.append(nusx1+2);
            }
        }
    }
    disCount = count;
}

void MORPH_Delft3DIO::writeBND()
{
    QFile fout(qsBnd);
    fout.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&fout);

    out<<"("<<dsCoords[0]<<","<<dsCoords[1]<<")..("<<dsCoords[2]<<","<<dsCoords[3]<<")      Z Q    "<<dsCoords[0]<<"    "<<dsCoords[1]<<"    "<<dsCoords[2]<<"    "<<dsCoords[3]<<"    0.0";

    fout.close();
}

void MORPH_Delft3DIO::writeBCQ()
{
    QFile fout(qsBcq);
    fout.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&fout);

    out<<"table-name\t\t\t'Boundary Section : 1'\n";
    out<<"contents\t\t\t'Uniform             '\n";
    out<<"location\t\t\t'("<<dsCoords[0]<<","<<dsCoords[1]<<")..("<<dsCoords[2]<<","<<dsCoords[3]<<") '\n";
    out<<"xy-function\t\t\t'equidistant'\n";
    out<<"interpolation\t\t'linear'\n";
    out<<"parameter\t\t\t'total discharge (t) end A'\t\t\tunit '[m3/s]'\n";
    out<<"parameter\t\t\t'water elevation (z) end B'\t\t\tunit '[m]'\n";
    out<<"records-in-table\t2\n";
    out<<"\t"<<q[nCurrentIteration]<<"\t"<<dswe[nCurrentIteration]<<"\n";
    out<<"\t"<<q[nCurrentIteration]<<"\t"<<dswe[nCurrentIteration]<<"\n";

    fout.close();
}

void MORPH_Delft3DIO::writeDEP()
{
    MORPH_Raster Raster;

    QFile fout(qsDep);
    fout.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&fout);
    int count = 0;

    float *value = (float*) CPLMalloc(sizeof(float)*1);

    out.setRealNumberNotation(QTextStream::ScientificNotation);
    out.setRealNumberPrecision(7);

    openExtendedDEM();

    //add row of no data values to top of file
    out<<DELFT_NODATA<<"\t";
    count++;

    for (int i=0; i<nColsExt; i++)
    {
        out<<DELFT_NODATA<<"\t";
        count++;
        if (count == 12)
        {
            out<<"\n";
            count = 0;
        }
    }
    out<<DELFT_NODATA;
    out<<"\n";

    for (int i=nRowsExt-1; i>=0; i--)
    {
        //add column of no data values to edge of file
        count = 0;
        out<<DELFT_NODATA<<"\t";
        count++;

        for (int j=0; j<nColsExt; j++)
        {
            pExtendedDEM->GetRasterBand(1)->RasterIO(GF_Read,j,i,1,1,value,1,1,GDT_Float32,0,0);
            if (*value >= 0)
            {
                out<<" ";
            }
            if (*value == noData)
            {
                out<<DELFT_NODATA<<"\t";
            }
            else
            {
                *value = (*value - newDemMax) * (-1.0);
                out<<*value<<"\t";
            }
            count++;
            if (count == 12)
            {
                out<<"\n";
                count = 0;
            }
        }
        out<<DELFT_NODATA;
        out<<"\n";
    }
    //add row of no data values to bottom of file
    count = 0;
    out<<DELFT_NODATA<<"\t";
    count++;
    for (int i=0; i<nColsExt; i++)
    {
        out<<DELFT_NODATA<<"\t";
        count++;
        if (count == 12)
        {
            out<<"\n";
            count = 0;
        }
    }
    out<<DELFT_NODATA;
    out<<"\n";

    fout.close();

    GDALClose(pExtendedDEM);

    CPLFree(value);
    value = NULL;
}

void MORPH_Delft3DIO::writeDIS()
{
    QFile fout(qsDis);
    fout.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&fout);
    QDateTime Itdate;
    Itdate = QDateTime::fromTime_t(date[nCurrentIteration]);

    for (int i=0; i<(disCount); i++)
    {
        out<<"table-name\t\t\t 'Discharge : "<<i+1<<"'\n";
        out<<"contents\t\t\t 'regular   '\n";
        out<<"location\t\t\t '("<<xUSCoords[i]<<","<<yUSCoords[i]<<")             '\n";
        out<<"time-function\t\t 'non-equidistant'\n";
        out<<"reference-time\t\t "<<Itdate.toString("yyyyMMdd")<<"\n";
        out<<"time-unit\t\t\t 'minutes'\n";
        out<<"interpolation\t\t 'linear'\n";
        out<<"parameter\t\t\t 'time                '                     unit '[min]'\n";
        out<<"parameter\t\t\t 'flux/discharge rate '                     unit '[m3/s]'\n";
        out<<"records-in-table\t 2\n";
        out.setRealNumberNotation(QTextStream::ScientificNotation);
        out<<" "<<"0.0"<<"\t"<<q[nCurrentIteration]/(disCount)<<"\n";
        out<<" "<<simTime<<"\t"<<q[nCurrentIteration]/(disCount)<<"\n";
        out.setRealNumberNotation(QTextStream::FixedNotation);
    }
    fout.close();
}

void MORPH_Delft3DIO::writeENC()
{
    QFile fout(qsEnc);
    fout.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&fout);

    out<<"\t 1\t1\t*** begin external enclosure\n";
    out<<"\t"<<nColsExt+2<<"\t1\n";
    out<<"\t"<<nColsExt+2<<"\t"<<nRowsExt+2<<"\n";
    out<<"\t 1\t"<<nRowsExt+2<<"\n";
    out<<"\t 1\t1\t*** end external grid enclosure\n";
}

void MORPH_Delft3DIO::writeFIL()
{
    QFile fout(qsFil);
    fout.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&fout);

    out<<"Domain, Checked : Yes\n";
    out<<"\t Grid : "<<qsFloodName<<".grd\n";
    out<<"\t Grid enclosure : "<<qsFloodName<<".enc\n";
    out<<"\t Bathymetry : "<<qsFloodName<<".dep\n";
    out<<"\t Dry points : none\n";
    out<<"\t Thin dams : none\n";
    out<<"\t "<<endl;

    out<<"Time frame, Checked : Yes\n\n";

    out<<"Processes, Checked : Yes\n\n";

    out<<"Initial conditions, Checked : Yes\n\n";

    out<<"Boundaries, Checked : Yes\n";
    out<<"\t Boundary definitions : "<<qsFloodName<<".bnd\n";
    out<<"\t Astronomical flow conditions : none\n";
    out<<"\t Astronomical corrections : none\n";
    out<<"\t Harmonic flow conditions : none\n";
    out<<"\t QH-relation flow conditions : "<<qsFloodName<<".bcq\n";
    out<<"\t Time series flow conditions : none\n";
    out<<"\t Transport conditions : none\n";
    out<<"\t \n";

    out<<"Physical parameters, Checked : Yes\n";
    out<<"\t Roughness coefficients : none\n";
    out<<"\t Hor. viscosity/diffusivity : none\n";
    out<<"\t Heat flux model data : none\n";
    out<<"\t Sediment data : none\n";
    out<<"\t Morphology data : none\n";
    out<<"\t Uniform wind data : none\n";
    out<<"\t Space varying wind data : none\n";
    out<<"\t \n";

    out<<"Numerical parameters, Checked : Yes\n\n";

    out<<"Operations, Checked : Yes\n";
    out<<"\t Discharge definitions : "<<qsFloodName<<".src\n";
    out<<"\t Discharge data : "<<qsFloodName<<".dis\n";
    out<<"\t Dredging and dumping data : none\n";
    out<<"\t \n";

    out<<"Monitoring, Checked : Yes\n";
    out<<"\t Observation points : "<<qsFloodName<<".obs\n";
    out<<"\t Drogues : none\n";
    out<<"\t Cross-sections : none\n";
    out<<"\t \n";

    out<<"Additional parameters, Checked : Yes\n\n";

    out<<"Output, Checked : No\n";
    out<<"\t Fourier analysis data : none\n";
    out<<"\t ";

    fout.close();
}

void MORPH_Delft3DIO::writeGRD()
{
    QFile fout(qsGrd);
    fout.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&fout);

    QDateTime currentDT = QDateTime::currentDateTime();
    QString dt = currentDT.toString("yyyy-MM-dd, hh:mm:ss");
    int count;

    out<<"*"<<endl;
    out<<"* Deltares, Delft3D-RGFGRID Version 4.18.01.18383, Sep  7 2011, 14:23:22\n";
    out<<"* File creation date: "<<dt<<"\n";
    out<<"* \n";
    out<<"Coordinate System = Cartesian\n";
    out<<"\t "<<nColsExt+1<<"\t "<<nRowsExt+1<<"\n";
    out<<" 0 0 0"<<"\n";

    out.setRealNumberNotation(QTextStream::ScientificNotation);
    out.setRealNumberPrecision(15);
    for (int i=0; i<nRowsExt+1; i++)
    {
        count = 0;
        out<<" ETA=\t"<<i+1<<"\t";

        for (int j=0; j<nColsExt+1; j++)
        {
            out<<xOrigin+(j*cellWidth)<<"\t";
            count++;
            if (count == 5)
            {
                out<<"\n\t\t\t";
                count = 0;
            }
        }
        out<<"\n";
    }

    for (int i=0; i<nRowsExt+1; i++)
    {
        count = 0;
        out<<" ETA=\t"<<i+1<<"\t";

        for (int j=0; j<nColsExt+1; j++)
        {
            out<<yOrigin+(i*cellWidth)<<"\t";
            count++;
            if (count == 5)
            {
                out<<"\n\t\t\t";
                count = 0;
            }
        }
        out<<endl;
    }

    fout.close();
}

void MORPH_Delft3DIO::writeINI()
{
    QFile fout(qsIni);
    fout.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&fout);

    out<<"[Compnent]\n";
    out<<"Name = flow2d3d\n";
    out<<"MDFfile = "<<qsFloodName<<".mdf";

    fout.close();
}

void MORPH_Delft3DIO::writeMDF()
{
    QFile fout(qsMdf);
    fout.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&fout);
    QDateTime Itdate;
    Itdate = QDateTime::fromTime_t(date[nCurrentIteration]);

    out.setRealNumberNotation(QTextStream::ScientificNotation);
    out.setRealNumberPrecision(7);

    out<<"Ident  = #Delft3D-FLOW 3.43.05.22651#"<<"\n";
    out<<"Commnt = "<<"\n";
    out<<"Runtxt = "<<"\n";
    out<<"Filcco = #"<<qsFloodName<<".grd#"<<"\n";
    out<<"Anglat =  0.0"<<"\n";
    out<<"Grdang =  0.0"<<"\n";
    out<<"Filgrd = #"<<qsFloodName<<".enc#"<<"\n";
    out<<"MNKmax = "<<nColsExt+2<<" "<<nRowsExt+2<<" "<<"1.0\n";//cellWidth<<"\n";
    out<<"Thick  =  100.0"<<"\n";
    out<<"Commnt = "<<"\n";
    out<<"Fildep = #"<<qsFloodName<<".dep#"<<"\n";
    out<<"Commnt = "<<"\n";
    out<<"Commnt =                 no. dry points: 0"<<"\n";
    out<<"Commnt =                 no. thin dams: 0"<<"\n";
    out<<"Commnt = "<<"\n";
    out<<"Itdate = #"<<Itdate.toString("yyyy-MM-dd")<<"#"<<"\n";
    out<<"Tunit  = #M#"<<"\n";
    out<<"Tstart =  "<<0.0<<"\n";
    out<<"Tstop  =  "<<simTime<<"\n";
    out<<"Dt     = "<<timeStep<<"\n";
    out<<"Tzone  = 0"<<"\n";
    out<<"Commnt = "<<"\n";
    out<<"Sub1   = #    #"<<"\n";
    out<<"Sub2   = #   #"<<"\n";
    out<<"Commnt = "<<"\n";
    out<<"Wnsvwp = #N#"<<"\n";
    out<<"Wndint = #Y#"<<"\n";
    out<<"Commnt = "<<"\n";
    out<<"Zeta0  = "<<dswe[nCurrentIteration]<<"\n";
    out<<"Commnt = "<<"\n";
    out<<"Commnt = no. open boundaries: 1"<<"\n";
    out<<"Filbnd = #"<<qsFloodName<<".bnd#"<<"\n";
    out<<"FilbcQ = #"<<qsFloodName<<".bcq#"<<"\n";
    out<<"Commnt = "<<"\n";
    out<<"Ag     =  9.81"<<"\n";
    out<<"Rhow   =  1000.0"<<"\n";
    out<<"Tempw  =  15.0"<<"\n";
    out<<"Salw   =  31.0"<<"\n";
    out<<"Wstres =  0.00063 0 0.00723 100.0 0.00723 100.0"<<"\n";
    out<<"Rhoa   =  1.0"<<"\n";
    out<<"Betac  =  0.5"<<"\n";
    out<<"Equil  = #N#"<<"\n";
    out<<"Ktemp  = 0"<<"\n";
    out<<"Fclou  =  0.0"<<"\n";
    out<<"Sarea  =  0.0"<<"\n";
    out<<"Temint = #Y#"<<"\n";
    out<<"Commnt = "<<"\n";
    out<<"Roumet = #W#"<<"\n";
    out<<"Ccofu  =  "<<roughness <<"\n";
    out<<"Ccofv  =  "<<roughness <<"\n";
    out<<"Xlo    =  0.0"<<"\n";
    out<<"Vicouv =  "<<hev <<"\n";
    out<<"Dicouv =  10.0"<<"\n";
    out<<"Htur2d = #N#"<<"\n";
    out<<"Irov   = 0"<<"\n";
    out<<"Commnt = "<<"\n";
    out<<"Iter   =      2"<<"\n";
    out<<"Dryflp = #YES#"<<"\n";
    out<<"Dpsopt = #DP#"<<"\n";
    out<<"Dpuopt = #MIN#"<<"\n";
    out<<"Dryflc =  0.05"<<"\n";
    out<<"Dco    = -999.0"<<"\n";
    out<<"Tlfsmo = 1.0"<<"\n";
    out<<"ThetQH = 0.0"<<"\n";
    out<<"Forfuv = #Y#"<<"\n";
    out<<"Forfuw = #N#"<<"\n";
    out<<"Sigcor = #N#"<<"\n";
    out<<"Trasol = #Cyclic-method#"<<"\n";
    out<<"Momsol = #Cyclic#"<<"\n";
    out<<"Commnt = "<<"\n";
    out<<"Commnt =                 no. discharges: 5"<<"\n";
    out<<"Filsrc = #"<<qsFloodName<<".src#"<<"\n";
    out<<"Fildis = #"<<qsFloodName<<".dis#"<<"\n";
    out<<"Commnt =                 no. observation points: 3"<<"\n";
    out<<"Filsta = #"<<qsFloodName<<".obs#"<<"\n";
    out<<"Commnt =                 no. drogues: 0"<<"\n";
    out<<"Commnt = "<<"\n";
    out<<"Commnt = "<<"\n";
    out<<"Commnt =                 no. cross sections: 0"<<"\n";
    out<<"Commnt = "<<"\n";
    out<<"SMhydr = #YYYYY#"<<"\n";
    out<<"SMderv = #YYYYYY#"<<"\n";
    out<<"SMproc = #YYYYYYYYYY#"<<"\n";
    out<<"PMhydr = #YYYYYY#"<<"\n";
    out<<"PMderv = #YYY#"<<"\n";
    out<<"PMproc = #YYYYYYYYYY#"<<"\n";
    out<<"SHhydr = #YYYY#"<<"\n";
    out<<"SHderv = #YYYYY#"<<"\n";
    out<<"SHproc = #YYYYYYYYYY#"<<"\n";
    out<<"SHflux = #YYYY#"<<"\n";
    out<<"PHhydr = #YYYYYY#"<<"\n";
    out<<"PHderv = #YYY#"<<"\n";
    out<<"PHproc = #YYYYYYYYYY#"<<"\n";
    out<<"PHflux = #YYYY#"<<"\n";
    out<<"Online = #N#"<<"\n";
    out<<"Wagmod = #N#"<<"\n";
    out<<"Flmap  =  0.0 "<<simTime<<" "<<simTime<<"\n";
    out<<"Flhis  =  0.0 "<<simTime<<" "<<simTime<<"\n";
    out<<"Flpp   =  0.0 "<<simTime<<" "<<simTime<<"\n";
    out<<"Flrst  = "<<simTime<<"\n";
    out<<"Commnt = "<<"\n";
    out<<"Commnt = "<<"\n";

    fout.close();
}

void MORPH_Delft3DIO::writeOBS()
{
    QFile fout(qsObs);
    fout.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&fout);

    out<<"("<<xObsCoords[0]<<","<<yObsCoords[0]<<")               "<<xObsCoords[0]<<"     "<<yObsCoords[0]<<"\n";
    out<<"("<<xObsCoords[1]<<","<<yObsCoords[1]<<")               "<<xObsCoords[1]<<"     "<<yObsCoords[1]<<"\n";
    out<<"("<<xObsCoords[2]<<","<<yObsCoords[2]<<")               "<<xObsCoords[2]<<"     "<<yObsCoords[2]<<"\n";

    fout.close();
}

void MORPH_Delft3DIO::writeOutputMacro()
{
    QFile fout(qsMacro);
    fout.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&fout);

    out<<"d3d_qp('openfile', '"+qsInputPath+"/"+qsFloodName+"/Delft3D/trim-"<<qsFloodName<<".dat')\n";
    out<<"d3d_qp('selectfield','depth averaged velocity')\n";
    out<<"d3d_qp('component','x component')\n";
    out<<"d3d_qp('exporttype','sample file')\n";
    out<<"d3d_qp('exportdata','"+qsInputPath+"/"+qsFloodName+"/Hydraulics/Xvelocity.xyz')\n";

    out<<"d3d_qp('selectfield','depth averaged velocity')\n";
    out<<"d3d_qp('component','y component')\n";
    out<<"d3d_qp('exporttype','sample file')\n";
    out<<"d3d_qp('exportdata','"<<qsInputPath+"/"+qsFloodName+"/Hydraulics/Yvelocity.xyz')\n";

    out<<"d3d_qp('selectfield','water depth')\n";
    out<<"d3d_qp('exporttype','sample file')\n";
    out<<"d3d_qp('exportdata','"+qsInputPath+"/"+qsFloodName+"/Hydraulics/WaterDepth.xyz')\n";

    out<<"d3d_qp('selectfield','bed shear stress')\n";
    out<<"d3d_qp('exporttype','sample file')\n";
    out<<"d3d_qp('exportdata','"+qsInputPath+"/"+qsFloodName+"/Hydraulics/BedShearStress.xyz')\n";

    fout.close();
}

void MORPH_Delft3DIO::writeSRC()
{
    QFile fout(qsSrc);
    fout.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&fout);

    for (int i=0; i<disCount; i++)
    {
        out<<"("<<xUSCoords[i]<<","<<yUSCoords[i]<<")              Y     "<<xUSCoords[i]<<"     "<<yUSCoords[i]<<"     "<<"0    N\n";
    }
    fout.close();
}

void MORPH_Delft3DIO::writeXYZ()
{
    MORPH_Raster Raster;

    double dMax, dInvertValue;
    double xtlCenter, ytlCenter, xCenter, yCenter;

    dMax = Raster.findMax(qsExtendedDemPath.toStdString().c_str());

    xtlCenter = topLeftX + (cellWidth/2);
    ytlCenter = topLeftY + (cellHeight/2);

    float *read = (float*) CPLMalloc(sizeof(float)*1);

    QFile fout(qsXyz);
    fout.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&fout);
    out.setRealNumberNotation(QTextStream::FixedNotation);

    openExtendedDEM();

    for (int i=0; i<nRowsExt; i++)
    {
        yCenter = ytlCenter + (i*cellHeight);
        for (int j=0; j<nColsExt; j++)
        {
            pExtendedDEM->GetRasterBand(1)->RasterIO(GF_Read,j,i,1,1,read,1,1,GDT_Float32,0,0);
            if (*read != noData)
            {
                dInvertValue = (*read - newDemMax) * (-1.0);
                xCenter = xtlCenter + (j*cellWidth);
                out.setRealNumberPrecision(5);
                out << xCenter << "\t" <<yCenter << "\t" << dInvertValue <<"\n";
            }
        }
    }
    fout.close();

    GDALClose(pExtendedDEM);

    CPLFree(read);
}

int MORPH_Delft3DIO::xCellAddress(double coord)
{
    int address;
    address = round((coord - xOrigin) / cellWidth);
    return address;
}

int MORPH_Delft3DIO::yCellAddress(double coord)
{
    int address;
    address = round((coord - yOrigin) / cellWidth);
    return address;
}

void MORPH_Delft3DIO::openExtendedDEM()
{
    pExtendedDEM = (GDALDataset*) GDALOpen(qsExtendedDemPath.toStdString().c_str(), GA_ReadOnly);
}

void MORPH_Delft3DIO::openSourceDEM()
{
    pSourceDEM = (GDALDataset*) GDALOpen(qsDelftDemPath.toStdString().c_str(), GA_ReadOnly);
}

