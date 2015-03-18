#ifndef MORPHED_LIB_GLOBAL_H
#define MORPHED_LIB_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QtCore>

#if defined(MORPHED_LIB_LIBRARY)
#  define MORPHED_LIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MORPHED_LIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MORPHED_LIB_GLOBAL_H
