#include "mytcpserver.h"
#include "singletondb.h"

#include <QDebug>
#include <QCoreApplication>
#include <QVariant>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlDatabase>

MyTcpServer::~MyTcpServer()
{
    //mTcpSocket->close();
    mTcpServer->close();
    server_status=0;
}
MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent){
    mTcpServer = new QTcpServer(this);
    connect(mTcpServer, &QTcpServer::newConnection,
            this, &MyTcpServer::slotNewConnection);

    if(!mTcpServer->listen(QHostAddress::Any, 33333)){
        qDebug() << "server is not started";
    } else {
        server_status=1;
        qDebug() << "server is started";
    }
}

void MyTcpServer::slotNewConnection(){
    if(server_status==1){
        QTcpSocket* TcpSocket;

        TcpSocket = mTcpServer->nextPendingConnection();
        qDebug() << "New user connected:" << TcpSocket->peerAddress().toString() << ":" << TcpSocket->peerPort();

        TcpSocket->write("connected!\r\n");
        connect(TcpSocket, &QTcpSocket::readyRead,
                this,&MyTcpServer::slotServerRead);
        connect(TcpSocket,&QTcpSocket::disconnected,
                this,&MyTcpServer::slotClientDisconnected);
        Users.insert(TcpSocket);
    }
}

void MyTcpServer::slotServerRead(){
    QTcpSocket* TcpSocket = (QTcpSocket*)sender();
    QByteArray data = TcpSocket->readAll();
    while(TcpSocket->bytesAvailable() > 0)
    {
        data.append(TcpSocket->readAll());
    }
    if(data.right(1) == "\n")
    {
        qDebug() << "Received data from" << TcpSocket->peerAddress().toString() << ":" << TcpSocket->peerPort() << ":" << data;

        if (data.startsWith("start&")) {
            QString login = data.mid(data.indexOf('&') + 1);

            if (waitingUsers.size() < 5) {
                waitingUsers.insert(TcpSocket);
                SingletonDB::insertUser(login, TcpSocket->peerPort());
                TcpSocket->write("waiting\r\n");

                if (waitingUsers.size() == 5) {
                    sendToAllClients("make_move\r\n");
                }
            }
        }
        else if (data.startsWith("break")) {
            waitingUsers.remove(TcpSocket);
            QString login = SingletonDB::getUserLogin(TcpSocket);
            SingletonDB::removeUser(login);
            TcpSocket->disconnectFromHost();
        }
        else if (data.startsWith("stats")) {
            QString stats;
            QSet<QTcpSocket *>::iterator it;
            for (it = waitingUsers.begin(); it != waitingUsers.end(); ++it) {
                stats += SingletonDB::getUserLogin(*it) + "\r\n";
            }
            TcpSocket->write(stats.toUtf8());
        }
        else if (data.startsWith("choice&")) {
            int maxNumber = 0;
            QSet<QTcpSocket *>::iterator maxSocketIt = waitingUsers.end();

            QSet<QTcpSocket *>::iterator it;
            for (it = waitingUsers.begin(); it != waitingUsers.end(); ++it) {
                int number = data.mid(data.indexOf('&') + 1).toInt();
                if (number > maxNumber) {
                    maxNumber = number;
                    maxSocketIt = it;
                }
            }

            for (it = waitingUsers.begin(); it != waitingUsers.end(); ++it) {
                if (it == maxSocketIt) {
                    (*it)->write("win\n");
                } else {
                    (*it)->write("lost\n");
                }
                QString login = SingletonDB::getUserLogin(*it);
                SingletonDB::removeUser(login);
                (*it)->disconnectFromHost();
            }

            waitingUsers.clear();
        }
    }
}

void MyTcpServer::slotClientDisconnected(){
    QTcpSocket* TcpSocket = (QTcpSocket*)sender();
    Users.remove(TcpSocket);
    waitingUsers.remove(TcpSocket);
}

void MyTcpServer::sendToAllClients(const QString &data)
{
    QSet<QTcpSocket *>::iterator it;
    for (it = Users.begin(); it != Users.end(); ++it) {
        (*it)->write(data.toUtf8());
    }
}
