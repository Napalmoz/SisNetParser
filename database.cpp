#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlResult>
#include <QString>
#include <QtDebug>
#include <unistd.h>
#include <database.h>


DB::DB(QString HN, QString DBN, QString UN, QString P) //Конструктор класса
{

    init();
    setParam(HN, DBN, UN, P);
}

    //Установка параметров подключения к БД
    void DB::setParam(QString HN, QString DBN, QString UN, QString P)
    {
               HostName = HN;
               DatabaseName = DBN;
               UserName = UN;
               Password = P;
               db.setHostName(HN);
               db.setDatabaseName(DBN);
               db.setUserName(UN);
               db.setPassword(P);
    }

    //Выполнение инициализации БД
    void DB::init()
    {
        db = QSqlDatabase::addDatabase("QPSQL");
        query = db.exec();
    }

    //Выполнение подключения к БД
    void DB::connect()
    {
        while(!db.open())
        {
            sleep(reconnect_time);
            qDebug() << db.lastError().text();
        }
    }

//    //Запись в БД
    bool DB::rec(char *query_str)
    {
        query.prepare(query_str);
        return query.exec();
    }

// Отправка запроса
    bool DB::send_query(char *query_str)
    {
        query.prepare(query_str);
        return query.exec();
    }

    //Получение кол-ва записей в ответе
    int DB::get_row_count()
    {
        return query.size();
    }

    //получение ячейки из ответа БД
    bool DB::get_cell_str(int row, char *column_name, char *out_str)
    {
        if((row > -1) && (row < get_row_count()))
        {
            query.seek(row);
            QString str = query.value(column_name).toString();
            int k = query.value(column_name).toInt();
            sprintf(out_str,"%s",(const char *)str.toStdString().c_str());
            return true;
        }
        return false;
    }



