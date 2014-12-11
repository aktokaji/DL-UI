#include "dlapp.h"

#include "dluicommon.h"
//#include <cryptopp/aes.h>
//#include <cryptopp/dh.h>
//#include <cryptopp/modes.h>

DLApp::DLApp(int &argc, char **argv, int flags) :
    QApplication(argc, argv, flags)
{
}

DLApp *DLApp_Get()
{
    return reinterpret_cast<DLApp *>(qApp);
}

static QUuid GenMd5UuidForBytes(const QByteArray &a_bytes)
{
    QCryptographicHash v_md5_gen(QCryptographicHash::Md5);
    v_md5_gen.addData(a_bytes);
    QByteArray v_md5 = v_md5_gen.result();
    Q_ASSERT(v_md5.length()==CryptoPP::AES::BLOCKSIZE);
    QUuid v_iv_uuid = QUuid::fromRfc4122(v_md5);
    return v_iv_uuid;
}

static QUuid GenMd5UuidForString(const QString &a_str)
{
    QByteArray v_bytes = a_str.toUtf8();
    return GenMd5UuidForBytes(v_bytes);
}

bool DLApp::MasterPasswordIsSet()
{
    QSettings v_settings( // C:\Users\root\AppData\Roaming\akio.tokaji\yt_dlui.ini
                          QSettings::IniFormat, //QSettings::NativeFormat,
                          QSettings::UserScope, // QSettings::SystemScope,
                          QCoreApplication::organizationName(),
                          QCoreApplication::applicationName()
                          );
    if(v_settings.contains("global/master-password-encoded")) return true;
    return false;
}

QByteArray DLApp::EncodeMasterPassword(const QString &a_password, const QString &a_text)
{
#if 0x1
    return QByteArray();
#else
    // Key
    QUuid v_key_uuid = QUuid(DLUI_AES_COMMON_KEY_UUID);
    QByteArray v_key = v_key_uuid.toRfc4122();
    Q_ASSERT(v_key.length()==CryptoPP::AES::DEFAULT_KEYLENGTH);
    QByteArray v_key_exp = v_key_uuid.toString().toUtf8();
    qDebug() << "[v_key]" << v_key_exp;

    // IV
    QUuid v_iv_uuid = GenMd5UuidForString(a_password);
    QByteArray v_iv = v_iv_uuid.toRfc4122();
    Q_ASSERT(v_iv.length()==CryptoPP::AES::BLOCKSIZE);
    QByteArray v_iv_exp = v_iv_uuid.toString().toUtf8();
    qDebug() << "[v_iv]" << v_iv_exp;

    QByteArray v_textBytes = a_text.toUtf8();
    v_textBytes.prepend(v_iv_exp);
    v_textBytes.prepend(v_key_exp);
    std::string v_textStdStr(v_textBytes.constData(), v_textBytes.length());

    // 暗号化オブジェクトの作成
    CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption enc;
    enc.SetKeyWithIV(reinterpret_cast<const byte*>(v_key.constData()), v_key.length(), (const byte*)v_iv.constData());

    // 暗号化のための変換フィルタの作成
    std::string encText;
    CryptoPP::StreamTransformationFilter encFilter(enc, new CryptoPP::StringSink(encText));

    // 暗号化
    encFilter.Put(reinterpret_cast<const byte*>(v_textStdStr.c_str()), v_textStdStr.size());
    encFilter.MessageEnd();

    //qDebug() << "Encrypted Text(1) : " << QString::fromStdString(encText);
    QByteArray encBytes(encText.c_str(), encText.length());
    //qDebug() << "Encrypted Text(2) : " << encBytes.toHex();
    qDebug() << "Encrypted Text(3) : " << encBytes.toBase64();

    return encBytes;
#endif
}

QString DLApp::DecodeMasterPassword(const QString &a_password, const QByteArray a_encBytes)
{
#if 0x1
    return "";
#else
    // Key
    QUuid v_key_uuid = QUuid(DLUI_AES_COMMON_KEY_UUID);
    QByteArray v_key = v_key_uuid.toRfc4122();
    Q_ASSERT(v_key.length()==CryptoPP::AES::DEFAULT_KEYLENGTH);
    QByteArray v_key_exp = v_key_uuid.toString().toUtf8();
    qDebug() << "[v_key]" << v_key_exp;

    // IV
    QUuid v_iv_uuid = GenMd5UuidForString(a_password);
    QByteArray v_iv = v_iv_uuid.toRfc4122();
    Q_ASSERT(v_iv.length()==CryptoPP::AES::BLOCKSIZE);
    QByteArray v_iv_exp = v_iv_uuid.toString().toUtf8();
    qDebug() << "[v_iv]" << v_iv_exp;

    //
    std::string encText(a_encBytes.constData(), a_encBytes.length());

    // 復号化オブジェクトの作成
    CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption dec;
    dec.SetKeyWithIV(reinterpret_cast<const byte*>(v_key.constData()), v_key.length(), reinterpret_cast<const byte*>(v_iv.constData()));

    // 復号化のための変換フィルタの作成
    std::string decText;
    CryptoPP::StreamTransformationFilter decFilter(dec, new CryptoPP::StringSink(decText));
    decFilter.Put(reinterpret_cast<const byte*>(encText.c_str()), encText.size());
    decFilter.MessageEnd();

    QByteArray v_decBytes(decText.c_str(), decText.size());
    if(v_decBytes.startsWith(v_key_exp+v_iv_exp))
    {
        v_decBytes.remove(0, v_key_exp.length()+v_iv_exp.length());
        return QString::fromUtf8(v_decBytes);
    }
    else
    {
        return "";
    }
#endif
}
