#ifndef BASEAPICALLER_H
#define BASEAPICALLER_H
#include <QObject>
#include <QDebug>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMetaObject>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

class BaseApiCaller: public QObject
{
    Q_OBJECT
public:
    virtual void sendRequest(QString) = 0;
    virtual void afterRequest(QNetworkReply*) = 0;
};

#endif // BASEAPICALLER_H
