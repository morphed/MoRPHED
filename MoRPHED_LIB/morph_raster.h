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
#ifndef MORPH_RASTER_H
#define MORPH_RASTER_H

#include "morphed_lib.h"

class MORPHED_LIBSHARED_EXPORT MORPH_Raster
{
public:
    MORPH_Raster();
    MORPH_Raster(const char *rasterPath);
    ~MORPH_Raster();

    void add(const char *addPath);
    void add(const char *sourcePath, const char *addPath);
    void aspect(const char *aspectPath);
    void aspect(const char *sourcePath, const char *aspectPath);
    double averageSlope_ColAve(int col1, int col2);
    double averageSlope_ColAve(const char *rasterPath, int col1, int col2);
    double averageSlope_RowAve(int row1, int row2);
    double averageSlope_RowAve(const char *rasterPath, int row1, int row2);
    void copyBoundary(int boundLoc);
    void copyBoundary(const char *rasterPath, int boundLoc);
    void demOfDifference(const char *oldDem, const char *newDem, const char *dodRaster);
    void filterLowPass(const char *filterRaster);
    void filterLowPass(const char *sourceRaster, const char *filterRaster);
    double findMax(const char *rasterPath);
    void flowDirection(const char *xVelPath, const char *yVelPath, const char *rasterPath);
    void fromXYZ(const char *rasterPath, const char *xyzPath, int cols, int rows, double noDataValue, double inTransform[], int headerRows = 0);
    void hillshade(const char *hlsdPath);
    void hillshade(const char *rasterPath, const char *hlsdPath);
    int regions(const char *regionsRaster);
    int regions(const char *inputRaster, const char *regionsRaster);
    void setProperties(const char *rasterPath);
    void slopeTOF(const char *slopePath);
    void slopeTOF(const char *sourcePath, const char *slopePath);
    void slopeDeg(const char *slopePath);
    void slopeDeg(const char *sourcePath, const char *slopePath);
    void subtract(const char *subtractPath);
    void subtract(const char *sourcePath, const char *subtractPath);
    void subtract(const char *sourcePath, const char *subtractPath, const char *outputPath);
    double sum();
    double sum(const char *rasterPath);
    void zeroToNoData(const char *sourcePath, double noDataValue);
protected:

private:
    int nRows,
        nCols;

    double  transform[6],
            noData;

    const char *m_rasterPath;

    GDALDriver *pDriverTiff;

    double averageCol(int col);
    double averageRow(int row);
    void loadDrivers();

};

#endif // MORPH_RASTER_H
