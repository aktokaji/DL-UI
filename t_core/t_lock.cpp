#include "t_lock.h"
#include "windows.h"

class T_CriticalSectionPrivate : public QObject
{
protected:
    T_DECLARE_PUBLIC(T_CriticalSection);
    CRITICAL_SECTION cs;
public:
    explicit T_CriticalSectionPrivate(T_CriticalSection *me)
        : T_INIT_QPTR(me)
    {
        InitializeCriticalSection(&cs);
    }
    ~T_CriticalSectionPrivate()
    {
        DeleteCriticalSection(&cs);
    }
};

T_CriticalSection::T_CriticalSection(QObject *parent)
    : QObject(parent)
{
    T_INIT_DPTR(T_CriticalSection)(this);
}
T_CriticalSection::~T_CriticalSection()
{
    T_D(T_CriticalSection);
    //if(d->m_buffer_top) delete d->m_buffer_top;
}

void T_CriticalSection::lock()
{
    T_D(T_CriticalSection);
    EnterCriticalSection(&d->cs);
}
void T_CriticalSection::unlock()
{
    T_D(T_CriticalSection);
    LeaveCriticalSection(&d->cs);
}
