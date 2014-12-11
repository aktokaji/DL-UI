#ifndef CRC32_H
#define CRC32_H

#include <QtCore>

// http://ja.wikipedia.org/wiki/%E5%B7%A1%E5%9B%9E%E5%86%97%E9%95%B7%E6%A4%9C%E6%9F%BB
// http://www.opensource.apple.com/source/xnu/xnu-1456.1.26/bsd/libkern/crc32.c?txt
class CRC32
{
public:
    explicit CRC32();
    quint32 crc32(quint32 crc, const void *buf, size_t size);
    quint32 crc32(quint32 crc, const QByteArray &bytes);
    quint32 crc32(quint32 crc, const QString &str);
private:
    quint32 crc32_tab[256];
};

#endif // CRC32_H
