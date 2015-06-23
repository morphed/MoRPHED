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
