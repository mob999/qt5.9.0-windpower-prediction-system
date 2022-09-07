#include "apicaller.h"

ApiCaller::ApiCaller()
{
    manage = new QNetworkAccessManager;
    connect(manage, &QNetworkAccessManager::finished, this, &ApiCaller::afterRequest);
    baseUrl = "http://4f548597.cpolar.top";
}

ApiCaller::~ApiCaller()
{
    delete manage;
}

void ApiCaller::sendRequest(QString api)
{
    QNetworkRequest req;
    req.setUrl(QUrl(baseUrl+api));
    QNetworkReply* reply = manage->get(req);
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
}

void ApiCaller::afterRequest(QNetworkReply* reply)
{
     QByteArray tmp = reply->readAll();
     QJsonDocument doc = QJsonDocument::fromJson(tmp);
     result = doc.object();
}

