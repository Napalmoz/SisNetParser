#ifndef DATABASE
#define DATABASE



class DB {                                         //Объявление класса BD
public:
    QString HostName,                              //IP адрес БД
            DatabaseName,                          //Имя БД
            UserName,                              //Имя пользователя
            Password;                              //Пароль

    const int reconnect_time = 10;                 //Время переподключения к БД(нет соединения или обрыв связи)
    QSqlDatabase db;
    QSqlQuery query;

    DB(QString HN, QString DBN, QString UN, QString P); //Конструктор класса

    //Установка параметров подключения к БД
    void setParam(QString HN, QString DBN, QString UN, QString P);

    //Выполнение подключения к БД
    void init();

    //Проверка БД на подключение
    void connect();

    //Запись в БД
    bool rec(char *query_str);

    // Отправка запроса
    bool send_query(char *query_str);

    int get_row_count();
    bool get_cell_str(int row, char *column_name, char *out_str);
};


#endif // DATABASE

