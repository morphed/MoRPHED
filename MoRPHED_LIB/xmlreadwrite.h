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
    void writeXMLdocViewer();
    void writeEvent(int nCurrentFlood);
};

#endif // XMLREADWRITE_H
