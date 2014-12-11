#ifndef T_GLOBAL_H
#define T_GLOBAL_H

#include <QtCore>
#include <QtNetwork>

#define T_DECLARE_PRIVATE(Class) \
class Class##Private *d_ptr; \
    Q_DECLARE_PRIVATE(Class)

#define T_DECLARE_PUBLIC(Class) \
    class Class *q_ptr; \
    Q_DECLARE_PUBLIC(Class)

#define T_INIT_QPTR(X) QObject(X), q_ptr(X)
#define T_INIT_DPTR(Class) d_ptr = new Class##Private
#define T_D(X) Q_D(X); Q_UNUSED(d)

#ifdef __GNUC__
#    define T_GCC_VERSION_AT_LEAST(x,y) (__GNUC__ > x || __GNUC__ == x && __GNUC_MINOR__ >= y)
#else
#    define T_GCC_VERSION_AT_LEAST(x,y) 0
#endif

#ifndef T_DEPRECATED
#if T_GCC_VERSION_AT_LEAST(3,1)
#    define T_DEPRECATED __attribute__((deprecated))
#else
#    define T_DEPRECATED
#endif
#endif

extern bool gEnqueueMethod(
        QObject *a_object,
        const char *a_member,
        QGenericArgument val0 = QGenericArgument(0),
        QGenericArgument val1 = QGenericArgument(),
        QGenericArgument val2 = QGenericArgument(),
        QGenericArgument val3 = QGenericArgument(),
        QGenericArgument val4 = QGenericArgument(),
        QGenericArgument val5 = QGenericArgument(),
        QGenericArgument val6 = QGenericArgument(),
        QGenericArgument val7 = QGenericArgument(),
        QGenericArgument val8 = QGenericArgument(),
        QGenericArgument val9 = QGenericArgument());

extern QString gNetworkRequestToString(const QNetworkRequest &a_request);

extern void gWinMsgHandler(QtMsgType t, const char* str);

#endif // T_GLOBAL_H
