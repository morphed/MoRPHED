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
#include "morph_exception.h"

MORPH_Exception::MORPH_Exception(int errorCode, QString evidence)
{
    init (errorCode, evidence);
}

void MORPH_Exception::init(int errorCode, QString evidence)
{
    m_nErrorCode = errorCode;
    m_qsEvidence = evidence;
}

QString MORPH_Exception::getErrorMsgString()
{
    QString result = getCodeAsString(m_nErrorCode) + ": " + m_qsEvidence;
    return result;
}

QString MORPH_Exception::getCodeAsString(int errorCode)
{
    switch(errorCode)
    {
    case PROCESS_OK:
        return "process completed successfully";
        break;

    case QUICKPLOT_ERROR:
        return "Quickplot (from Delft3D) Error";
        break;

    case INPUTXML_ERROR:
        return "Input XML Error";
        break;

    default:
        return "unhandled error";
        break;
    }
}
