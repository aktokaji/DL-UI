#include <QtCore>
#include <QtGui>
#include <QtWebKit>
#include <QtWebKitWidgets>
#include <QtSql>

#include "dluicommon.h"
#include "dlapp.h"

#include "moduledb.h"
#include "dlprogress.h"

#include "nicovideowidget.h"
#include "masterpassworddialog.h"

#include "crc32.h"

static QString Crc32ToHex(quint32 value)
{
    QString s;
    s.sprintf("%08X", value);
    return s;
}

static quint32 GetMacId(ModuleDB *a_moduel_db)
{
    QProcess v_proc;
    QString v_exe = a_moduel_db->getPcizPath();
    QStringList v_args;
    v_args << "/list";
    v_proc.start(v_exe, v_args);
    v_proc.waitForFinished(-1);
    QByteArray v_bytes = v_proc.readAll();
    qDebug() << "[v_bytes]" << v_bytes;
    QBuffer v_buffer(&v_bytes);
    v_buffer.open(QIODevice::ReadOnly);
    QByteArray v_line = v_buffer.readLine();
    v_line = v_line.trimmed();
    qDebug() << "[v_line](1)" << v_line;
    if(!v_line.startsWith("CPU name: "))
    {
        return 0;
    }
    v_line.replace("CPU name: ", "");
    qDebug() << "[v_line](2)" << v_line;
    QString v_line_str = QString::fromLatin1(v_line);
    qDebug() << "[v_line_str](1)" << v_line_str;
    QRegExp rx("[0-9.]+[ ]*[KMG]?Hz");
    v_line_str.replace(rx, "?Hz");
    qDebug() << "[v_line_str](2)" << v_line_str;
    CRC32 v_crc32_engine;
    quint32 v_crc = v_crc32_engine.crc32(0, v_line_str);
    quint32 v_mac_id = v_crc % 1000000;
    qDebug() << Crc32ToHex(v_crc) << v_mac_id;
    return v_mac_id;
}

static quint32 MacIdToPasscode(quint32 a_mac_id)
{
    CRC32 v_crc32_engine;
    QString v_mac_id_str = QString::number(a_mac_id, 10);
#if 0x1
    quint32 v_mac_id = v_mac_id_str.toULongLong();
    qDebug() << "[MacIdToPasscode()]" << v_mac_id;
#endif
    quint32 v_passcode_crc = v_crc32_engine.crc32(0, v_mac_id_str);
    quint32 v_passcode = v_passcode_crc % 1000000;
    return v_passcode;
}

static void Crc32Test2()
{
#if 0x0
    CRC32 v_crc32_engine;
    QFile v_file("C:/Users/root/Desktop/finfo_v_246_.exe");
    v_file.open(QIODevice::ReadOnly);
    QByteArray v_bytes = v_file.readAll();
    qDebug() << "[v_bytes.size()]" << v_bytes.size();
    v_file.close();
    quint32 v_crc32 = v_crc32_engine.crc32(0, v_bytes);
    qDebug() << "[v_crc32](A)" << Crc32ToHex(v_crc32);

    v_file.open(QIODevice::ReadOnly);
    v_crc32 = 0;
    while(v_file.bytesAvailable())
    {
        QByteArray v_bytes = v_file.read(1024);
        v_crc32 = v_crc32_engine.crc32(v_crc32, v_bytes);
    }
    v_file.close();
    qDebug() << "[v_crc32](B)" << Crc32ToHex(v_crc32);
    QString v_str = "this is test. これはテスト。";
    v_crc32 = v_crc32_engine.crc32(v_crc32, v_str);
    qDebug() << "[v_crc32](C)" << QString::number(v_crc32,16).toUpper();
    qDebug() << "[v_crc32](D)" << Crc32ToHex(v_crc32);
    for(int i=0; i<100; i++)
    {
        QUuid v_uuid = QUuid::createUuid();
        QByteArray v_uuid_bytes = v_uuid.toRfc4122();
        quint32 v_uuid_crc = v_crc32_engine.crc32(0, v_uuid_bytes);
        qDebug() << i+1 << v_uuid.toString() << Crc32ToHex(v_uuid_crc) << v_uuid_crc % 1000000;
    }
#endif
}

static void MachineIdTest(ModuleDB *a_moduel_db)
{
    quint32 v_mac_id = GetMacId(a_moduel_db);
    qDebug() << MacIdToPasscode(v_mac_id);
}

static void SettingTest()
{
    QStringList v_args;
    v_args << "/select,";
    v_args << "C:\\Users\\root\\AppData\\Roaming\\akio.tokaji\\yt_dlui.ini";
    QProcess::startDetached("explorer.exe", v_args);
}

// http://qt-project.org/doc/qt-4.8/webkit-simplewebplugin.html
class CSVFactory : public QWebPluginFactory
 {
     Q_OBJECT

 public:
     CSVFactory(ModuleDB *a_moduel_db, QObject *parent = 0):
         QWebPluginFactory(parent),
         f_module_db(a_moduel_db)
     {
     }
     QObject *create(const QString &mimeType, const QUrl &url,
                     const QStringList &argumentNames,
                     const QStringList &argumentValues) const
     {
         qDebug() << "[CSVFactory::create()]" << mimeType;
         qDebug() << "[CSVFactory::create()]" << url;
         qDebug() << "[CSVFactory::create()] argumentNames" << argumentNames;
         qDebug() << "[CSVFactory::create()] argumentValues" << argumentValues;
         DLProgress *v_widget = new DLProgress(f_module_db, url);
         return v_widget;
     }
     QList<QWebPluginFactory::Plugin> plugins() const
     {
         return QList<QWebPluginFactory::Plugin>();
     }
protected:
     ModuleDB *f_module_db;
 };

class T_Web : public QWebView
{
    Q_OBJECT
public:
    explicit T_Web()
    {
        this->setWindowTitle("VideoDownload Shield");
        connect(this->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
                this, SLOT(populateJavaScriptWindowObject()));
        //MasterPasswordDialog_ShowModal(this);
    }
    virtual ~T_Web()
    {
    }
protected slots:
    void populateJavaScriptWindowObject()
    {
        this->page()->mainFrame()->addToJavaScriptWindowObject("webview", this);
    }
public slots:
    int add2(int x, int y)
    {
        qDebug() << "[x, y]" << x << y;
        return x+y;
    }
    QString refine_watch_url(QString a_watch_url)
    {
        QUrl v_url(a_watch_url);
        if(v_url.host()=="youtube.com" || v_url.host().endsWith("youtube.com"))
        {
            if(v_url.path()=="/watch")
            {
                QUrlQuery v_url_query(v_url);
                QString v_video_id = v_url_query.queryItemValue("v");
                return QString("https://www.youtube.com/watch?v=%1").arg(v_video_id);
            }
        }
        return a_watch_url;
    }
    void set_trial_flag(bool a_flag)
    {
        qDebug() << "[T_Web::set_trial_flag()]" << a_flag;
        f_module_db.f_trial_flag = a_flag;
    }
    void on_button_dialog_click()
    {
        NicoVideoWidget_ShowModal(this);
    }
    void on_button_1_click()
    {
        //CryptppTest();
        //SettingTest();
        //LoginTest();
        Crc32Test2();
        MachineIdTest(&f_module_db);
    }
public:
    Q_PROPERTY(QString watch_url   MEMBER watch_url)   QString watch_url;
    ModuleDB f_module_db;
};

T_Web *g_webview = NULL;

int main(int argc, char *argv[])
{
    ////QApplication app(argc, argv);
    DLApp app(argc, argv);

    QCoreApplication::setOrganizationDomain("akio.tokaji");
    QCoreApplication::setOrganizationName("akio.tokaji");
    QCoreApplication::setApplicationName("yt_dlui");

    QWebSettings *webSettings = QWebSettings::globalSettings();
    webSettings->setAttribute(QWebSettings::JavascriptEnabled, true);
    webSettings->setAttribute(QWebSettings::PluginsEnabled, true);
    //webSettings->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);

    QStringList v_movie_dir_list = QStandardPaths::standardLocations(QStandardPaths::MoviesLocation);
    qDebug() << "[v_movie_dir_list]" << v_movie_dir_list;
    QDir::setCurrent(v_movie_dir_list.at(0));

    //ModuleDB g_module_db;

    g_webview = new T_Web();
    CSVFactory *factory = new CSVFactory(&g_webview->f_module_db);
    g_webview->page()->setPluginFactory(factory);

    g_webview->setObjectName("Dummy Name");
    g_webview->resize(800, 600);
    g_webview->show();
    //v_webview->watch_url = "https://www.youtube.com/watch?v=Ibua-qDVkOY";
    g_webview->watch_url = "https://www.youtube.com/watch?v=60oXx_kXEOo";
    g_webview->setUrl(QUrl("qrc:/page/page1.html"));

    MasterPasswordDialog_ShowModal(g_webview);

    return app.exec();
}

#include "main.moc"
