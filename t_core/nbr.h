#ifndef NETWORKBATCHREQUEST_H
#define NETWORKBATCHREQUEST_H

#include <QtCore>
#include <QtNetwork>
#include "t_lock.h"

class T_NetworkWaiter : public QObject //QThread //FIXME
{
    Q_OBJECT
public:
    T_NetworkWaiter(QNetworkReply *a_reply, const QString &a_name = "")
    {
        ctor_NetworkWaiter(a_reply, a_name, false);
    }
    ~T_NetworkWaiter();
    QNetworkReply *reply() { return m_reply; }
    bool isFinished() { return (m_reply->isFinished()); }
    void wait();
    void wait_metaDataChanged();
    qint64 wait_bytesAvailable(qint64 a_target);
    QByteArray wait_readAll(qint64 a_target);
    qint64 just_bytesAvailable();
    QByteArray just_read(qint64 a_max);
    QByteArray just_readAll();
    void setName(const QString &a_name) { m_name = a_name; }
    QString name() { return m_name; }
    qint64 contentLength()
    {
        QVariant v_cont_len = m_reply->header(QNetworkRequest::ContentLengthHeader);
        if(!v_cont_len.isValid()) return -1;
        return v_cont_len.toLongLong();
    }
    void setNewReply(QNetworkReply *a_reply);

protected:
    void p_connect_slots();
    void p_disconnect_slots();
    void p_wait_one_step();

protected slots:
    void on_reply_metaDataChanged();
    void on_reply_readyRead();
    void on_reply_finished();

signals:
    void metaDataChanged();
    void readyRead();
    void finished();

protected:
    T_NetworkWaiter() {  }
    void ctor_NetworkWaiter(QNetworkReply *a_reply, const QString &a_name, bool a_buffer_mode);
protected:
    class T_NetworkWaiterPrivate *d_ptr;
    Q_DECLARE_PRIVATE(T_NetworkWaiter)
protected:
#if 0x0
    QNetworkReply *m_reply;
#else
    QPointer< QNetworkReply > m_reply;
#endif
    QString m_name;
    bool m_buffer_mode;
};

class T_NetworkBuffer : public T_NetworkWaiter
{
    Q_OBJECT
public:
    explicit T_NetworkBuffer(QNetworkReply *a_reply, const QString &a_name = "")
    {
        ctor_NetworkWaiter(a_reply, a_name, true);
    }
};

class T_NetworkBatch : public QObject
{
    Q_OBJECT
public:
    explicit T_NetworkBatch(QNetworkAccessManager *a_nam)
        : m_nam(a_nam)
    {
    }
    QNetworkReply *get(const QString &a_url)
    {
        return this->get(QUrl(a_url));
    }
    QNetworkReply *get(const QUrl &a_url)
    {
        return this->get(QNetworkRequest(a_url));
    }
    QNetworkReply *get(const QNetworkRequest &a_request)
    {
        QNetworkReply *v_reply = m_nam->get(a_request);
        T_NetworkWaiter v_waiter(v_reply);
        v_waiter.wait();
        return v_reply;
    }
    QNetworkReply *head(const QString &a_url)
    {
        return this->head(QUrl(a_url));
    }
    QNetworkReply *head(const QUrl &a_url)
    {
        return this->head(QNetworkRequest(a_url));
    }
    QNetworkReply *head(const QNetworkRequest &a_request)
    {
        QNetworkReply *v_reply = m_nam->head(a_request);
        T_NetworkWaiter v_waiter(v_reply);
        v_waiter.wait();
        return v_reply;
    }
protected:
    QNetworkAccessManager *m_nam;
};

class T_NetworkSequence : public QObject
{
    Q_OBJECT
public:
    explicit T_NetworkSequence(QNetworkReply *a_reply)
        : m_waiter(a_reply)
        , m_mutex(QMutex ::Recursive)
    {
        m_file.setBuffer(&m_file_bytes);
        m_file.open(QIODevice::ReadOnly);
    }
    bool isFinished()
    {
        return m_waiter.isFinished();
    }
    qint64 expectedSize()
    {
        return m_waiter.contentLength();
    }
    qint64 pos()
    {
        T_ATOMIC_LOCKER(v_lock, &m_mutex)
        return m_file.pos();
    }
    bool seek(qint64 offset)
    {
        T_ATOMIC_LOCKER(v_lock, &m_mutex)
        qint64 v_diff = offset - m_file.pos();
        if(v_diff > 0)
        {
            qint64 v_avail = m_file.bytesAvailable();
            if(v_avail < v_diff)
            {
                int v_wait = (v_diff-v_avail);
                m_file_bytes.append(m_waiter.wait_readAll(v_wait));
            }
        }
        return m_file.seek(offset);
    }
    qint64 availableSize()
    {
        ////qDebug() << "[NetworkSequence::availableSize()]";
        return m_file_bytes.size()+m_waiter.just_bytesAvailable();
    }
    QByteArray wait_read(int a_max)
    {
        T_ATOMIC_LOCKER(v_lock, &m_mutex)
        qint64 v_avail = m_file.bytesAvailable();
        if(v_avail < a_max)
        {
            m_file_bytes.append(m_waiter.wait_readAll(a_max-v_avail));
        }
        return m_file.read(a_max);
    }
protected:
    T_NetworkBuffer m_waiter;
    QMutex m_mutex;
    QByteArray m_file_bytes;
    QBuffer m_file;
};

#endif // NETWORKBATCHREQUEST_H
