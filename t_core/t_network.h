#ifndef T_NETWORK_H
#define T_NETWORK_H

#include <QtCore>
#include <QtNetwork>

class T_NetworkRequest_Parser : public QObject
{
    Q_OBJECT
protected:
    bool f_complete;
    int f_line_count;
    QNetworkAccessManager::Operation f_op;
    QNetworkRequest f_request;
public:
    explicit T_NetworkRequest_Parser(QObject *parent = 0)
        : QObject(parent)
        , f_complete(false)
        , f_line_count(0)
        , f_op(QNetworkAccessManager::UnknownOperation)
    {
    }
    virtual ~T_NetworkRequest_Parser()
    {
    }
    QNetworkAccessManager::Operation operation()
    {
        return f_op;
    }
    QNetworkRequest &request()
    {
        return f_request;
    }
    bool parse_completed()
    {
        return f_complete;
    }
    void parse_next_line(QByteArray &a_line);
};

class T_NetworkReply_Parser : public QObject
{
    Q_OBJECT
protected:
    bool f_complete;
    int f_line_count;
public:
    // ctor
    explicit T_NetworkReply_Parser(QObject *a_parent = 0);
    // dtor
    virtual ~T_NetworkReply_Parser();
    // parse_completed()
    bool parse_completed()
    {
        return f_complete;
    }
    // parse_next_line()
    void parse_next_line(QByteArray &a_line);
    // http status
    void setHttpStatus(int status, const QString &reason);
    // raw headers:
    bool hasRawHeader(const QByteArray &headerName) const;
    QList<QByteArray> rawHeaderList() const;
    QByteArray rawHeader(const QByteArray &headerName) const;
    void  setRawHeader(const QByteArray &headerName, const QByteArray &headerValue);
    // attributes
    QVariant attribute(QNetworkRequest::Attribute code) const;
    void setAttribute(QNetworkRequest::Attribute code, const QVariant &value);
public:
    QString toString()
    {
        QByteArray v_result;
        v_result.append("{T_NetworkReply_Parser: ");
        v_result.append(this->attribute(QNetworkRequest::HttpStatusCodeAttribute).toByteArray());
        v_result.append(" ");
        v_result.append(this->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray());
        v_result.append("\n");
        QList<QByteArray> v_keys = this->rawHeaderList();
        foreach(QByteArray v_key, v_keys)
        {
            v_result.append("    [");
            v_result.append(v_key);
            v_result.append("]: ");
            v_result.append(this->rawHeader(v_key));
            v_result.append("\n");
        }
        v_result.append("}");
        return QLatin1String(v_result);
    }
protected:
    QMap<QByteArray, QByteArray> f_raw_header_map;
    QMap<QNetworkRequest::Attribute, QVariant> f_attributes_map;
};

#endif // T_NETWORK_H
