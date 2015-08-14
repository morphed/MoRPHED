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
