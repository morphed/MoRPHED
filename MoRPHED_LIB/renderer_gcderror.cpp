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
#include "renderer_gcderror.h"

Renderer_GCDError::Renderer_GCDError(const char *rasterPath,
                                     int nTransparency):Renderer_Classified(rasterPath, 12, CR_PartialSpectrum, nTransparency, false, false)
{
}

void Renderer_GCDError::createByteRaster()
{
    setClassBreaks();
    classifyRaster();
}

void Renderer_GCDError::setClassBreaks()
{
    if (max < 0.5 || max > 2.0)
    {
        setEqualIntervalBreaks();
    }
    else
    {
        classBreaks.resize(nClasses+1);
        classBreaks[0] = 0.0;
        classBreaks[1] = 0.1;
        classBreaks[2] = 0.2;
        classBreaks[3] = 0.3;
        classBreaks[4] = 0.4;
        classBreaks[5] = 0.5;
        classBreaks[6] = 0.6;
        classBreaks[7] = 0.7;
        classBreaks[8] = 0.8;
        classBreaks[9] = 0.9;
        classBreaks[10] = 1.0;
        classBreaks[11] = 1.1;
        classBreaks[12] = 1.2;
    }
}
