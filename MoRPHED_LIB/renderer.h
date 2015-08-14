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
#ifndef RENDERER_H
#define RENDERER_H

#include "gdal_priv.h"
#include <QtGui>

enum ColorRamp{CR_BlackWhite, CR_DEM, CR_DoD, CR_GrainSize, CR_GreenBlue, CR_LtBlueDkBlue, CR_PartialSpectrum, CR_Precipitation, CR_Slope, CR_SlopeGCD, CR_WhiteRed};

class Renderer
{
public:
    Renderer(const char *inputRasterPath,
             ColorRamp ramp = CR_BlackWhite,
             int nTransparency = 255,
             bool zeroNoData = FALSE);
    virtual ~Renderer();

    void printLegend();
    void printLegend(const char *path);
    int rasterToPNG(const char *pngPath,
                    int nQuality,
                    int nLength);
    int setRendererColorTable(ColorRamp rampStyle,
                      int nTransparency);
    void setPrecision(int prec);
    void setZeroNoData(bool bValue);

    static void stackImages(const char *inputList, const char *outputImage, int nQuality);

protected:
    const char *rasterPath;
    const char *pngOutPath;
    float *oldRow;
    unsigned char *newRow;
    QString tempRasterPath, legendPath;
    GDALDataset *pRaster, *pTempRaster, *pPngDS;
    GDALDriver *pDriverPNG, *pDriverTiff;
    GDALColorTable *colorTable;
    GDALDataType rasterType;
    int nRows, nCols, precision;
    double min, max, mean, stdev, noData, noData2;
    double adjMin, adjMax, adjMean, range;
    double transform[6];
    bool zeroNoData, zeroCenter;

    void cleanUp();
    virtual void createByteRaster() = 0;
    virtual void createLegend() = 0;
    int resizeAndCompressPNG(const char *inputImage,
                             int nLength,
                             int nQuality);
    void setLegendPath();
    void setLegendPath(const char *path);
    void setPrecision();
    int setTempRasterPath(const char *rasterPath);
    void setup();
    int setupRaster(const char *inputRasterPath);
};

#endif // RENDERER_H
