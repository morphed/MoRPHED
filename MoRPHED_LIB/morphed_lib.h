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
#ifndef MORPHED_LIB_H
#define MORPHED_LIB_H

#include "morphed_lib_global.h"

//#include "gdal.h"
#include "gdal_priv.h"

const double PI = 3.14159265;
const double POROSITY = 0.4;
const double RHO_S = 2650.0;
const double RHO = 1000.0;
const double A = 9.0;
const double G = 9.81;

const int ROW_OFFSET[9] = {-1,-1,-1,0,0,0,1,1,1};
const int COL_OFFSET[9] = {-1,0,1,-1,0,1,-1,0,1};
const int ROW_OFFSET5[25] = {-2,-2,-2,-2,-2,-1,-1,-1,-1,-1,0,0,0,0,0,1,1,1,1,1,2,2,2,2,2};
const int COL_OFFSET5[25] = {-2,-1,0,1,2,-2,-1,0,1,2,-2,-1,0,1,2,-2,-1,0,1,2,-2,-1,0,1,2};


class MORPHED_LIBSHARED_EXPORT MoRPHED_LIB
{

public:
    MoRPHED_LIB();
};

#endif // MORPHED_LIB_H
