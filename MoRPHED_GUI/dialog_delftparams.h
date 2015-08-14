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

    bool getCloseOk();

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
