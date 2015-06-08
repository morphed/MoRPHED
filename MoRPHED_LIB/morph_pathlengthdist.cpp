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

QVector<double> MORPH_PathLengthDist::getDistribution(int nType, double sigA, double muB, int nLength)
{
    QVector<double> qvDist;

    bool good = true;
    double factor;
    double sum = 0.0;

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
