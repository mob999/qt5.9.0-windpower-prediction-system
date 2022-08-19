#include "jsonprocess.h"

QVector<double> JsonProcess::toDoubleArray(QJsonObject obj, QString key)
{
    QStringList tmp = obj[key].toString().split('[')[1].split(']')[0].split(',');
    QVector<double> res;
    for(QString& x : tmp) {
        res.append(x.toDouble());
    }
    return res;
}
