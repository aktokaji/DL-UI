#ifndef T_THREAD_H
#define T_THREAD_H

#include "t_global.h"

class T_Sleeper : public QThread
{
public:
    static void msleep(unsigned long a_msecs)
    {
        QThread::msleep(a_msecs);
    }
    static void usleep(unsigned long a_usecs)
    {
        QThread::usleep(a_usecs);
    }
};

#endif // T_THREAD_H
