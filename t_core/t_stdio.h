#ifndef T_STDIO_H
#define T_STDIO_H

#include <QtCore>
#include "t_lock.h"

#include <stdio.h>
#include <fcntl.h>
//#include <io.h>


class T_StdIn_Thread : public QThread
{
    Q_OBJECT
protected:
    T_Atomic *f_stdin_atom;
    QByteArray *f_stdin_array;
public:
    T_StdIn_Thread(T_Atomic *a_stdin_atom, QByteArray *a_stdin_array,
                   QObject *a_parent = 0)
        : QThread(a_parent)
        , f_stdin_atom(a_stdin_atom)
        , f_stdin_array(a_stdin_array)
    {
        //qDebug() << "T_StdIn_Thread::T_StdIn_Thread()";
        //this->start();
    }
    virtual void run()
    {
        //qDebug() << "T_StdIn_Thread::run(1)";
        for(;;)
        {
            //qDebug() << "T_StdIn_Thread::run(2)";
            char c = fgetc(stdin);
            if(c == EOF) break;
            {
                T_ATOMIC_LOCKER(v_lock, f_stdin_atom);
                f_stdin_array->append(c);
                //qDebug() << "T_StdIn_Thread::run(3)" << *f_stdin_array;
#if 0x0
                QMetaObject::invokeMethod(this, "emit_readyRead", Qt::QueuedConnection);
#else
                this->enqueue(SLOT(emit_readyRead()));
#endif
            }
        }
#if 0x0
        QMetaObject::invokeMethod(this, "emit_readChannelFinished", Qt::QueuedConnection);
#else
        this->enqueue(SLOT(emit_readChannelFinished()));
#endif
    }
protected:
    void enqueue(const char *a_member)
    {
        QByteArray v_member = a_member;
        Q_ASSERT(v_member.startsWith("1"));
        v_member.remove(0, 1);
        Q_ASSERT(v_member.endsWith("()"));
        v_member.replace("()", "");
        QMetaObject::invokeMethod(this, v_member.constData(), Qt::QueuedConnection);
    }
signals:
    void readyRead();
    void readChannelFinished();
protected slots:
    void emit_readyRead() { emit readyRead(); }
    void emit_readChannelFinished() { emit readChannelFinished(); }
};

class T_StdInOut : public QIODevice
{
    Q_OBJECT
protected:
    mutable T_Atomic f_stdin_atom;
    mutable T_Atomic f_stdout_atom;
    QByteArray f_stdin_total;
    QByteArray f_stdin_array;
    QBuffer f_stdin_buffer;
    T_StdIn_Thread f_stdin_thread;
public:
    T_StdInOut(QObject *a_parent = 0)
        : QIODevice(a_parent)
        //, f_stdin_buffer(&f_stdin_array)
        , f_stdin_buffer(&f_stdin_total)
        , f_stdin_thread(&f_stdin_atom, &f_stdin_array)
    {
        qDebug() << "[T_StdIn::T_StdIn()]";
        if (0 > _setmode( _fileno(stdin), _O_BINARY))
        {
            qDebug()<<"[NG(in)]";
        }
        else
        {
            qDebug()<<"[OK(in)]";
        }
        if (0 > _setmode( _fileno(stdout), _O_BINARY))
        {
            qDebug()<<"[NG(out)]";
        }
        else
        {
            qDebug()<<"[OK(out)]";
        }
        f_stdin_buffer.open(QIODevice::ReadOnly);
        this->open(QIODevice::ReadWrite);
        connect(&f_stdin_thread, SIGNAL(readyRead()), this, SLOT(self_thread_readyRead()));
        connect(&f_stdin_thread, SIGNAL(readChannelFinished()), this, SLOT(self_thread_finished()));
        qDebug() << "[f_stdin_thread.isFinished()]" << f_stdin_thread.isFinished();
        f_stdin_thread.start();
    }
public:
    virtual qint64 bytesAvailable() const
    {
        qDebug() << "[T_StdIn::bytesAvailable()]";
        T_ATOMIC_LOCKER(v_lock, &f_stdin_atom);
        return f_stdin_buffer.bytesAvailable();
    }
protected:
    void enqueue(const char *a_member)
    {
        ////qDebug() << "[T_StdInOut::enqueue] a" << a_member;
        QByteArray v_member = a_member;
        Q_ASSERT(v_member.startsWith("1"));
        v_member.remove(0, 1);
        Q_ASSERT(v_member.endsWith("()"));
        v_member.replace("()", "");
        ////qDebug() << "[T_StdInOut::enqueue] v" << v_member;
        QMetaObject::invokeMethod(this, v_member.constData(), Qt::QueuedConnection);
    }
    virtual qint64 readData(char *data, qint64 maxlen)
    {
        //qDebug() << "[T_StdIn::readData()]" << maxlen;
        T_ATOMIC_LOCKER(v_lock, &f_stdin_atom);
        QByteArray v_bytes = f_stdin_buffer.read(maxlen);
        memcpy(data, v_bytes.constData(), v_bytes.size());
        return v_bytes.size();
    }
    virtual qint64 writeData(const char *data, qint64 len)
    {
        //qDebug() << "[writeData()]" << len;
        T_ATOMIC_LOCKER(v_lock, &f_stdout_atom);
        size_t v_ret = fwrite(data, sizeof(char), (size_t)len, stdout);
        fflush(stdout);
        return v_ret;
    }
protected slots:
protected slots:
    void emit_readyRead() { emit readyRead(); }
    void emit_readChannelFinished() { emit readChannelFinished(); }
    void self_thread_readyRead()
    {
        T_ATOMIC_LOCKER(v_lock, &f_stdin_atom);
        f_stdin_total.append(f_stdin_array);
        f_stdin_array.clear();
#if 0x0
        QMetaObject::invokeMethod(this, "emit_readyRead", Qt::QueuedConnection);
#else
        this->enqueue(SLOT(emit_readyRead()));
#endif
    }
    void self_thread_finished()
    {
#if 0x0
        QMetaObject::invokeMethod(this, "emit_readChannelFinished", Qt::QueuedConnection);
#else
        this->enqueue(SLOT(emit_readChannelFinished()));
#endif
    }
};

#endif // T_STDIO_H
