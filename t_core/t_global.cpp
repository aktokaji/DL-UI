#include "t_global.h"
#include "t_lock.h"
#include <windows.h> //OutputDebugString

bool gEnqueueMethod(
        QObject *a_object,
        const char *a_member,
        QGenericArgument val0,
        QGenericArgument val1,
        QGenericArgument val2,
        QGenericArgument val3,
        QGenericArgument val4,
        QGenericArgument val5,
        QGenericArgument val6,
        QGenericArgument val7,
        QGenericArgument val8,
        QGenericArgument val9)
{
    QByteArray v_member = a_member;
#if 0x0
    Q_ASSERT(v_member.startsWith("1"));
    v_member.remove(0, 1);
#else
    if(v_member.startsWith("1")||v_member.startsWith("2"))
    {
        v_member.remove(0, 1);
    }
#endif
    int v_idx = v_member.indexOf("(");
#if 0x0
    Q_ASSERT(v_idx != -1);
    //v_member.replace("()", "");
    v_member = v_member.left(v_idx);
    v_member = v_member.trimmed();
#else
    if(v_idx != -1)
    {
        v_member = v_member.left(v_idx);
        v_member = v_member.trimmed();
    }
#endif
    bool v_success =
            QMetaObject::invokeMethod(
                    a_object, v_member.constData(), Qt::QueuedConnection
                    , val0, val1, val2, val3, val4, val5, val6, val7, val8, val9);
    if(!v_success)
    {
        qDebug() << "[enqueue] failed." << v_member;
    }
    return v_success;
}

QString gNetworkRequestToString(const QNetworkRequest &a_request)
{
    QByteArray v_result;
    v_result.append("{QNetworkRequest: ");
    v_result.append(a_request.url().toEncoded());
    v_result.append("\n");
    QList<QByteArray> v_keys = a_request.rawHeaderList();
    foreach(QByteArray v_key, v_keys)
    {
        v_result.append("    [");
        v_result.append(v_key);
        v_result.append("]: ");
        v_result.append(a_request.rawHeader(v_key));
        v_result.append("\n");
    }
    v_result.append("}");
    return QLatin1String(v_result);
}

void gWinMsgHandler(QtMsgType t, const char* str)
{
    Q_UNUSED(t);
    // OutputDebugString is not threadsafe.

    // cannot use QMutex here, because qWarning()s in the QMutex
    // implementation may cause this function to recurse
    static T_CriticalSection staticCriticalSection;

    if (!str)
        str = "(null)";

    staticCriticalSection.lock();

#if 0x0
    QString s(QString::fromLocal8Bit(str));
    s += QLatin1Char('\n');
    OutputDebugString((wchar_t*)s.utf16());
#else
    //const int c_output_max = 1024;
    //const int c_output_max = 2048;
    //const int c_output_max = 4096;
    //const int c_output_max = 16000;
    //const int c_output_max = 26000;
    const int c_output_max = 30000;
    //const int c_output_max = 35000;
    //const int c_output_max = 40960;
    QString s(QString::fromLocal8Bit(str));
    s += QLatin1Char('\n');
    for(int i=0; i<s.size(); i+=c_output_max)
    {
        OutputDebugString((wchar_t*)s.mid(i, c_output_max).utf16());
    }
//    QString n1 = QString::number(s.size());
//    if(s.size() >= c_output_max) s = s.left(c_output_max);
//    QString n2 = QString::number(s.size());
//    s = "[" + n1 + ":" + n2 + "] " + s;
//    s += QLatin1Char('\n');
//    OutputDebugString((wchar_t*)s.utf16());
#endif

    staticCriticalSection.unlock();
}
