#include "masterpassworddialog.h"

////#include <QtCore>
#include <QtWebKitWidgets>

#include "dlapp.h"

class MasterPasswordDialog : public QWebView
{
    Q_OBJECT
public:
    explicit MasterPasswordDialog(QWidget *a_parent) : QWebView(a_parent)
    {
        connect(this->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
                this, SLOT(populateJavaScriptWindowObject()));

        this->setFixedSize(400, 200);
        this->setWindowTitle("マスターパスワード入力");
        this->setUrl(QUrl("qrc:/page/masterpassworddialog.html"));
    }
    ~MasterPasswordDialog()
    {
        qDebug() << "[~MasterPasswordDialog()]";
    }

    Q_PROPERTY(QString nico_mail READ getNicoMail)
    QString f_nico_mail;
    QString getNicoMail()
    {
        QSettings v_settings( // C:\Users\root\AppData\Roaming\akio.tokaji\yt_dlui.ini
                              QSettings::IniFormat, //QSettings::NativeFormat,
                              QSettings::UserScope, // QSettings::SystemScope,
                              QCoreApplication::organizationName(),
                              QCoreApplication::applicationName()
                              );
        return v_settings.value("nicovideo/mail").toString();
    }

    Q_PROPERTY(QString nico_pass READ getNicoPass)
    QString f_nico_pass;
    QString getNicoPass()
    {
        QSettings v_settings( // C:\Users\root\AppData\Roaming\akio.tokaji\yt_dlui.ini
                              QSettings::IniFormat, //QSettings::NativeFormat,
                              QSettings::UserScope, // QSettings::SystemScope,
                              QCoreApplication::organizationName(),
                              QCoreApplication::applicationName()
                              );
        return v_settings.value("nicovideo/password").toString();
    }

signals:

public slots:
    bool checkMasterPassword(const QString &a_password)
    {
        QSettings v_settings( // C:\Users\root\AppData\Roaming\akio.tokaji\yt_dlui.ini
                              QSettings::IniFormat, //QSettings::NativeFormat,
                              QSettings::UserScope, // QSettings::SystemScope,
                              QCoreApplication::organizationName(),
                              QCoreApplication::applicationName()
                              );
        QByteArray v_bytes = v_settings.value("global/master-password-encoded").toByteArray();
        QString v_text = DLApp_Get()->DecodeMasterPassword(a_password, v_bytes);
        if(v_text.isEmpty()) return false;
        return true;
    }
    int checkMasterPassword2(const QString &a_password)
    {
        qDebug() << "[checkMasterPassword2()]";
        QSettings v_settings( // C:\Users\root\AppData\Roaming\akio.tokaji\yt_dlui.ini
                              QSettings::IniFormat, //QSettings::NativeFormat,
                              QSettings::UserScope, // QSettings::SystemScope,
                              QCoreApplication::organizationName(),
                              QCoreApplication::applicationName()
                              );
        if(!DLApp_Get()->MasterPasswordIsSet()) return -1;
        ////if(!v_settings.contains("global/master-password-encoded")) return -1;
        QByteArray v_bytes = v_settings.value("global/master-password-encoded").toByteArray();
        QString v_text = DLApp_Get()->DecodeMasterPassword(a_password, v_bytes);
        if(v_text.isEmpty()) return 0;
        return 1;
    }
    void saveMasterPassword(const QString &a_password)
    {
        QSettings v_settings( // C:\Users\root\AppData\Roaming\akio.tokaji\yt_dlui.ini
                              QSettings::IniFormat, //QSettings::NativeFormat,
                              QSettings::UserScope, // QSettings::SystemScope,
                              QCoreApplication::organizationName(),
                              QCoreApplication::applicationName()
                              );
        v_settings.setValue("global/master-password", a_password);

        QByteArray v_bytes = DLApp_Get()->EncodeMasterPassword(a_password, "This is test text (これはテストテキストです).");
        v_settings.setValue("global/master-password-encoded", v_bytes);

        QString v_text = DLApp_Get()->DecodeMasterPassword(a_password, v_bytes);
        qDebug() << "[v_text]" << v_text;
        v_settings.setValue("global/v_text", v_text);

        QStringList v_args;
        v_args << "/select,";
        //v_args << "C:\\Users\\root\\AppData\\Roaming\\akio.tokaji\\yt_dlui.ini";
        v_args << QDir::toNativeSeparators(v_settings.fileName());
        qDebug() << v_args;
        QProcess::startDetached("explorer.exe", v_args);
    }

protected slots:
    void populateJavaScriptWindowObject()
    {
        this->page()->mainFrame()->addToJavaScriptWindowObject("webview", this);
    }
};

void MasterPasswordDialog_ShowModal(QWidget *a_parent)
{
    qDebug() << "[MasterPasswordDialog_ShowModal(begin)]";
    MasterPasswordDialog widget(a_parent);
    //widget->setAttribute(Qt::WA_DeleteOnClose);
    widget.setWindowFlags(Qt::Dialog | Qt::SubWindow);
    widget.setWindowModality(Qt::WindowModal);
    widget.show();
    while(widget.isVisible())
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
    qDebug() << "[MasterPasswordDialog_ShowModal(end)]";
}

#include "masterpassworddialog.moc"
