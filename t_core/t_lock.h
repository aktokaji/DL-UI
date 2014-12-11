#ifndef T_LOCK_H
#define T_LOCK_H

#include <QtCore>
#include "t_global.h"

class T_Atomic : public QMutex
{
public:
    explicit T_Atomic() : QMutex(QMutex::Recursive)
    {  }
};

class T_AtomicObject : public QObject, public T_Atomic
{
    Q_OBJECT
public:
    explicit T_AtomicObject() : T_Atomic()
    {  }
};

class T_Locked : public QReadWriteLock
{
public:
    explicit T_Locked() : QReadWriteLock(QReadWriteLock::Recursive)
    {  }
};

class T_LockedObject : public QObject, public T_Locked
{
    Q_OBJECT
public:
    explicit T_LockedObject() : T_Locked()
    {  }
};

class T_SlimpleWaitCondition
{
public:
    explicit T_SlimpleWaitCondition()
        : m_wait_mutex(QMutex::NonRecursive)
    {  }
    bool wait(unsigned long a_msec = ULONG_MAX)
    {
        m_wait_mutex.lock();
        bool b = m_wait_cond.wait(&m_wait_mutex, a_msec);
        m_wait_mutex.unlock();
        return b;
    }
    void wakeOne()
    {
        m_wait_cond.wakeOne();
    }
    void wakeAll()
    {
        m_wait_cond.wakeAll();
    }
protected:
    QMutex m_wait_mutex;
    QWaitCondition m_wait_cond;
};

#define T_ATOMIC_LOCKER(V,X) QMutexLocker V(X);Q_UNUSED(V)
#define T_READ_LOCKER(V,X)   QReadLocker V(X);Q_UNUSED(V)
#define T_WRITE_LOCKER(V,X)  QWriteLocker V(X);Q_UNUSED(V)

class T_AtomicCounter32 : public T_Atomic
{
protected:
    qint32 m_count;
public:
    explicit T_AtomicCounter32(qint32 a_init_value = 0) : m_count(a_init_value) {  }
    operator qint32() { T_ATOMIC_LOCKER(v_locker, this); return (m_count); }
    qint32 operator++() { T_ATOMIC_LOCKER(v_locker, this); return (++m_count); }
    qint32 operator--() { T_ATOMIC_LOCKER(v_locker, this); return (--m_count); }
    qint32 operator++(int) { T_ATOMIC_LOCKER(v_locker, this); return (m_count++); }
    qint32 operator--(int)  { T_ATOMIC_LOCKER(v_locker, this); return (m_count--); }
    qint32 operator+=(qint32 a) { T_ATOMIC_LOCKER(v_locker, this); return (m_count+=a); }
    qint32 operator-=(qint32 a) { T_ATOMIC_LOCKER(v_locker, this); return (m_count-=a); }
};

class T_AtomicCounter64 : public T_Atomic
{
protected:
    qint64 m_count;
public:
    explicit T_AtomicCounter64(qint64 a_init_value = 0) : m_count(a_init_value) {  }
    operator qint64() { T_ATOMIC_LOCKER(v_locker, this); return (m_count); }
    qint64 operator++() { T_ATOMIC_LOCKER(v_locker, this); return (++m_count); }
    qint64 operator--() { T_ATOMIC_LOCKER(v_locker, this); return (--m_count); }
    qint64 operator++(int) { T_ATOMIC_LOCKER(v_locker, this); return (m_count++); }
    qint64 operator--(int)  { T_ATOMIC_LOCKER(v_locker, this); return (m_count--); }
    qint64 operator+=(qint64 a) { T_ATOMIC_LOCKER(v_locker, this); return (m_count+=a); }
    qint64 operator-=(qint64 a) { T_ATOMIC_LOCKER(v_locker, this); return (m_count-=a); }
};

class T_CriticalSection : public QObject
{
       Q_OBJECT
protected:
    T_DECLARE_PRIVATE(T_CriticalSection);
public:
    T_CriticalSection(QObject *parent = 0);
    ~T_CriticalSection();
    void lock();
    void unlock();
};

#endif // T_LOCK_H
