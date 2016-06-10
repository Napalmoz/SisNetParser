#include <rtklib.h>
#include <sisnetparser.h>
#include <time.h>
#include <stdio.h>

#include <iostream>
#include <ctime>

using namespace std;

/*
 * проверка наличия нулевого сообщения
 */
bool CheckSisNetMsgIsNull(unsigned char *msg)
{
    char ch = msg[1];
    ch = msg[1]&0x02;
    if((msg[1]&0x02) == 0)
    {
        for(int i=2; i<29; i++)
            if(msg[i] != 0)
                return false;
    }
    return true;
}

/*
 * Получения время приема сообщения
 */

time_t GetReciveTime()
{
    char buffer[80];
    time_t seconds = time(NULL);
    tm* timeinfo = localtime(&seconds);
    char* format = "%Y/%m/%d %H:%M:%S";
    strftime(buffer, 80, format, timeinfo);
<<<<<<< HEAD
    return seconds;
=======
    cout<<"Current Datetime: "<<buffer<<endl;
    cin.get();
    return 0;
>>>>>>> feeb92804102c997de95d1a9c8f396da0554a3f7
}

//time_t GetReciveTime()
//{
//    time_t rawtime;
//    struct tm * timeinfo;
//    time ( &rawtime);
//    timeinfo = localtime( &rawtime );
//    return rawtime;
//}

unsigned int GetCRCWAAS(unsigned char *msg)
{
    unsigned int b;
    char *p= (char*)msg+73;
    sscanf(p,"%7x", &b);
    return ((b>>2)&0xFFFFFF);
}

bool CheckCRCWAAS(sbsmsg_t *sbsmsg, unsigned int crc)
{
    unsigned char f[29];
    int i;

    for (i=28;i>0;i--) f[i]=(sbsmsg->msg[i]>>6)+(sbsmsg->msg[i-1]<<2);
    f[0]=sbsmsg->msg[0]>>6;

    return crc24q(f,29)==crc;
}

/* decode sisnet ------------------------------------------------------*/
static int decode_sisnet(raw_t *raw)
{
    int i=0;
    unsigned int b;

    try
    {
        char *p= (char*)raw->buff+5;

        sscanf(p,"%d,%d,",&raw->sbsmsg.week,&raw->sbsmsg.tow);

        uint p_save = (uint)p;

        p = strrchr(p, ',')+1;
        uint p_now = (uint)p;
        if(((p_save + (4096-5)) < p_now) || (p_save > p_now))
            return 0;

        for (i=0;i<29;i++)
        {
            b=0;
            sscanf(p,"%2x", &b);
            p+=2;
            raw->sbsmsg.msg[i] = b;
        }

        raw->sbsmsg.msg[28]&=0xC0;

        raw->sbsmsg.prn=125;	/*SISNET PRN*/

        return 3;
    }
    catch(...)
    {
        return 0;
    }
}

int input_sisnet(raw_t *raw, unsigned char data)
{
    int len;

    if (raw->nbyte==0) {
        if (data!='*') return 0;
    }

    raw->buff[raw->nbyte++]=data;

    if( data=='\r' )
    {
        len = raw->nbyte;
        raw->buff[raw->nbyte++]='\0';
        raw->nbyte = 0;
        if(len>50)
            return decode_sisnet(raw);

        return 0;
    }

    return 0;
}

/* Раскодировка Сиснте сообщения ---------------------------------------------------------
* args   : unsigned char *msg       I   строка Сиснет сообщения
*          int n                    I   длина строки Сиснет сообщения
*          sisnetmsg_t *sbsmsg      O   структура сиснет соощения
* return : status (1:ok,0:error) - ошибки при раскодировании
*-----------------------------------------------------------------------------*/
unsigned int ParseSisNetMsg(unsigned char *msg, int n, sisnetmsg_t *sisnetmsg)
{
    //иниц. структуру
    sisnetmsg->recive_time = GetReciveTime();       //взять системное время записать в sisnetmsg->recive_time;
    sisnetmsg->waas_crc_flag = false;
    sisnetmsg->type = -1;

    //проверяем длину сообщ. Sisnet
    if(n < 80)
    {
        return 0;
    }
    sisnetmsg->waas_crc = GetCRCWAAS(msg);
    raw_t raw;

//    raw_t *raw = new raw_t;
    memcpy(raw.buff,msg,n);
    raw.nbyte = n;
    if(!input_sisnet(&raw, raw.buff[n-2]))
        return 0;
    sisnetmsg->sbsmsg = raw.sbsmsg;
    if(sisnetmsg->sbsmsg.week == 0)
        return 0;
    sisnetmsg->waas_crc_flag = CheckCRCWAAS(&sisnetmsg->sbsmsg, sisnetmsg->waas_crc);
    sisnetmsg->type = getbitu(sisnetmsg->sbsmsg.msg ,8,6);
    sisnetmsg->is_msg_null = CheckSisNetMsgIsNull(sisnetmsg->sbsmsg.msg);
    return 1;
}
