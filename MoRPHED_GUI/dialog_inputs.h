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
#ifndef DIALOG_INPUTS_H
#define DIALOG_INPUTS_H

#include <QMessageBox>
#include "xmlreadwrite.h"
#include "morph_base.h"

#include <QDialog>

namespace Ui {
class dialog_inputs;
}

class dialog_inputs : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_inputs(XMLReadWrite &XmlObj, QWidget *parent = 0);
    ~dialog_inputs();

    bool getCloseOk();
    void setupPlot();
    void updatePlot();

private slots:
    void on_btn_cancel_clicked();

    void on_btn_ok_clicked();

    void on_line_dem_textChanged(const QString &arg1);

    void on_line_input_textEdited(const QString &arg1);

    void on_btn_graph_clicked();

    void on_tbtn_dem_clicked();

    void on_tbtn_input_clicked();

private:
    Ui::dialog_inputs *ui;

    XMLReadWrite XmlDoc;
    QString qsDem, qsHydroSedi;
    int nImportType;
    bool closeOk;
    QVector<double> q, sedi, date, dswe;

    void checkClose();
    void readXml();
    void writeXml();
};

#endif // DIALOG_INPUTS_H
