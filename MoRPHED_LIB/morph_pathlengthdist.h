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
#ifndef MORPH_PATHLENGTHDIST_H
#define MORPH_PATHLENGTHDIST_H

#include "morphed_lib.h"

class MORPHED_LIBSHARED_EXPORT MORPH_PathLengthDist
{
public:
    MORPH_PathLengthDist();
    MORPH_PathLengthDist(double length, double sig_a, double mu_b, int type, double cellSize);

    double getCellSize();
    QVector<double> getDistribution();
    double getLength();
    int getLengthCells();
    double getMuB();
    double getSigA();
    double getValueAtPosition(int nPos);
    void setupDistribution(double length, double sig_a, double mu_b, int type, double cellSize);

    static QVector<double> getDistribution(int nType, double sigA, double muB, int nLength, double cellSize);

private:
    int nLength, nType;
    double dLength, sigA, muB, cellWidth;
    QVector<double> qvDist;

    void setupDistribution();
    void setProperties(double length, double sig_a, double mu_b, int type, double cellSize);
};

#endif // MORPH_PATHLENGTHDIST_H
