#include "moduledb.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "t_core/t_time.h"

static QString s_module_path(const QFile &a_data_dir, const QString &a_module_name)
{
#if 0x1
    return a_data_dir.fileName() + "/" + a_module_name;
#else
    QString v_result = a_data_dir.fileName() + "/" + a_module_name;
    qDebug() << v_result;
    return v_result;
#endif
}

static QUuid s_module_md5_test(QFile &a_file)
{
    qDebug() << "[s_module_md5_test()]" << a_file.fileName();
    if (a_file.open(QIODevice::ReadOnly)) {
        QByteArray fileData = a_file.readAll();
        QByteArray hashData = QCryptographicHash::hash(fileData, QCryptographicHash::Md5);
        qDebug() << "==>[s_module_md5_test()]" <<  hashData.toHex();
        QUuid v_uuid = QUuid::fromRfc4122(hashData);
        qDebug() << "==>[s_module_md5_test()]" <<  v_uuid.toString();
        return v_uuid;
    }
    else
    {
        return QUuid();
    }
}

static void s_module_md5_test(const QFile &a_data_dir, const QString &a_module_name)
{
    T_AutoRunClock v_clock;
    QDateTime v_now1 = QDateTime::currentDateTime();
    QFile v_real_file(s_module_path(a_data_dir, a_module_name));
    QUuid v_real_uuid = s_module_md5_test(v_real_file);
    QFile v_reso_file(":/bin/" + a_module_name);
    QUuid v_reso_uuid = s_module_md5_test(v_reso_file);
    qDebug() << (v_real_uuid == v_reso_uuid);
    QDateTime v_now2 = QDateTime::currentDateTime();
    qDebug() << v_now1.msecsTo(v_now2);
    qDebug() << v_clock.elapsed();
}

#if 0x0
static bool s_module_exists(const QFile &a_data_dir, const QString &a_module_name)
{
    return QFile::exists(s_module_path(a_data_dir, a_module_name));
}
#endif

static bool s_module_copy(const QFile &a_data_dir, const QString &a_module_name)
{
    return QFile::copy(":/bin/" + a_module_name, s_module_path(a_data_dir, a_module_name));
}

ModuleDB::ModuleDB(QObject *parent) :
    QObject(parent),
    f_trial_flag(false)
{
    QStringList v_data_dir_list = QStandardPaths::standardLocations(QStandardPaths::DataLocation);
    qDebug() << "[v_data_dir_list]" << v_data_dir_list;
    QString v_data_dir = v_data_dir_list.at(0);
    f_data_dir.setFileName(v_data_dir);
    QDir v_root = QDir::root();
    qDebug() << v_root.absolutePath();
    if(!v_root.mkpath(v_data_dir))
    {
        return;
    }
    s_module_copy(f_data_dir, MODULE_PCI_Z);
    s_module_copy(f_data_dir, MODULE_YT_DL);
    s_module_md5_test(f_data_dir, MODULE_YT_DL);
}

QString ModuleDB::getPcizPath() const
{
    return s_module_path(f_data_dir, MODULE_PCI_Z);
}

QString ModuleDB::getYtDlPath() const
{
#if 0x0
    return s_module_path(f_data_dir, MODULE_YT_DL);
#else
    QString v_result = s_module_path(f_data_dir, MODULE_YT_DL);
    qDebug() << "[ModuleDB::getYtDlPath()]" << v_result;
    return v_result;
#endif
}
