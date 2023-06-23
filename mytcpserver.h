#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtNetwork>
#include <QByteArray>
#include <QDebug>
#include <QSet>

class MyTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit MyTcpServer(QObject *parent = nullptr);
    ~MyTcpServer();


public slots:
    void slotNewConnection();
    void slotClientDisconnected();

    void slotServerRead();
    //void slotReadClient();
private:
    QTcpServer * mTcpServer;
    QTcpSocket * mTcpSocket;

    void sendToAllClients(const QString &data);

    int server_status;

    QSet<QTcpSocket* > Users;
    QSet<QTcpSocket *> waitingUsers;
};
#endif // MYTCPSERVER_H







