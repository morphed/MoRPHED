#ifndef MORPH_MODEL_H
#define MORPH_MODEL_H

#include <QObject>
#include "morph_sedimenttransport.h"
#include "morph_delft3dio.h"
#include "morph_filemanager.h"

class MORPH_Model : public QObject
{
    Q_OBJECT
public:
    explicit MORPH_Model(XMLReadWrite inputXML, QObject *parent = 0);

    int init(XMLReadWrite inputXML);
    int run();
    int writePngOutputs(int nFlood);

private:
    XMLReadWrite m_inXML, m_outXML;
    MORPH_FileManager m_FileManager;

signals:

public slots:

};

#endif // MORPH_MODEL_H
