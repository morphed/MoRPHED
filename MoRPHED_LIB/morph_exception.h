#ifndef MORPH_EXCEPTION_H
#define MORPH_EXCEPTION_H

#include <iostream>
#include <QString>
#include "morphed_lib.h"

enum MorphedExceptionCodes
{
    PROCESS_OK = 0
    , QUICKPLOT_ERROR
    , INPUTXML_ERROR
};

class MORPHED_LIBSHARED_EXPORT MORPH_Exception : public std::exception
{
public:
    MORPH_Exception(int errorCode, QString evidence = "");
    virtual ~MORPH_Exception() throw(){};

    void init(int errorCode, QString evidence);
    QString getErrorMsgString();

    static QString getCodeAsString(int errorCode);

private:
    int m_nErrorCode;
    QString m_qsEvidence;
};

#endif // MORPH_EXCEPTION_H
