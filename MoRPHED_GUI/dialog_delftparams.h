#ifndef DIALOG_DELFTPARAMS_H
#define DIALOG_DELFTPARAMS_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include "xmlreadwrite.h"

namespace Ui {
class dialog_delftParams;
}

class dialog_delftParams : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_delftParams(XMLReadWrite &XmlObj, QWidget *parent = 0);
    ~dialog_delftParams();

private slots:
    void on_btn_cancel_clicked();

    void on_btn_ok_clicked();

    void on_tbtn_dem_clicked();

private:
    Ui::dialog_delftParams *ui;

    XMLReadWrite XmlDoc;
    double simTime, timeStep, roughness, hev;
    int nUsBound, nDsBound;
    bool closeOk;
    QString qsDelftPath;

    void checkClose();
    int getDsBound();
    int getUsBound();
    void readXml();
    void setDsBound(int value);
    void setInitialValues();
    void setUsBound(int value);
    void writeXml();
};

#endif // DIALOG_DELFTPARAMS_H
