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
#include "renderer_gcdslopeper.h"

Renderer_GCDSlopePer::Renderer_GCDSlopePer(const char *rasterPath,
                                           int nTransparency):Renderer_Classified(rasterPath, 13, CR_SlopeGCD, nTransparency, false, false)
{
}

void Renderer_GCDSlopePer::createByteRaster()
{
    setClassBreaks();
    classifyRaster();
}

void Renderer_GCDSlopePer::setClassBreaks()
{
    classBreaks.resize(14);
    classBreaks[0] = 0.0;
    classBreaks[1] = 1.0;
    classBreaks[2] = 2.0;
    classBreaks[3] = 5.0;
    classBreaks[4] = 10.0;
    classBreaks[5] = 15.0;
    classBreaks[6] = 20.0;
    classBreaks[7] = 25.0;
    classBreaks[8] = 30.0;
    classBreaks[9] = 35.0;
    classBreaks[10] = 40.0;
    classBreaks[11] = 50.0;
    classBreaks[12] = 100.0;
    classBreaks[13] = 300.0;
}
