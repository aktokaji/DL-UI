#include "t_memory.h"

class T_MemoryAlignedPrivate : public QObject
{
protected:
    T_DECLARE_PUBLIC(T_MemoryAligned);
    size_t m_size;
    size_t m_alignment;
    uchar *m_buffer_top;
    uchar *m_align_addr;
public:
    explicit T_MemoryAlignedPrivate(T_MemoryAligned *me)
        : T_INIT_QPTR(me)
        , m_size(0)
        , m_alignment(0)
        , m_buffer_top(0)
        , m_align_addr(0)
    {
    }
    ~T_MemoryAlignedPrivate()
    {
    }
};
T_MemoryAligned::T_MemoryAligned(QObject *parent)
    : QObject(parent)
{
    T_INIT_DPTR(T_MemoryAligned)(this);
}
#if 0x0
T_MemoryAligned::~T_MemoryAligned()
{
    Q_D(T_MemoryAligned);
    if(d->m_buffer_top) delete d->m_buffer_top;
}
uchar *T_MemoryAligned::resize(size_t a_size, size_t a_alignment)
{
    Q_D(T_MemoryAligned);
    Q_ASSERT(a_size>0);Q_ASSERT(a_alignment>0);
    d->m_size = a_size;
    d->m_alignment = a_alignment;
    d->m_buffer_top = (uchar *)realloc(d->m_buffer_top, a_size+a_alignment);
    d->m_align_addr = d->m_buffer_top+(((size_t)d->m_buffer_top)%a_alignment);
    return d->m_align_addr;
}
#else
T_MemoryAligned::~T_MemoryAligned()
{
    Q_D(T_MemoryAligned);
    if(d->m_buffer_top) qFreeAligned(d->m_buffer_top);
}
uchar *T_MemoryAligned::resize(size_t a_size, size_t a_alignment)
{
    Q_D(T_MemoryAligned);
    Q_ASSERT(a_size>0);Q_ASSERT(a_alignment>0);
    d->m_alignment = a_alignment;
    d->m_buffer_top = (uchar *)qReallocAligned(d->m_buffer_top, a_size, d->m_size, d->m_alignment);
    Q_ASSERT((((size_t)d->m_buffer_top)%a_alignment)==0);
    d->m_size = a_size;
    d->m_align_addr = d->m_buffer_top;
    return d->m_align_addr;
}
#endif
size_t T_MemoryAligned::size()
{
    Q_D(T_MemoryAligned);
    return d->m_size;
}
size_t T_MemoryAligned::alignment()
{
    Q_D(T_MemoryAligned);
    return d->m_alignment;
}
uchar *T_MemoryAligned::top()
{
    Q_D(T_MemoryAligned);
    return (uchar *)d->m_align_addr;
}
