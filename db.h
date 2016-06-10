#ifndef db
#define db

#include <QCoreApplication>
#include <QtDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlResult>

class DB {                                         //Объявление класса DB
    QString HostName,                              //IP адрес БД
            DatabaseName,                          //Имя БД
            UserName,                              //Имя пользователя
            Password;                              //Пароль
public:
    QSqlDatabase db;
    QSqlQuery query;

    DB(QString HN, QString DBN, QString UN, QString P) //Конструктор класса
    {
          setParam(HN, DBN, UN, P);
          db = QSqlDatabase::addDatabase("QPSQL");
    }
void setParam(QString HN, QString DBN, QString UN, QString P)  //Установка параметров
    {
        HostName = HN;
        DatabaseName = DBN;
        UserName = UN;
        Password = P;
        HN = db.setHostName;
        DBN = db.setDatabaseName;
        UN = db.setUserName;
        P = db.setPassword;
    }

bool connect()
    {
        return db.open();
        qDebug() << db.lastError().text();
    }

#endif // db

