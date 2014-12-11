#ifndef DLAPP_H
#define DLAPP_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class DLApp : public QApplication
{
    Q_OBJECT
public:
    explicit DLApp(int &argc, char **argv, int flags = ApplicationFlags);
    bool MasterPasswordIsSet();
    QByteArray EncodeMasterPassword(const QString &a_password, const QString &a_text);
    QString DecodeMasterPassword(const QString &a_password, const QByteArray a_encBytes);

signals:

public slots:

};

DLApp *DLApp_Get();

#endif // DLAPP_H
