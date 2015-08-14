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
#include "renderer_bytedata.h"

Renderer_ByteData::Renderer_ByteData(const char *inputRasterPath,
                                     ColorRamp ramp,
                                     int nTransparency):Renderer_StretchMinMax(inputRasterPath, ramp, nTransparency, false, false)
{
    setPrecision(0);
}

void Renderer_ByteData::createByteRaster()
{
    unsigned char *byteRow = (unsigned char*) CPLMalloc(sizeof(int)*nCols);
    for (int i=0; i<nRows; i++)
    {
        pRaster->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, byteRow, nCols, 1, GDT_Byte, 0, 0);

        for (int j=0; j<nCols; j++)
        {
            if (byteRow[j] <= 0 || byteRow[j] >= 255)
            {
                newRow[j] = 0;
            }
            else
            {
                newRow[j] = byteRow[j];
            }
        }
        pTempRaster->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, newRow, nCols, 1, GDT_Byte, 0, 0);
    }
    CPLFree(byteRow);
}
