#include "singletondb.h"

DatabaseDestroyer::~DatabaseDestroyer(){delete p_instance;}
void DatabaseDestroyer::initialize(SingletonDB * p) {p_instance = p;}

void SingletonDB::openDB(){
    if (db.isOpen()) {
        return;
    }

    qDebug()<<"MyDB()\n";
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("MATH");

    if(!db.open())
        qDebug()<<db.lastError().text();
}


void SingletonDB::close(){
    if(db.isOpen())
        db.close();
}


void SingletonDB::createDB(){
    SingletonDB::getInstance();
    openDB();
    createTables();
}


SingletonDB* SingletonDB::getInstance() {
    if (!p_instance)
    {
        p_instance = new SingletonDB();
        destroyer.initialize(p_instance);
    }
    return p_instance;
}


void SingletonDB::createTables(){
    QSqlQuery query(db);


    query.exec("CREATE TABLE IF NOT EXISTS data_user("
               "login VARCHAR(30) NOT NULL,"
               "port INTEGER NOT NULL,"
               "count_win INTEGER NOT NULL,"
               "count_lose INTEGER NOT NULL"
               ")");
}

void SingletonDB::insertUser(QString login, int port){
    QSqlQuery query(db);
    query.prepare("INSERT INTO data_user "
                  "VALUES (:login, :port, 0, 0)");
    query.bindValue(":login", login);
    query.bindValue(":port", port);

    if(!query.exec())
        qDebug()<<query.lastError().text();
}

void SingletonDB::updateUserLosses(const QString login)
{
    QSqlQuery query;
    query.prepare("UPDATE clients SET count_lose = count_lose + 1 WHERE login = :login");
    query.bindValue(":login", login);

    if (!query.exec()) {
       qDebug()<<query.lastError().text();
    }
}

void SingletonDB::updateUserWins(const QString login)
{
    QSqlQuery query;
    query.prepare("UPDATE clients SET count_win = count_win + 1 WHERE login = :login");
    query.bindValue(":login", login);

    if (!query.exec()) {
       qDebug()<<query.lastError().text();
    }
}

QString SingletonDB::getUserLogin(QTcpSocket *socket){
    QSqlQuery query;
    query.prepare("SELECT login FROM data_user WHERE port = :port");
    query.bindValue(":port", socket->socketDescriptor());

    if (query.exec() && query.next()) {
       return query.value(0).toString();
    }
    return "";
}

void SingletonDB::removeUser(QString login)
{
    QSqlQuery query;
    query.prepare("DELETE FROM data_user WHERE login = :login");
    query.bindValue(":login", login);

    if (!query.exec()) {
       qDebug()<<query.lastError().text();
    }
}

QSqlDatabase SingletonDB::db;
SingletonDB* SingletonDB::p_instance;
DatabaseDestroyer SingletonDB::destroyer;
