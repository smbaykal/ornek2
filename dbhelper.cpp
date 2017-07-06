#include "dbhelper.h"

DbHelper::DbHelper(QString driver, QString hostName, QString databaseName, QString userName, QString password, int port)
{
    db = QSqlDatabase::addDatabase(driver);
    db.setHostName(hostName);
    db.setDatabaseName(databaseName);
    db.setUserName(userName);
    db.setPassword(password);
    db.setPort(port);

    db.open();
}

DbHelper::~DbHelper()
{
    db.close();
}

QSqlDatabase DbHelper::getDb() const
{
    return db;
}

QSqlQuery DbHelper::executeQuery(QString query)
{
    return db.exec(query);
}

QSqlQuery DbHelper::getData(QString table)
{
    return db.exec("SELECT * FROM " + table);
}

QSqlQuery DbHelper::getData(QStringList tables)
{
    QString query = "SELECT * FROM ";
    foreach(QString table, tables){
        query += table;
    }
    return db.exec(query);
}

QSqlQuery DbHelper::getData(QStringList tables, QString condition)
{
    QString query = "SELECT * FROM ";
    foreach(QString table, tables){
        query += table;
    }
    query += " WHERE " + condition;
}
