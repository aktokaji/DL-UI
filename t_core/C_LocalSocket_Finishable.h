#ifndef C_LOCALSOCKET_FINISHABLE_H
#define C_LOCALSOCKET_FINISHABLE_H

#include <QtCore>
#include <QtNetwork>

class C_LocalSocket_Finishable : public QIODevice
{
    Q_OBJECT
protected:
    QLocalSocket *f_socket;
    qint64 f_socket_blockSize;
    QBuffer f_buffer;
public:
    explicit C_LocalSocket_Finishable(QLocalSocket *socket, QObject *parent = 0)
        : QIODevice(parent)
        , f_socket(socket)
        , f_socket_blockSize(0)
    {
        Q_ASSERT(socket);
        f_socket->setParent(this);
        this->open(QIODevice::ReadWrite);
        bool b = f_buffer.open(QIODevice::ReadOnly);
        qDebug() << "[b]" << b;
        connect(f_socket, SIGNAL(aboutToClose()), this, SIGNAL(aboutToClose()));
        connect(f_socket, SIGNAL(bytesWritten(qint64)), this, SIGNAL(bytesWritten(qint64)));
        connect(f_socket, SIGNAL(connected()), this, SIGNAL(connected()));
        connect(f_socket, SIGNAL(destroyed()), this, SIGNAL(destroyed()));
        connect(f_socket, SIGNAL(destroyed(QObject*)), this, SIGNAL(destroyed(QObject*)));
        connect(f_socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
        connect(f_socket, SIGNAL(error(QLocalSocket::LocalSocketError)),
                this, SIGNAL(error(QLocalSocket::LocalSocketError)));
        connect(f_socket, SIGNAL(readChannelFinished()), this, SIGNAL(readChannelFinished()));
//#if 0x0
//        connect(f_socket, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
//#else
        //connect(f_socket, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
        connect(f_socket, SIGNAL(readyRead()), this, SLOT(self_socket_readyRead()));
//#endif
    }
    virtual ~C_LocalSocket_Finishable()
    {
    }

    QByteArray _buffer()
    {
        return f_buffer.buffer();
    }
    qint64 _pos()
    {
        return f_buffer.pos();
    }

    void connectToServer(const QString &name, OpenMode openMode = ReadWrite)
    {
        f_socket->connectToServer(name, openMode);
    }
    void disconnectFromServer()
    {
        f_socket->disconnectFromServer();
    }

    QString serverName() const
    {
        return f_socket->serverName();
    }
    QString fullServerName() const
    {
        return f_socket->fullServerName();
    }

    void abort()
    {
        f_socket->abort();
    }
    virtual bool isSequential() const
    {
        return f_socket->isSequential();
    }
    virtual qint64 bytesAvailable() const
    {
        //return f_socket->bytesAvailable();
        return f_buffer.bytesAvailable();
    }
    virtual qint64 bytesToWrite() const
    {
        return f_socket->bytesToWrite();
    }
    virtual bool canReadLine() const
    {
        //return f_socket->canReadLine();
        return f_buffer.canReadLine();
    }
    virtual void close()
    {
        f_socket->close();
    }
    QLocalSocket::LocalSocketError error() const
    {
        return f_socket->error();
    }
    bool flush()
    {
        return f_socket->flush();
    }
    bool isValid() const
    {
        return f_socket->isValid();
    }
    qint64 readBufferSize() const
    {
        return f_socket->readBufferSize();
    }
    void setReadBufferSize(qint64 size)
    {
        f_socket->setReadBufferSize(size);
    }

    bool setSocketDescriptor(quintptr socketDescriptor,
                             QLocalSocket::LocalSocketState socketState = QLocalSocket::ConnectedState,
                             OpenMode openMode = ReadWrite)
    {
        return f_socket->setSocketDescriptor(socketDescriptor, socketState, openMode);
    }
    quintptr socketDescriptor() const
    {
        return f_socket->socketDescriptor();
    }

    QLocalSocket::LocalSocketState state() const
    {
        return f_socket->state();
    }
    bool waitForBytesWritten(int msecs = 30000)
    {
        return f_socket->waitForBytesWritten(msecs);
    }
    bool waitForConnected(int msecs = 30000)
    {
        return f_socket->waitForConnected(msecs);
    }
    bool waitForDisconnected(int msecs = 30000)
    {
        return f_socket->waitForDisconnected(msecs);
    }
    bool waitForReadyRead(int msecs = 30000)
    {
        return f_socket->waitForReadyRead(msecs);
    }

    void writeEOF()
    {
        qDebug() << "[C_LocalSocket_Finishable::writeEOF()]";
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);
        out << (qint64)-1;
        f_socket->write(block);
        f_socket->flush();
    }

Q_SIGNALS:
    void connected();
    void disconnected();
    void error(QLocalSocket::LocalSocketError socketError);
    void stateChanged(QLocalSocket::LocalSocketState socketState);
    void finished();

protected slots:
    void self_socket_readyRead()
    {
        Q_ASSERT(f_socket_blockSize != -1);

        QByteArray v_bytes = p_socket_readable();

        if(v_bytes.isEmpty())
        {
            if(f_socket_blockSize == -1)
            {
                QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
                return;
            }
            return;
        }

        f_buffer.buffer().append(v_bytes);
        QMetaObject::invokeMethod(this, "readyRead", Qt::QueuedConnection);

        if(f_socket_blockSize == -1)
        {
            QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
        }
    }
protected:
    //virtual qint64 readData(char *data, qint64 maxlen) = 0;
    virtual qint64 readData(char * data, qint64 maxSize)
    {
        qDebug() << "[C_LocalSocket_Finishable::readData()]" << maxSize;
        return f_buffer.read(data, maxSize);
    }
    //virtual qint64 readLineData(char *data, qint64 maxlen);
    qint64 readLineData(char *data, qint64 maxlen)
    {
        return f_buffer.readLine(data, maxlen);
    }
    //virtual qint64 writeData(const char *data, qint64 len) = 0;
    virtual qint64 writeData(const char * data, qint64 c)
    {
        Q_ASSERT(c>=0 && c<=LONG_MAX);
        QByteArray a_bytes(data, c);
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);
        out << (qint64)0;
        out << a_bytes;
        out.device()->seek(0);
        out << (qint64)(block.size() - sizeof(qint64));
        f_socket->write(block);
        f_socket->flush();
        return a_bytes.size();
    }
protected:
    QByteArray p_socket_readable()
    {
        if(f_socket_blockSize == -1)
        {
            return QByteArray();
        }
        QByteArray v_result;
        QDataStream in(f_socket);
        in.setVersion(QDataStream::Qt_4_0);
        while(f_socket->bytesAvailable())
        {
            if (f_socket_blockSize == 0)
            {
                if (f_socket->bytesAvailable() < (int)sizeof(qint64)) return v_result;
                in >> f_socket_blockSize;
            }
            if(f_socket_blockSize == -1)
            {
                return v_result;
            }
            if (f_socket->bytesAvailable() < f_socket_blockSize) return v_result;
            Q_ASSERT(f_socket_blockSize != -1);
            f_socket_blockSize = 0;
            QByteArray v_bytes;
            in >> v_bytes;
            v_result.append(v_bytes);
        }
        return v_result;
    }
};

#endif // C_LOCALSOCKET_FINISHABLE_H
