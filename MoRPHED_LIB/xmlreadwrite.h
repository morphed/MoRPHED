#ifndef XMLREADWRITE_H
#define XMLREADWRITE_H

#include "morphed_lib.h"
#include <QtXml>
#include <iostream>

class MORPHED_LIBSHARED_EXPORT XMLReadWrite
{
private:
    QDomDocument doc;
    QDomElement root;
    QString docFilename;

public:
    XMLReadWrite();

    void clearNodeData(QString tagName);
    void clearNodeData(QString parent, QString tagName);
    void clearNodeData(QString parent, QString subParent, QString tagName);
    void createNewElement(QString tagName);
    void createNewElement(QString parent, QString tagName);
    void createNewElement(QString parent, QString subParent, QString tagName);
    QString getDocumentFilename();
    void loadDocument(QString filename, int type);
    void printXML();
    void printXML(QString filename);
    QString readNodeData(QString tagName);
    QString readNodeData(QString parent, QString tagName);
    QString readNodeData(QString parent, QString subParent, QString tagName);
    void setDocumentFilename(QString filename);
    void writeNodeData(QString tagName, QString nodeData);
    void writeNodeData(QString parent, QString tagName, QString nodeData);
    void writeNodeData(QString parent, QString subParent, QString tagName, QString nodeData);
    void writeRasterProperties(QString rasterPath);
    void writeRasterProperties(const char *rasterPath);
    void writeXMLdocGUI();
    void writeXMLdocViewer(int floodNum);
};

#endif // XMLREADWRITE_H
