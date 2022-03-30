#ifndef OXI_CORE_GLOBAL_H
#define OXI_CORE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(OXI_CORE_LIBRARY)
#  define OXI_CORE_EXPORT Q_DECL_EXPORT
#else
#  define OXI_CORE_EXPORT Q_DECL_IMPORT
#endif

#endif // OXI_CORE_GLOBAL_H
