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
#ifndef RENDERER_BYTEDATA_H
#define RENDERER_BYTEDATA_H

#include "renderer_stretchminmax.h"

class Renderer_ByteData : public Renderer_StretchMinMax
{
public:
    Renderer_ByteData(const char *inputRasterPath,
                      ColorRamp ramp = CR_BlackWhite,
                      int nTransparency = 255);

protected:
    void createByteRaster();
};

#endif // RENDERER_BYTEDATA_H
