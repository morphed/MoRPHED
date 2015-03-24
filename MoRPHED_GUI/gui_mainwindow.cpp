#include "gui_mainwindow.h"
#include "ui_gui_mainwindow.h"


GUI_MainWindow::GUI_MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUI_MainWindow)
{
    ui->setupUi(this);

    GDALAllRegister();
}

GUI_MainWindow::~GUI_MainWindow()
{
    delete ui;
}

void GUI_MainWindow::on_btn_description_clicked()
{

}

void GUI_MainWindow::on_btn_inputs_clicked()
{
    dialog_inputs dialog(this);
    dialog.setModal(true);
    dialog.exec();
}

void GUI_MainWindow::on_btn_morphParam_clicked()
{
    dialog_morphParams dialog(this);
    dialog.setModal(true);
    dialog.exec();
}

void GUI_MainWindow::on_btn_delftParam_clicked()
{
    dialog_delftParams dialog(this);
    dialog.setModal(true);
    dialog.exec();
}

void GUI_MainWindow::on_btn_outputs_clicked()
{

}

void GUI_MainWindow::on_btn_run_clicked()
{
    QFileInfo file(filenameXml);

    MORPH_FileManager fm(file.absolutePath());
    MORPH_Delft3DIO *delft = new MORPH_Delft3DIO(filenameXml);
    MORPH_SedimentTransport *trans = new MORPH_SedimentTransport(filenameXml);

    for (int i=0; i<trans->getIterations(); i++)
    {

        if (i == 0)
        {
            fm.createFloodDirectories(i);
            qDebug()<<"running delft";
            delft->run();
            qDebug()<<"delft done, load rasters";
            trans->loadRasters();
            qDebug()<<"rasters loaded";
        }

        fm.createFloodDirectories(i+1);
        delft->setCurrentIteration(i);
        trans->setCurrentIteration(i);

        qDebug()<<"running bank erosoin";
        trans->runBankErode();

        qDebug()<<"running delft "<<i;
        delft->run();
        qDebug()<<"done running delft "<<i;

        //trans->setImportCells(delft->getDisRowVector(), delft->getDisColVector());
        qDebug()<<"import cells set";
        trans->loadRasters();
        qDebug()<<"running bed erosion";
        trans->runBedErode();
        qDebug()<<"importing";
        trans->importSediment();
        qDebug()<<"import done";
        qDebug()<<"priting outputs";
        //trans->copyOutputs();
        qDebug()<<"flood done "<<i;
    }

    qDebug()<<"FINISHED!";
}

void GUI_MainWindow::on_actionNew_Project_triggered()
{
    QString filename, name;
    QStringList list;
    filename = QFileDialog::getExistingDirectory(this, "Select or Create Project Folder");

    if (filename.isNull() || filename.isEmpty())
    {
        QMessageBox::information(this, "Empty or Null Path", "The directory path is empty or null, you must select a valid directory to continue");
    }

    else
    {
        XmlGui.writeXMLdocGUI();
        XmlGui.writeNodeData("ProjectDirectory",filename);
        list = filename.split("/");
        name = list[list.size()-1];
        filenameXml = filename + "/" + name + ".morph";
        XmlGui.setDocumentFilename(filenameXml);
    }
}

void GUI_MainWindow::on_actionOpen_Project_triggered()
{
    QString filename;
    filename = QFileDialog::getOpenFileName(this, "Select *.morph file to open");
    filenameXml = filename;

    if (filename.isNull() || filename.isEmpty())
    {
        QMessageBox::information(this, "Empty or Null Path", "The file path is null or empty, you must select a valid *.morph file to continue");
    }
    else
    {
        XmlGui.loadDocument(filename, 1);
    }
}
