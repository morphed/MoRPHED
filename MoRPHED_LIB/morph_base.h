#ifndef MORPH_BASE_H
#define MORPH_BASE_H

#include "morphed_lib.h"
#include "xmlreadwrite.h"
#include "morph_raster.h"
#include "morph_filemanager.h"

class MORPHED_LIBSHARED_EXPORT MORPH_Base : QObject
{
public:
    MORPH_Base(QString xmlPath);
    ~MORPH_Base();

    int getCurrentIteration();
    int getIterations();
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
