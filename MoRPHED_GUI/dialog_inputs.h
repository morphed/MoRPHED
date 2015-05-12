#ifndef DIALOG_INPUTS_H
#define DIALOG_INPUTS_H

#include "xmlreadwrite.h"

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

private slots:
    void on_btn_cancel_clicked();

    void on_btn_ok_clicked();

private:
    Ui::dialog_inputs *ui;

    XMLReadWrite XmlDoc;
    QString qsDem, qsHydroSedi;
    int nImportType;

    void readXml();
};

#endif // DIALOG_INPUTS_H
