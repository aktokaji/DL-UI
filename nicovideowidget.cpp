#include "nicovideowidget.h"

#include <QtCore>
#include <QtWebKitWidgets>

class NicoVideoWidget : public QWebView
{
    Q_OBJECT
public:
    explicit NicoVideoWidget(QWidget *a_parent) : QWebView(a_parent)
    {
        this->setFixedSize(400, 200);

        connect(this->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
                this, SLOT(populateJavaScriptWindowObject()));

        this->setUrl(QUrl("qrc:/page/nicodialog.html"));
    }
    ~NicoVideoWidget()
    {
        qDebug() << "[~NicoVideoWidget()]";
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
    ////void setNicoMail(const QString &a_nico_mail) {  }

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
    ////void setNicoPass(const QString &a_nico_pass) {  }

signals:

public slots:
    void on_button_ok(const QString &a_nico_mail, const QString &a_nico_pass)
    {
        qDebug() << "[NicoVideoWidget::on_button_ok()]" << a_nico_mail << a_nico_pass;

        QSettings v_settings( // C:\Users\root\AppData\Roaming\akio.tokaji\yt_dlui.ini
                              QSettings::IniFormat, //QSettings::NativeFormat,
                              QSettings::UserScope, // QSettings::SystemScope,
                              QCoreApplication::organizationName(),
                              QCoreApplication::applicationName()
                              );
        v_settings.setValue("nicovideo/mail",     a_nico_mail);
        v_settings.setValue("nicovideo/password", a_nico_pass);
    }
    void on_button_test1(QVariant a_var)
    {
        qDebug() << "[NicoVideoWidget::on_button_test1()]" << a_var;
        QMap<QString, QVariant> v_map = a_var.toMap();
        qDebug() << v_map.keys();
        qDebug() << v_map.value("a");
        qDebug() << v_map.value("b");
        qDebug() << v_map.value("c");
        //QVariant v_a = v_map.value("a");
        //qDebug() << v_a.canConvert<int>();
        qDebug() << v_map.value("a").canConvert<int>();
        qDebug() << v_map.value("b").canConvert<int>();
        qDebug() << v_map.value("c").canConvert<int>();
        //qDebug() << qvariant_cast<int>(v_map.value("a"));
        qDebug() << v_map.value("a").value<int>();
        qDebug() << v_map.value("b").value<int>();
        qDebug() << v_map.value("c").value<int>();
        qDebug() << v_map.value("a").typeName();
        qDebug() << v_map.value("a").type();
        QVariant v_a = v_map.value("a");
        qDebug() << v_a.convert(QVariant::Int);
        qDebug() << v_a;
    }
    bool check_nicovideo_login(const QString &a_nico_mail, const QString &a_nico_pass)
    {
        QUrl v_url("https://secure.nicovideo.jp/secure/login");
        QUrlQuery v_param;
        v_param.addQueryItem("mail", a_nico_mail);
        v_param.addQueryItem("password", a_nico_pass);

        qDebug() << "[v_param.toEncoded()]" << v_param.toString();
        QByteArray v_post_data = v_param.toString().toLatin1();
        qDebug() << "[v_post_data]" << v_post_data;

        QNetworkAccessManager v_nam;

        qDebug() << "(1)";

        QNetworkRequest v_request;
        v_request.setUrl(v_url);
        v_request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
        QNetworkReply *v_reply = v_nam.post(v_request, v_post_data);
        qDebug() << "(2)";
        while(!v_reply->isFinished())
        {
            //qDebug() << "(3)";
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents, 100);
            //qDebug() << "(4)";
        }
        qDebug() << "(5)";
        qDebug() << v_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        qDebug() << v_reply->header(QNetworkRequest::LocationHeader);
        qDebug() << v_reply->error();
        qDebug() << v_reply->errorString();
        QByteArray v_result = v_reply->readAll();
        qDebug() << "[v_result]" << v_result;
        //if(v_reply->header(QNetworkRequest::LocationHeader).isValid())
        if(v_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute) == 302)
        {
            qDebug() << "[Login OK]";
            return true;
        }
        else
        {
            qDebug() << "[Login Failed]";
            return false;
        }
    }

protected slots:
    void populateJavaScriptWindowObject()
    {
        this->page()->mainFrame()->addToJavaScriptWindowObject("webview", this);
    }
};

void NicoVideoWidget_ShowModal(QWidget *a_parent)
{
    qDebug() << "[T_Web::on_button_dialog_click(begin)]";
#if 0x0
    QWidget *widget = new NicoVideoWidget(a_parent);
    widget->setAttribute(Qt::WA_DeleteOnClose);
    widget->setWindowFlags(Qt::Dialog | Qt::SubWindow);
    widget->setWindowModality(Qt::WindowModal);
    widget->show();
    while(widget->isVisible())
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
#else
    NicoVideoWidget widget(a_parent);
    //widget.setAttribute(Qt::WA_DeleteOnClose);
    widget.setWindowFlags(Qt::Dialog | Qt::SubWindow);
    widget.setWindowModality(Qt::WindowModal);
    widget.show();
    while(widget.isVisible())
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
#endif
    qDebug() << "[MasterPasswordDialog_ShowModal(end)]";
    qDebug() << "[T_Web::on_button_dialog_click(end)]";
}

#include "nicovideowidget.moc"
