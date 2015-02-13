#include "dlprogress.h"
#include "ui_dlprogress.h"

#include <QtCore>

static bool parse_youtube_dl_json(const QUrl &a_url, const QByteArray &a_json_bytes, Youtube_Video_Info *a_video_info)
{
    Q_ASSERT(!!a_video_info);
    a_video_info->f_watch_url = a_url;
    a_video_info->f_title = "";
    a_video_info->f_filename = "";
    a_video_info->f_stream_url = "";
    a_video_info->f_fmt_map.clear();

    QByteArray v_bytes = a_json_bytes;
    v_bytes = v_bytes.trimmed();

    if(v_bytes.isEmpty()) return false;

#if 0x1
    QFile debugJsonFile("C:/Users/root/Desktop/debugJson.txt");
    debugJsonFile.open(QIODevice::WriteOnly);
    debugJsonFile.write(v_bytes);
    debugJsonFile.close();
#endif

    QJsonDocument d = QJsonDocument::fromJson(v_bytes);
    QVariantMap result = d.toVariant().toMap();

    a_video_info->f_title = result["title"].toString();
    a_video_info->f_filename = result["_filename"].toString();
    a_video_info->f_stream_url = result["url"].toString();

    foreach (QVariant format, result["formats"].toList()) {
        QVariantMap v_map = format.toMap();
        //qDebug() << "[format_id]" << v_map["format_id"].toInt();
        //qDebug() << "[url]" << v_map["url"].toString();
        a_video_info->f_fmt_map[v_map["format_id"].toInt()] = v_map["url"].toString();
    }

    return true;
}

DLProgress::DLProgress(ModuleDB *a_moduel_db, const QUrl &a_url, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DLProgress),
    f_moduel_db(a_moduel_db),
    f_input_url(a_url)
{
    ui->setupUi(this);
    ui->label->setText("");
    qDebug() << "[DLProgress::DLProgress()]" << a_url;
    ui->progressBar->setValue(0); /**/


    ui->labelWatchUrl->setText(a_url.toString());

    // [f_proc_meta]
    QStringList v_args;
    v_args.clear();
    v_args << "-j";
    if(f_moduel_db->f_trial_flag)
    {
        //v_args << "-o" << "[体験版][%(extractor)s]%(title)s-%(id)s-(fmt=%(format_id)s).%(ext)s";
        v_args << "-o" << "[体験版]%(title)s-%(id)s.%(ext)s";
    }
    if(a_url.host().endsWith("nicovideo.jp"))
    {
        v_args << "-u" << "y2suzu@gmail.com";
        v_args << "-p" << "rendertech";
    }
    v_args << a_url.toString();
    connect(&f_proc_meta, SIGNAL(finished(int)),
            this, SLOT(slot_meta_finished()));
    qDebug() << (long)f_moduel_db;
    qDebug() << f_moduel_db->getYtDlPath();
    f_proc_meta.start(f_moduel_db->getYtDlPath(), v_args);
}

DLProgress::~DLProgress()
{
    qDebug() << "[DLProgress::~DLProgress()]";
    delete ui;
}

void DLProgress::slot_meta_finished()
{
    QByteArray v_json_bytes = f_proc_meta.readAllStandardOutput();
    parse_youtube_dl_json(f_input_url, v_json_bytes, &f_video_info);
    qDebug() << "[f_video_info.f_title]" << f_video_info.f_title;
    qDebug() << "[f_video_info.f_filename]" << f_video_info.f_filename;
    //ui->labelWatchUrl->setText("タイトル: " + f_video_info.f_title);
    ui->labelWatchUrl->setText("ファイル名: " + f_video_info.f_filename);
    // [f_proc_stream]
    QStringList v_args;
    v_args.clear();
    v_args << "--newline";
    v_args << "--no-mtime";
    if(f_moduel_db->f_trial_flag)
    {
        //v_args << "-o" << "[体験版][%(extractor)s]%(title)s-%(id)s-(fmt=%(format_id)s).%(ext)s";
        v_args << "-o" << "[体験版]%(title)s-%(id)s.%(ext)s";
    }
    if(f_input_url.host().endsWith("nicovideo.jp"))
    {
        v_args << "-u" << "y2suzu@gmail.com";
        v_args << "-p" << "rendertech";
    }
    v_args << f_input_url.toString();
    connect(&f_proc_stream, SIGNAL(readyRead()),
            this, SLOT(slot_stream_readyRead()));
    connect(&f_proc_stream, SIGNAL(finished(int)),
            this, SLOT(slot_stream_finished()));
    connect(&f_proc_stream, SIGNAL(stateChanged(QProcess::ProcessState)),
            this, SLOT(slot_stream_stateChanged(QProcess::ProcessState)));
    if(f_moduel_db->f_trial_flag)
    {
        connect(&f_stream_timer, SIGNAL(timeout()),
                this, SLOT(slot_timer_timeout()));
        f_stream_timer.start(1000);
    }
    f_proc_stream.start(f_moduel_db->getYtDlPath(), v_args);
}

void DLProgress::slot_stream_readyRead()
{
    while(f_proc_stream.canReadLine())
    {
        QByteArray v_bytes = f_proc_stream.readLine();
        v_bytes = v_bytes.trimmed();
        qDebug() << v_bytes;
#if 0x0
        this->ui->label->setText(v_bytes);
#else
        QString v_msg = QString::fromLocal8Bit(v_bytes);
        this->ui->label->setText(v_msg);
#endif
        if(v_bytes.startsWith("[download]"))
        {
            parse_download_progress(v_bytes);
        }
    }
}

void DLProgress::slot_stream_finished()
{
    qDebug() << "[DLProgress::slot_finished()]" << QDir::currentPath();
    qDebug() << "[f_video_info.f_filename]" << f_video_info.f_filename;
#if 0x0
    QDir v_cwd = QDir::current();
    QFile v_file(v_cwd.absoluteFilePath(f_video_info.f_filename));
#else
    QFile v_file(f_video_info.f_filename);
#endif
    qDebug() << "[v_file.fileName()]" << v_file.fileName();
    if(v_file.exists())
    {
        ui->progressBar->setValue(100);
        QStringList v_args;
        v_args << "/select,";
        v_args << QDir::toNativeSeparators(v_file.fileName());
        QProcess::startDetached("explorer.exe", v_args);
    }
    else if(f_moduel_db->f_trial_flag)
    {
        QFile v_file_part(f_video_info.f_filename + ".part");
        if(v_file_part.exists())
        {
            v_file_part.rename(v_file.fileName());
            QStringList v_args;
            v_args << "/select,";
            v_args << QDir::toNativeSeparators(v_file.fileName());
            QProcess::startDetached("explorer.exe", v_args);
        }
    }
}

void DLProgress::slot_stream_stateChanged(QProcess::ProcessState a_state)
{
    qDebug() << "[DLProgress::slot_stream_stateChanged]" << a_state;
}

void DLProgress::slot_timer_timeout()
{
    qDebug() << "[DLProgress::slot_timer_timeout]" << f_proc_stream.state();
    if(f_proc_stream.state() == QProcess::NotRunning)
    {
        f_stream_timer.stop();
        this->slot_stream_finished();
    }
}

void DLProgress::parse_download_progress(QByteArray a_bytes)
{
    Q_ASSERT(a_bytes.startsWith("[download]"));
    //this->ui->label->setText(a_bytes);
    QRegExp rx("([0-9.]+)%");
    //rx.setMinimal(true);
    int pos = rx.indexIn(a_bytes);
    if (pos > -1) {
        qDebug() << "Captured texts: " << rx.capturedTexts();
        qDebug() << "rx.cap(0): " <<rx.cap(0);
        qDebug() << "rx.cap(1): " <<rx.cap(1);
        qDebug() << "rx.cap(1).toInt(): " << rx.cap(1).toDouble();
        ui->progressBar->setValue((int)rx.cap(1).toDouble());
        if(f_moduel_db->f_trial_flag)
        {
            if(ui->progressBar->value()>=50)
            {
                f_proc_stream.kill();
            }
        }
    } else qDebug() << "No indexin";
}
