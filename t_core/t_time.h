#ifndef T_TIME_H
#define T_TIME_H

#include <QtCore>

class T_Clock
{
protected:
    bool m_active;
    qint64 m_elapsed;
    QDateTime m_started;
public:
    explicit T_Clock()
        : m_active(false)
        , m_elapsed(0)
    {
    }
    bool isActive()
    {
        return m_active;
    }
    void start()
    {
        if(m_active) return;
        m_started = QDateTime::currentDateTime();
        m_active = true;
    }
    void stop()
    {
        if(!m_active) return;
        QDateTime v_now = QDateTime::currentDateTime();
        m_elapsed += p_msecsTo(m_started, v_now);
        m_active = false;
    }
    void reset()
    {
        m_elapsed = 0;
        m_started = QDateTime::currentDateTime();
    }
    void restart()
    {
        reset();
        start();
    }
    qint64 elapsed() const
    {
        if(!m_active) return m_elapsed;
        QDateTime v_now = QDateTime::currentDateTime();
        //int v_result = m_elapsed + p_msecsTo(m_started, v_now);
        int v_result = m_elapsed + m_started.msecsTo(v_now);
        return v_result;
    }
    void setElapsed(qint64 a_msec)
    {
        if(a_msec < 0) return;
        m_elapsed = a_msec;
        m_started = QDateTime::currentDateTime();
    }
    QString toString(const QString &a_format)
    {
        QTime v_elapsed_clock(0, 0, 0, 0);
        v_elapsed_clock = v_elapsed_clock.addMSecs(this->elapsed());
        return v_elapsed_clock.toString(a_format);
    }
protected:
    qint64 p_msecsTo(const QDateTime &a, const QDateTime &b) const
    {
        static const int c_daysToMillisecs = 24 * 3600 * 1000;
        int v_daysTo = a.date().daysTo(b.date());
        int v_msecsTo = a.time().msecsTo(b.time());
        if(v_daysTo == 0) return v_msecsTo;
        return v_msecsTo + (v_daysTo * c_daysToMillisecs);
    }
};

class T_AutoRunClock : public T_Clock
{
public:
    explicit T_AutoRunClock() : T_Clock()
    {
        this->start();
        Q_ASSERT(this->elapsed()>=0);
    }
};

class T_FPS
{
public:
    T_Clock m_time_line;
    qint64 m_interval;
    qint64 m_count;
    qint64 m_last_set;
    double m_last_fps;
    explicit T_FPS(qint64 a_interval = 1000)
        : m_interval(a_interval)
        , m_count(0)
        , m_last_set(0)
        , m_last_fps(0)
    {
    }
    void start()
    {
        m_time_line.start();
    }
    void restart()
    {
        m_count = 0;
        m_time_line.restart();
    }
    void add(int a)
    {
        m_count += a;
    }
    void set(int a)
    {
        int v_diff = a - m_last_set;
        m_last_set = a;
        m_count += v_diff;
    }
    double calculate()
    {
        qint64 v_elapsed = m_time_line.elapsed();
        double v_value = v_elapsed==0 ? 0 : 1000.0 * m_count / v_elapsed;
        return v_value;
    }
    double value()
    {
        if(!m_time_line.isActive())
        {
            qDebug() << "[T_FPS.value()] warning: T_FPS::start() was not called.";
            return 0;
        }
        else if(m_time_line.elapsed() < m_interval)
        {
            return m_last_fps;
        }
        else
        {
            m_last_fps = this->calculate();
            m_count = 0;
            m_time_line.reset();
            return m_last_fps;
        }
    }
};

class T_AutoRunFPS : public T_FPS
{
public:
    explicit T_AutoRunFPS(qint64 a_interval = 1000)
        : T_FPS(a_interval)
    {
        this->start();
    }
};

#endif // T_TIME_H
