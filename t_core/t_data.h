#ifndef T_DATA_H
#define T_DATA_H

#include "t_global.h"
#include "t_lock.h"

template <typename T, T DEFAULT_VALUE>
class T_Queue : public T_AtomicObject
{
public:
    explicit T_Queue() : m_count(0) {  }
    virtual ~T_Queue() {  }
    bool &f_complete()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        return m_complete;
    }
    virtual bool isEmpty()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        return m_queue.isEmpty();
    }
    virtual int size()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        return m_queue.size();
    }
    virtual int count()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        return m_count;
    }
    virtual int endlessCount()
    {
        if(m_complete) return INT_MAX;
        return m_count;
    }
    virtual void clear()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        m_queue.clear();
        m_count = 0;
    }
    virtual void enqueue(T a)
    {
        T_ATOMIC_LOCKER(v_lock, this);
        this->m_queue.enqueue(a);
        m_count++;
    }
    virtual T dequeue()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        if(this->m_queue.size()==0) return DEFAULT_VALUE;
        return m_queue.dequeue();
    }
    virtual T head()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        if(m_queue.size()==0) return DEFAULT_VALUE;
        return m_queue.head();
    }
    virtual T last()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        if(m_queue.size()==0) return DEFAULT_VALUE;
        return m_queue.last();
    }
    virtual T value(int i)
    {
        T_ATOMIC_LOCKER(v_lock, this);
        if(i<0) return DEFAULT_VALUE;
        if(i>=m_queue.size()) return DEFAULT_VALUE;
        return m_queue.value(i);
    }
protected:
    QQueue< T > m_queue;
    int m_count;
    bool m_complete;
};

template <typename T>
class T_NormalPointerQueue : public T_AtomicObject
{
public:
    explicit T_NormalPointerQueue() : m_count(0) {  }
    virtual ~T_NormalPointerQueue() {  }
    bool &f_complete()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        return m_complete;
    }
    virtual bool isEmpty()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        return m_queue.isEmpty();
    }
    virtual int size()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        return m_queue.size();
    }
    virtual int count()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        return m_count;
    }
    virtual int endlessCount()
    {
        if(m_complete) return INT_MAX;
        return m_count;
    }
    virtual void clear()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        m_queue.clear();
        m_count = 0;
    }
    virtual void enqueue(T * a)
    {
        T_ATOMIC_LOCKER(v_lock, this);
        this->m_queue.enqueue(a);
        m_count++;
    }
    virtual T * dequeue()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        if(this->m_queue.size()==0) return NULL;
        return m_queue.dequeue();
    }
    virtual T * head()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        if(m_queue.size()==0) return NULL;
        return m_queue.head();
    }
    virtual T * last()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        if(m_queue.size()==0) return NULL;
        return m_queue.last();
    }
    virtual T * value(int i)
    {
        T_ATOMIC_LOCKER(v_lock, this);
        if(i<0) return NULL;
        if(i>=m_queue.size()) return NULL;
        return m_queue.value(i);
    }
protected:
    QQueue< T * > m_queue;
    int m_count;
    bool m_complete;
};

template <typename T>
class T_SharedPointerQueue : public T_AtomicObject
{
public:
    explicit T_SharedPointerQueue() : m_count(0) {  }
    virtual ~T_SharedPointerQueue() {  }
    bool &f_complete()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        return m_complete;
    }
    virtual bool isEmpty()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        return m_queue.isEmpty();
    }
    virtual int size()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        return m_queue.size();
    }
    virtual int count()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        return m_count;
    }
    virtual int endlessCount()
    {
        if(m_complete) return INT_MAX;
        return m_count;
    }
    virtual void clear()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        m_queue.clear();
        m_count = 0;
    }
    virtual void enqueue(QSharedPointer<T> a)
    {
        T_ATOMIC_LOCKER(v_lock, this);
        this->m_queue.enqueue(a);
        m_count++;
    }
    virtual void enqueue(T * a)
    {
        T_ATOMIC_LOCKER(v_lock, this);
        this->m_queue.enqueue(QSharedPointer<T>(a));
        m_count++;
    }
    virtual QSharedPointer<T> dequeue()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        if(this->m_queue.size()==0) return QSharedPointer<T>(NULL);
        return m_queue.dequeue();
    }
    virtual QSharedPointer<T> head()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        if(m_queue.size()==0) return QSharedPointer<T>(NULL);
        return m_queue.head();
    }
    virtual QSharedPointer<T> last()
    {
        T_ATOMIC_LOCKER(v_lock, this);
        if(m_queue.size()==0) return QSharedPointer<T>(NULL);
        return m_queue.last();
    }
    virtual QSharedPointer<T> value(int i)
    {
        T_ATOMIC_LOCKER(v_lock, this);
        if(i<0) return QSharedPointer<T>(NULL);
        if(i>=m_queue.size()) return QSharedPointer<T>(NULL);
        return m_queue.value(i);
    }
protected:
    QQueue< QSharedPointer<T> > m_queue;
    int m_count;
    bool m_complete;
};

#endif // T_DATA_H
