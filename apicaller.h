#ifndef FORECAST_H
#define FORECAST_H
#include "baseapicaller.h"
#include <QVector>

class ApiCaller: public BaseApiCaller
{
public:
    ApiCaller();
    void sendRequest(QString) override;
    void afterRequest(QNetworkReply*) override;
    void setBaseUrl(QString url) {baseUrl = url;}
    QJsonObject getResult() {return result;}
    ~ApiCaller();
private:
    QJsonObject result;
    QNetworkAccessManager* manage;
    QString baseUrl;
};

#endif // FORECAST_H
