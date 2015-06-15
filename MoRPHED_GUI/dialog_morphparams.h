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
    double length1, length2, sigA1, sigA2, muB1, muB2, slpThresh, shrThresh, areaThresh, erosionFactor, grainSize;
    int nType1, nType2, nCellWidth;
    QVector<double> x1, x2, y1, y2;

    void checkClose();
    int getType1();
    int getType2();
    void readXml();
    void writeXml();
};

#endif // DIALOG_MORPHPARAMS_H
