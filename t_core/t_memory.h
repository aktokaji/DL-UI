#ifndef T_MEMORY_H
#define T_MEMORY_H

#include <QtCore>
#include "t_global.h"

class T_MemoryAligned : public QObject
{
    Q_OBJECT
protected:
    T_DECLARE_PRIVATE(T_MemoryAligned);
public:
    explicit T_MemoryAligned(QObject *parent = 0);
    virtual ~T_MemoryAligned();
    uchar *resize(size_t a_size, size_t a_alignment);
    size_t size();
    size_t alignment();
    uchar *top();
};

#endif // T_MEMORY_H
