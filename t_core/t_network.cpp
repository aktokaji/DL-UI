#include "t_network.h"

static QByteArray p_cut_from_left(QByteArray &a_line, const QByteArray &a_delim)
{
    QByteArray v_result;
    int v_idx = a_line.indexOf(a_delim);
    if(v_idx == -1)
    {
        v_result = a_line.trimmed();
        a_line.clear();
        return v_result;
    }
    v_result = a_line.left(v_idx).trimmed();
    a_line = a_line.mid(v_idx+1).trimmed();
    return v_result;
}

static QNetworkAccessManager::Operation p_op_for_name(const QByteArray &a_op_name)
{
    if(a_op_name.toUpper()=="HEAD") return QNetworkAccessManager::HeadOperation;
    if(a_op_name.toUpper()=="GET") return QNetworkAccessManager::GetOperation;
    if(a_op_name.toUpper()=="PUT") return QNetworkAccessManager::PutOperation;
    if(a_op_name.toUpper()=="POST") return QNetworkAccessManager::PostOperation;
    if(a_op_name.toUpper()=="DELETE") return QNetworkAccessManager::DeleteOperation;
    return QNetworkAccessManager::UnknownOperation;
}

void T_NetworkRequest_Parser::parse_next_line(QByteArray &a_line)
{
    qDebug() << "[T_NetworkRequest_Parser::parse_next_line()]" << f_line_count << a_line;
    Q_ASSERT(!f_complete);
    if(a_line.trimmed().isEmpty())
    {
        f_complete = true;
        return;
    }
    if(f_line_count == 0)
    {
        QByteArray v1 = p_cut_from_left(a_line, " ");
        QByteArray v2 = p_cut_from_left(a_line, " ");
        //Q_ASSERT(v1=="GET");
        f_op = p_op_for_name(v1);
        QUrl v_url = QUrl::fromEncoded(v2);
        f_request.setUrl(v_url);
    }
    else
    {
        QByteArray v1 = p_cut_from_left(a_line, ":");
        f_request.setRawHeader(v1, a_line);
    }
    f_line_count++;
}



// ctor
T_NetworkReply_Parser::T_NetworkReply_Parser(QObject *a_parent)
    : QObject(a_parent)
    , f_complete(false)
    , f_line_count(0)
{
    this->setAttribute(QNetworkRequest::ConnectionEncryptedAttribute, false);
    this->setAttribute(QNetworkRequest::SourceIsFromCacheAttribute, false);
    this->setAttribute(QNetworkRequest::HttpPipeliningWasUsedAttribute, false);
}

// dtor
T_NetworkReply_Parser::~T_NetworkReply_Parser()
{
}

// parse_next_line()
void T_NetworkReply_Parser::parse_next_line(QByteArray &a_line)
{
    qDebug() << "[T_NetworkRequest_Parser::parse_next_line()]" << f_line_count << a_line;
    Q_ASSERT(!f_complete);
    if(a_line.trimmed().isEmpty())
    {
        f_complete = true;
        return;
    }
    if(f_line_count == 0)
    {
        QByteArray v1 = p_cut_from_left(a_line, " ");
        QByteArray v2 = p_cut_from_left(a_line, " ");
        this->setHttpStatus(v2.toInt(), QLatin1String(a_line));
    }
    else
    {
        QByteArray v1 = p_cut_from_left(a_line, ":");
        this->setRawHeader(v1, a_line);
    }
    f_line_count++;
}

// http status
void T_NetworkReply_Parser::setHttpStatus(int status, const QString &reason)
{
    qDebug() << "[T_HttpAccessAttribute::setHttpStatus()]" << status << reason;
    this->setAttribute(QNetworkRequest::HttpStatusCodeAttribute, status);
    this->setAttribute(QNetworkRequest::HttpReasonPhraseAttribute, reason);
}

// raw headers:
bool T_NetworkReply_Parser::hasRawHeader(const QByteArray &headerName) const
{
    return f_raw_header_map.contains(headerName);
}
QList<QByteArray> T_NetworkReply_Parser::rawHeaderList() const
{
    return f_raw_header_map.keys();
}
QByteArray T_NetworkReply_Parser::rawHeader(const QByteArray &headerName) const
{
    if(!f_raw_header_map.contains(headerName)) return QByteArray();
    return f_raw_header_map.value(headerName);
}
void  T_NetworkReply_Parser::setRawHeader
        (const QByteArray &headerName, const QByteArray &headerValue)
{
    f_raw_header_map.insert(headerName, headerValue);
}

// attributes
QVariant T_NetworkReply_Parser::attribute(QNetworkRequest::Attribute code) const
{
    if(code == QNetworkRequest::RedirectionTargetAttribute)
    {
        int status = this->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        switch (status) {
        case 301:				   // Moved Permanently
        case 302:				   // Found
        case 303:				   // See Other
        case 307:				   // Temporary Redirect
            {
                QByteArray header = this->rawHeader("location");
                QUrl url = QUrl::fromEncoded(header);
                if (!url.isValid()) url = QUrl(QLatin1String(header));
                return url;
            }
        default:
            return QVariant();
        }
    }
    if(!f_attributes_map.contains(code)) return QVariant();
    return f_attributes_map.value(code);
}
void T_NetworkReply_Parser::setAttribute
        (QNetworkRequest::Attribute code, const QVariant &value)
{
    f_attributes_map.insert(code, value);
}
