#ifndef DLPROGRESS_H
#define DLPROGRESS_H

#include <QtCore>
#include <QWidget>

#include "moduledb.h"

namespace Ui {
class DLProgress;
}

class Youtube_Video_Info
{
public:
    QUrl f_watch_url;
    QString f_title;
    QString f_filename;
    QString f_stream_url;
    QMap<int, QUrl> f_fmt_map;
};

class DLProgress : public QWidget
{
    Q_OBJECT

public:
    explicit DLProgress(ModuleDB *a_moduel_db, const QUrl &a_url, QWidget *parent = 0);
    ~DLProgress();

private:
    Ui::DLProgress *ui;

protected slots:
    void slot_meta_finished();
    void slot_stream_readyRead();
    void slot_stream_finished();
    void slot_stream_stateChanged(QProcess::ProcessState a_state);
    void slot_timer_timeout();
protected:
    void parse_download_progress(QByteArray a_bytes);
protected:
    ModuleDB *f_moduel_db;
    QUrl f_input_url;
    Youtube_Video_Info f_video_info;
    QProcess f_proc_meta;
    QProcess f_proc_stream;
    QTimer f_stream_timer;
};

#endif // DLPROGRESS_H
