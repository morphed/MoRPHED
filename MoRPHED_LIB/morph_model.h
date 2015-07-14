#ifndef MORPH_MODEL_H
#define MORPH_MODEL_H

#include <QObject>
#include "morph_sedimenttransport.h"
#include "morph_delft3dio.h"
#include "morph_filemanager.h"
#include "renderer_bytedata.h"
#include "renderer_classified.h"
#include "renderer_stretchminmax.h"

class MORPHED_LIBSHARED_EXPORT MORPH_Model : public QObject
{
    Q_OBJECT
public:
    explicit MORPH_Model(XMLReadWrite inputXML, QObject *parent = 0);

    int init(XMLReadWrite inputXML);
    int run();
    int writeDisplayData(int nFlood, QVector<double> volumes);
    int writeInitialData(int nFlood);
    int writePngOutputs(QString eventName, int nFlood);
    int writeVolumes(QString eventName, QVector<double> volumes);

private:
    XMLReadWrite m_inXML, m_outXML;
    MORPH_FileManager m_FileManager;

signals:

public slots:

};

#endif // MORPH_MODEL_H
