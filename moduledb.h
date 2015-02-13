#ifndef MODULEDB_H
#define MODULEDB_H

#include <QtCore>

#define MODULE_PCI_Z "PCI-Z-1.3.exe"
////#define MODULE_YT_DL "youtube-dl-2014.08.25.2.exe"
////#define MODULE_YT_DL "youtube-dl-2014.10.02.exe"
#define MODULE_YT_DL "youtube-dl-2015.02.11.exe"

class ModuleDB : public QObject
{
    Q_OBJECT
public:
    explicit ModuleDB(QObject *parent = 0);
    QString getPcizPath() const;
    QString getYtDlPath() const;

public:
    bool f_trial_flag;

protected:
    QFile f_data_dir;

signals:

public slots:

};

#endif // MODULEDB_H
