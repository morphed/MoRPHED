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
            disMax;

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
