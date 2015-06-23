#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QtGui>
#include "dialog_inputs.h"
#include "dialog_morphparams.h"
#include "dialog_delftparams.h"
#include "morph_delft3dio.h"
#include "morph_sedimenttransport.h"

namespace Ui {
class GUI_MainWindow;
}

class GUI_MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GUI_MainWindow(QWidget *parent = 0);
    ~GUI_MainWindow();

    void checkRun();
    void closeEvent(QCloseEvent *event);
    void disableButtons();
    void enableButtons();
    void setName(QString name);

    static void run(XMLReadWrite XmlGui);

private slots:
    void on_btn_description_clicked();

    void on_btn_inputs_clicked();

    void on_btn_morphParam_clicked();

    void on_btn_delftParam_clicked();

    void on_btn_outputs_clicked();

    void on_btn_run_clicked();

    void on_actionNew_Project_triggered();

    void on_actionOpen_Project_triggered();


private:
    Ui::GUI_MainWindow *ui;

    XMLReadWrite XmlGui;
    QString filenameXml, baseDir, baseName;
    bool morphOk, delftOk;
};

#endif // GUI_MAINWINDOW_H
