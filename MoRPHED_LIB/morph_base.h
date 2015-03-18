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
            nPlDistType,
            nDirDSbound,
            nDirUSbound;

    double  transform[6],
            noData,
            sigA,
            muB,
            simTime,
            timeStep,
            roughness,
            hev,
            cellWidth,
            cellHeight,
            topLeftX,
            topLeftY,
            layerThickness,
            plDistLength,
            gsActiveSize,
            bankSlopeThresh,
            bankShearThresh;

    QString qsRootPath,
            qsInputPath,
            qsOutputPath,
            qsTempPath,
            qsOrigDemPath,
            qsHydroSediPath,
            qsGsActPath,
            qsGsSubPath,
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

private:
    void init();
    void loadDrivers();
    void loadInputText();

signals:
    void sendMessage(QString message);

public slots:

};

#endif // MORPH_BASE_H
