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
