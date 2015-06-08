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
