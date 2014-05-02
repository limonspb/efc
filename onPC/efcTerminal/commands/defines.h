#ifndef DEFINES_H
#define DEFINES_H

#ifdef QT_ENABLED
    #include <QtCore>
#else
    #include <Arduino.h>
    #define quint8 uint8_t
    #define qint8 int8_t
    #define quint16 uint16_t
    #define qint16 int16_t
    #define quint32 uint32_t
    #define qint32 int32_t
#endif

#endif // DEFINES_H
