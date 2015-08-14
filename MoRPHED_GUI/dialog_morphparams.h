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
#ifndef DIALOG_MORPHPARAMS_H
#define DIALOG_MORPHPARAMS_H

#include <QDialog>
#include "xmlreadwrite.h"
#include "morph_pathlengthdist.h"
#include "morph_base.h"

namespace Ui {
class dialog_morphParams;
}

class dialog_morphParams : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_morphParams(XMLReadWrite &XmlObj, QWidget *parent = 0);
    ~dialog_morphParams();

    bool getCloseOk();
    void setupDisplay();
    void setupPlots();
    void setPathLengthValues();
    void setErosionValues();
    void updatePlots();

private slots:
    void on_btn_cancel_clicked();

    void on_btn_ok_clicked();

    void on_btn_update_clicked();

    void on_rbtn_exp2_toggled(bool checked);

    void on_rbtn_exp1_toggled(bool checked);

    void on_rbtn_gaus1_toggled(bool checked);

    void on_rbtn_gaus2_toggled(bool checked);

private:
    Ui::dialog_morphParams *ui;

    XMLReadWrite XmlDoc;
    bool closeOk, plotOk;
    double length1, length2, sigA1, sigA2, muB1, muB2, slpThresh, shrThresh, areaThresh, erosionFactor, grainSize, cellWidth;
    int nType1, nType2;
    QVector<double> x1, x2, y1, y2;

    void checkClose();
    int getType1();
    int getType2();
    void readXml();
    void writeXml();
};

#endif // DIALOG_MORPHPARAMS_H
