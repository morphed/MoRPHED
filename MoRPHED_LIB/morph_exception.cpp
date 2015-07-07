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
