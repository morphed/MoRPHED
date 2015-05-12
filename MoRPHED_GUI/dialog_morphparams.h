#ifndef DIALOG_MORPHPARAMS_H
#define DIALOG_MORPHPARAMS_H

#include <QDialog>

namespace Ui {
class dialog_morphParams;
}

class dialog_morphParams : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_morphParams(QWidget *parent = 0);
    ~dialog_morphParams();

private slots:
    void on_btn_cancel_clicked();

    void on_btn_ok_clicked();

private:
    Ui::dialog_morphParams *ui;

    bool closeOk;
    double length1, length2, sigA1, sigA2, muB1, muB2, slpThresh, shrThresh, areaThresh, erosionFactor, grainSize;
    int nType1, nType2;

    void checkClose();
    int getType1();
    int getType2();
    void readXml();
    void writeXml();
};

#endif // DIALOG_MORPHPARAMS_H
