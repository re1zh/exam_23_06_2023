
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QVector>

QByteArray parsing(const QString& command, QTcpSocket* socket);

#endif // FUNCTIONS_H
