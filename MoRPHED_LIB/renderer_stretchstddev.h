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
#ifndef STRETCHRENDERERSTDEV_H
#define STRETCHRENDERERSTDEV_H

#include "renderer.h"

class Renderer_StretchStdDev : public Renderer
{
public:
    Renderer_StretchStdDev(const char *inputRasterPath,
                         double stDevStretch = 2.5,
                         ColorRamp ramp = CR_BlackWhite,
                         int nTransparency = 255,
                         bool zeroCenter = FALSE,
                         bool zeroNoData = FALSE);

protected:
    double sdStretch, sdMin, sdMax, corVal, maxCalc, minCalc;

    void createByteRaster();
    void createLegend();
    void setZeroCenter(bool bValue);
};

#endif // STRETCHRENDERERSTDEV_H
