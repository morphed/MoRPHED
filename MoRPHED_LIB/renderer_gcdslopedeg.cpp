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
#include "renderer_gcdslopedeg.h"

Renderer_GCDSlopeDeg::Renderer_GCDSlopeDeg(const char *rasterPath,
                                           int nTransparency):Renderer_Classified(rasterPath, 10, CR_SlopeGCD, nTransparency, false, false)
{
    setZeroCenter(zeroCenter);
    setPrecision();
    adjMax = 90.0, adjMin = 0.0;
}

void Renderer_GCDSlopeDeg::createByteRaster()
{
    setClassBreaks();
    classifyRaster();
}

void Renderer_GCDSlopeDeg::setClassBreaks()
{
    classBreaks.resize(nClasses + 1);
    classBreaks[0] = 0.0;
    classBreaks[1] = 2.0;
    classBreaks[2] = 5.0;
    classBreaks[3] = 10.0;
    classBreaks[4] = 15.0;
    classBreaks[5] = 25.0;
    classBreaks[6] = 35.0;
    classBreaks[7] = 45.0;
    classBreaks[8] = 60.0;
    classBreaks[9] = 80.0;
    classBreaks[10] = 90.0;
}

