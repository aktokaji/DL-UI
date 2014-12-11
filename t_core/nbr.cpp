#include "nbr.h"

class NetworkWaiterSleepCondition : public QObject
{
    Q_OBJECT
public:
    explicit NetworkWaiterSleepCondition()
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

class T_NetworkWaiterPrivate : public QObject
{
    explicit T_NetworkWaiterPrivate(T_NetworkWaiter *me)
        : QObject(me), q_ptr(me)
        , m_meta_data(false)
        , m_buffer_mutex(QMutex::Recursive)
    {  }
    ~T_NetworkWaiterPrivate()
    {  }
    Q_DECLARE_PUBLIC(T_NetworkWaiter)
    class T_NetworkWaiter *q_ptr;
    bool m_meta_data;
    QMutex m_buffer_mutex;
    QByteArray m_buffer_bytes;
    QEventLoop m_eventLoop;
    NetworkWaiterSleepCondition m_wait_cond;
    QByteArray p_pop_front(QByteArray &a_bytes, int a_maxlen)
    {
        int v_len = qMin(a_maxlen, a_bytes.size());
        QByteArray v_pop = a_bytes.left(v_len);
        a_bytes = a_bytes.right(a_bytes.size()-v_len);
        return v_pop;
    }
};

void T_NetworkWaiter::ctor_NetworkWaiter(QNetworkReply *a_reply, const QString &a_name, bool a_buffer_mode)
{
    //[args]
    m_reply = a_reply;
    m_buffer_mode = a_buffer_mode;
    //[private data]
    d_ptr = new T_NetworkWaiterPrivate(this);
    //Q_D(NetworkWaiter);
    //[thread affinity]
    this->moveToThread(m_reply->thread());
    Q_ASSERT(m_reply->thread() == this->thread());
    //[a_name]
    if(a_name.isEmpty())
    {
        m_name.sprintf("%s@0x%08x", this->metaObject()->className(), (uint)this);
    }
    else
    {
        m_name = a_name;
    }
    //qDebug() << "[NetworkWaiter::NetworkWaiter()]" << d->m_name;
    p_connect_slots();
}

T_NetworkWaiter::~T_NetworkWaiter()
{
    //Q_D(NetworkWaiter);
    //qDebug() << "[NetworkWaiter::~NetworkWaiter()]" << d->m_name;
    p_disconnect_slots();
}

void T_NetworkWaiter::setNewReply(QNetworkReply *a_reply)
{
    m_reply = a_reply;
    this->moveToThread(m_reply->thread());
    p_connect_slots();
}

void T_NetworkWaiter::p_connect_slots()
{
    p_disconnect_slots();
    connect(m_reply, SIGNAL(metaDataChanged()), this, SLOT(on_reply_metaDataChanged()));
    connect(m_reply, SIGNAL(readyRead()), this, SLOT(on_reply_readyRead()));
    connect(m_reply, SIGNAL(finished()), this, SLOT(on_reply_finished()));
}

void T_NetworkWaiter::p_disconnect_slots()
{
    if(m_reply)
    {
        //[m_reply might be deleted beforehand]
        disconnect(m_reply, 0, this, 0);
    }
}

void T_NetworkWaiter::p_wait_one_step()
{
    Q_D(T_NetworkWaiter);
    if(m_reply->thread() == QThread::currentThread())
    {
        d->m_eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
    }
    else
    {
        //bool b = d->m_wait_cond.wait(10);
        bool b = d->m_wait_cond.wait(1);
        if(!b)
        {
            //qDebug() << "[!d->m_wait_cond.wait(10)]";
        }
    }
}

void T_NetworkWaiter::on_reply_metaDataChanged()
{
    //qDebug() << "NetworkWaiter::on_reply_metaDataChanged()";
    Q_D(T_NetworkWaiter);
    d->m_meta_data = true;
    if(false)
    {
        qDebug() << ("NetworkWaiter::metaDataChanged()") << m_reply->url();
        foreach(QByteArray v_first, m_reply->rawHeaderList())
        {
            qDebug() << v_first << m_reply->rawHeader(v_first);
        }
    }
    emit metaDataChanged();
}

void T_NetworkWaiter::on_reply_readyRead()
{
    Q_D(T_NetworkWaiter);
    d->m_meta_data = true;
    if(false)
    {
        qDebug()<< "NetworkWaiter::on_reply_readyRead()=====>m_reply->bytesAvailable():"
                << m_reply->bytesAvailable()
                << m_reply->header(QNetworkRequest::ContentLengthHeader)
                << m_reply->rawHeader("Content-Type");
    }
    if(m_buffer_mode)
    {
        T_ATOMIC_LOCKER(v_buffer_lock, &d->m_buffer_mutex)
        QByteArray v_bytes = m_reply->readAll();
        d->m_buffer_bytes.append(v_bytes);
    }
    d->m_eventLoop.exit(0);
    d->m_wait_cond.wakeAll();
    emit readyRead();
}

void T_NetworkWaiter::on_reply_finished()
{
    //qDebug() << ("NetworkBatchRequest::on_reply_finished()");
    Q_D(T_NetworkWaiter);
    d->m_meta_data = true;
    d->m_eventLoop.exit(0);
    d->m_wait_cond.wakeAll();
    emit finished();
}

void T_NetworkWaiter::wait()
{
    //qDebug() << "[NetworkWaiter::wait()]";
    while(!m_reply->isFinished())
    {
        p_wait_one_step();
    }
}

void T_NetworkWaiter::wait_metaDataChanged()
{
    Q_D(T_NetworkWaiter);
    //qDebug() << "[NetworkWaiter::waitMetaData()]";
    while(!d->m_meta_data)
    {
        p_wait_one_step();
    }
}

qint64 T_NetworkWaiter::wait_bytesAvailable(qint64 a_target)
{
    //qDebug() << "[NetworkWaiter::wait_bytesAvailable()]" << a_target;
    this->wait_metaDataChanged();
    qint64 v_break_size = a_target < 0 ? 0 : a_target;
    while(!m_reply->isFinished())
    {
        if(this->just_bytesAvailable()>=v_break_size)
        {
            break;
        }
        p_wait_one_step();
    }
    return this->just_bytesAvailable();
}

QByteArray T_NetworkWaiter::wait_readAll(qint64 a_target)
{
#if 0x0
    Q_D(NetworkWaiter);
    this->wait_bytesAvailable(a_target);
    if(m_buffer_mode)
    {
        T_ATOMIC_LOCKER(v_buffer_lock, &d->m_buffer_mutex)
        QByteArray v_result = d->m_buffer_bytes;
        d->m_buffer_bytes.clear();
        return v_result;
    }
    else
    {
        QByteArray v_result = m_reply->readAll();
        return v_result;
    }
#else
    //Q_D(NetworkWaiter);
    this->wait_bytesAvailable(a_target);
    return this->just_readAll();
#endif
}

qint64 T_NetworkWaiter::just_bytesAvailable()
{
    Q_D(T_NetworkWaiter);
    if(m_buffer_mode)
    {
        T_ATOMIC_LOCKER(v_buffer_lock, &d->m_buffer_mutex)
        return d->m_buffer_bytes.size();
    }
    else
    {
        return m_reply->bytesAvailable();
    }
}

QByteArray T_NetworkWaiter::just_read(qint64 a_max)
{
    Q_D(T_NetworkWaiter);
    if(m_buffer_mode)
    {
        T_ATOMIC_LOCKER(v_buffer_lock, &d->m_buffer_mutex)
        QByteArray v_result = d->p_pop_front(d->m_buffer_bytes, a_max);
        return v_result;
    }
    else
    {
        QByteArray v_result = m_reply->read(a_max);
        return v_result;
    }
}

QByteArray T_NetworkWaiter::just_readAll()
{
    Q_D(T_NetworkWaiter);
    if(m_buffer_mode)
    {
        T_ATOMIC_LOCKER(v_buffer_lock, &d->m_buffer_mutex)
        QByteArray v_result = d->m_buffer_bytes;
        d->m_buffer_bytes.clear();
        return v_result;
    }
    else
    {
        QByteArray v_result = m_reply->readAll();
        return v_result;
    }
}

#include "nbr.moc"
