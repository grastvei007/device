#ifndef DEVICE_GLOBAL_H
#define DEVICE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DEVICE_LIBRARY)
#  define DEVICESHARED_EXPORT Q_DECL_EXPORT
#else
#  define DEVICESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DEVICE_GLOBAL_H
