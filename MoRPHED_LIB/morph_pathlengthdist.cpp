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
#include "morph_pathlengthdist.h"

MORPH_PathLengthDist::MORPH_PathLengthDist()
{

}

MORPH_PathLengthDist::MORPH_PathLengthDist(double length, double sig_a, double mu_b, int type, double cellSize)
{
    setProperties(length, sig_a, mu_b, type, cellSize);

    setupDistribution();
}

double MORPH_PathLengthDist::getCellSize()
{
    return cellWidth;
}

QVector<double> MORPH_PathLengthDist::getDistribution()
{
    return qvDist;
}

double MORPH_PathLengthDist::getLength()
{
    return dLength;
}

int MORPH_PathLengthDist::getLengthCells()
{
    return nLength;
}

double MORPH_PathLengthDist::getMuB()
{
    return muB;
}

double MORPH_PathLengthDist::getSigA()
{
    return sigA;
}

double MORPH_PathLengthDist::getValueAtPosition(int nPos)
{
    return qvDist[nPos];
}

void MORPH_PathLengthDist::setupDistribution(double length, double sig_a, double mu_b, int type, double cellSize)
{
    setProperties(length, sig_a, mu_b, type, cellSize);

    setupDistribution();
}

QVector<double> MORPH_PathLengthDist::getDistribution(int nType, double sigA, double muB, int nLength, double cellSize)
{
    QVector<double> qvDist;

    bool good = true;
    double factor;
    double sum = 0.0;

    sigA /= cellSize;
    muB /= cellSize;

    //exponential
    if (nType == 1)
    {
        for (int i=0; i<nLength; i++)
        {
            qvDist.append((1.0/sqrt(2.0*PI))*exp(((-1.0)*sigA)*muB*(i+1.0)));
            sum += qvDist[i];
        }
    }
    //gaussian
    else if (nType == 2)
    {
        for (int i=0; i<nLength; i++)
        {
            qvDist.append((1.0/(sigA*sqrt(2.0*PI))) * exp((-0.5)*pow((((i+1.0)-muB)/sigA),2.0)));
            sum += qvDist[i];
        }
    }
    //custom
    else if (nType == 3)
    {

    }
    else
    {
        good = false;
    }

    if (good)
    {
        factor = 1.0/sum;
        for (int i=0; i<nLength; i++)
        {
            qvDist[i] *= factor;
        }
    }

    return qvDist;
}

void MORPH_PathLengthDist::setupDistribution()
{
    qvDist.clear();

    bool good = true;
    double factor;
    double sum = 0.0;

    sigA /= cellWidth;
    muB /= cellWidth;

    //exponential
    if (nType == 1)
    {
        for (int i=0; i<nLength; i++)
        {
            qvDist.append((1.0/sqrt(2.0*PI))*exp(((-1.0)*sigA)*muB*(i+1.0)));
            sum += qvDist[i];
        }
    }
    //gaussian
    else if (nType == 2)
    {
        for (int i=0; i<nLength; i++)
        {
            qvDist.append((1.0/(sigA*sqrt(2.0*PI))) * exp((-0.5)*pow((((i+1.0)-muB)/sigA),2.0)));
            sum += qvDist[i];
        }
    }
    //custom
    else if (nType == 3)
    {

    }
    else
    {
        good = false;
    }

    if (good)
    {
        factor = 1.0/sum;
        for (int i=0; i<nLength; i++)
        {
            qvDist[i] *= factor;
        }
    }
}

void MORPH_PathLengthDist::setProperties(double length, double sig_a, double mu_b, int type, double cellSize)
{
    dLength = length;
    cellWidth = cellSize;
    sigA = sig_a;
    muB = mu_b;
    nType = type;
    nLength = rint(dLength / cellWidth);
}
