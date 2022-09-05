#ifndef JSONPROCESS_H
#define JSONPROCESS_H
#include <QVector>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
class JsonProcess
{
public:
    static QVector<double> toDoubleArray(QJsonObject, QString);
    static double getValue(QJsonObject, QString);
};

#endif // JSONPROCESS_H
