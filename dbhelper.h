#ifndef DBHELPER_H
#define DBHELPER_H

#include <QtSql>


class DbHelper : public QObject
{
    Q_OBJECT
public:
    DbHelper(QString driver, QString hostName, QString databaseName, QString userName, QString password, int port = 5432);
    ~DbHelper();

    QSqlDatabase getDb() const;
    QSqlQuery executeQuery(QString query);
    QSqlQuery getData(QString table);
    QSqlQuery getData(QStringList tables);
    QSqlQuery getData(QStringList tables, QString condition);

private:
    QSqlDatabase db;
};

#endif // DBHELPER_H
