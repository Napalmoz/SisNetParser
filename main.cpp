#include <QCoreApplication>
#include <QString>
#include <QtDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlField>
#include <QSqlRecord>

#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <pthread.h>
#include <ntripclient.h>
#include <time.h>
#include <stdio.h>

#include <rtklib.h>
#include <sisnetparser.h>
#include <database.h>
#include <unistd.h>

void init_arr_count(int *arr, int count)
{
    for(int i = 0; i < count; i++)
    {
        arr[i] = 0;
    }
}

void run_all_cl(NtCl *ClPool, int count)
{
    for(int i = 0; i < count; i++)
    {
        ClPool[i].Run();
    }
}

void init_arr_cl(NtCl *ClPool, QString *mnt, int count, char *s, char *pr, char *u, char *p, int md, int recon_time)
{
    for(int i = 0; i < count; i++)
    {
        ClPool[i] = *new NtCl(s, pr, u, p, mnt->toLocal8Bit().data(), md, recon_time);
        mnt++;
    }
    int a = 0;
}

void rec(int type, char *tstr)
{

    char buf[1024];
    QSqlQuery query;
    snprintf(buf, 1024, "INSERT INTO \"Data\" VALUES(%d,%d,'%s');", 5, type, tstr);
    query.prepare(buf);
    query.exec();
}



void time2str2(gtime_t t, char *s, int n)
{
    double ep[6];

    if (n<0) n=0; else if (n>12) n=12;
    if (1.0-t.sec<0.5/pow(10.0,n)) {t.time++; t.sec=0.0;};
    time2epoch(t,ep);
    sprintf(s,"%04.0f/%02.0f/%02.0f %02.0f:%02.0f:%02.0f",ep[0],ep[1],ep[2],
            ep[3],ep[4],n<=0?2:n+3,n<=0?0:n,ep[5]);
}

void make_query(sisnetmsg_t *sisnetMsg, char *out_query)
{
    gtime_t time;
    char tstr[32];
    time = gpst2time(sisnetMsg->sbsmsg.week, sisnetMsg->sbsmsg.tow);
    time2str2(time,tstr,3);

    sprintf(out_query, "INSERT INTO \"Test\" VALUES(%d,%d,'%s');", 8, sisnetMsg->type, tstr);
}

int main()
{
//Параметры подключения к БД
    QString HN = "192.168.111.12";
    QString DBN = "CarTest";
    QString UN = "postgres";
    QString P = "Tran5ma5t3r";
    DB *my_bd = new DB(HN, DBN, UN, P);
//Проверка и подключение к БД
    my_bd->connect();

    char buf[1024];
    QSqlQuery query;

//Параметры подключения для приема и парсинга sbas сообщений
    char *s  = (char*)"212.15.100.143";
    char *pr = (char*)"5000";
    char *u = (char*)"user";
    char *p = (char*)"1234";
    char *m = (char*)"sbas0";
    int  md = AUTO;
    int reconect_time = 5;

//Количество клиентов
    int ClientCount = 0;
    if(my_bd->send_query("SELECT * FROM \"Station\";"))
    {
        ClientCount = my_bd->get_row_count();
    }

    QString mount_arr[ClientCount];

    for(int k = 0; k < ClientCount; k++)
    {
        my_bd->get_cell_str(k, "mountpoint", buf);
        mount_arr[k] = (QString)buf;
        mount_arr[k] += "0";
    }

    NtCl *ClientPool = NULL;
    ClientPool = new NtCl[ClientCount];

    init_arr_cl(ClientPool, mount_arr, ClientCount, s, pr, u, p, md, reconect_time);

    int bcount_arr[ClientCount];

    init_arr_count(bcount_arr, ClientCount);

    run_all_cl(ClientPool, ClientCount);

    Buffer my_buf_struct;
    int buyte_count = -1;

    sisnetmsg_t sisnetMsg;
    gtime_t time;
    struct tm * timeinfo;
    char tstr[32];
    char ctstr[32];
    char* format = "%Y/%m/%d %H:%M:%S";

//    //my_bd->get_cell_str(3, "mountpoint", buf);
//    my_bd->get_cell_str(3, "id_rec", buf);



//    if(my_bd->send_query("SELECT * FROM \"Station\";"))
//    {
//        printf("Ok");
//    }
//    my_bd->get_row_count();
//    my_bd->get_cell_str(1, "mountpoint", buf);

//    query.exec("SELECT * FROM \"Station\";");
//    qDebug() << query.size();

//    make_query(&sisnetMsg, buf);
//    if(my_bd->rec(buf))
//    {
//        printf("Ok");
//    }

    int i = 0;

    while (true)
    {
        for(i = 0; i < ClientCount; i++)
        {
            my_buf_struct = ClientPool[i].GetBuf();
            if(bcount_arr[i] != my_buf_struct.buyte_count)
            {
                printf("%d ", i);
                //printf("%s ", my_buf_struct.buf);

                ParseSisNetMsg((unsigned char*)my_buf_struct.buf, my_buf_struct.size_msg, &sisnetMsg);
                if(sisnetMsg.sbsmsg.week != 0)
                {
                    time = gpst2time(sisnetMsg.sbsmsg.week, sisnetMsg.sbsmsg.tow);
                    time2str(time,tstr,3);
                    timeinfo = localtime( &sisnetMsg.recive_time );
                    strftime(ctstr, 80, format, timeinfo);
                    printf("%s ", tstr);
                    printf("%d ", sisnetMsg.type);
                    printf ("%s ", ctstr);


                    if(sisnetMsg.waas_crc_flag)
                        printf("%s\r\n", "OK CRC");
                    else
                        printf("%s\r\n", "ERROR CRC");
                }

            }
            bcount_arr[i] = my_buf_struct.buyte_count;
        }
    }

    return 0;
}
