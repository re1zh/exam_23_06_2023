#ifndef SINGLETONDB_H
#define SINGLETONDB_H
#include <QCoreApplication>
#include <QVariant>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QString>
#include <QTcpSocket>

class SingletonDB;

class DatabaseDestroyer
{
private:
    SingletonDB * p_instance;
public:
    ~DatabaseDestroyer();
    void initialize(SingletonDB * p);
};


class SingletonDB
{
private:
    static SingletonDB* p_instance;
    static DatabaseDestroyer destroyer;

    static QSqlDatabase db;

    static void openDB();
    static void createTables();
    SingletonDB() {};
    SingletonDB(const SingletonDB&) = delete;
    SingletonDB& operator = (SingletonDB&) = delete;
    ~SingletonDB() {};
    friend class DatabaseDestroyer;

public:
    static SingletonDB* getInstance();

    static void createDB();

    static void insertUser(QString login, int port);

    static void close();

    static void updateUserWins(QString login);

    static void updateUserLosses(QString login);

    static QString getUserLogin(QTcpSocket *socket);

    static void removeUser(const QString login);
};

#endif // SINGLETONDB_H
