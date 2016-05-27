#include <rtklib.h>
#include <sisnetparser.h>

/*
 * проверка наличия нулевого сообщения
 */
bool CheckSisNetMsgIsNull()
{
    //
}

/*
 * Получения время приема сообщения
 */

gtime_t GetReciveTime()
{
    //
}

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

    char *p= (char*)raw->buff+5;

    sscanf(p,"%d,%d,",&raw->sbsmsg.week,&raw->sbsmsg.tow);

    p = strrchr(p, ',')+1;

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
    sisnetmsg->waas_crc_flag = false;
    sisnetmsg->type = -1;

    //взять системное время записать в sbsmsg->recive_time;

    //проверяем длину сообщ. Sisnet
    if(n < 80)
    {
        return 0;
    }
    sisnetmsg->waas_crc = GetCRCWAAS(msg);

    raw_t *raw = new raw_t;
    memcpy(raw->buff,msg,n);
    raw->nbyte = n;
    if(!input_sisnet(raw, raw->buff[n-2]))
        return 0;
    sisnetmsg->sbsmsg = raw->sbsmsg;
    if(sisnetmsg->sbsmsg.week == 0)
        return 0;
    sisnetmsg->waas_crc_flag = CheckCRCWAAS(&sisnetmsg->sbsmsg, sisnetmsg->waas_crc);
    sisnetmsg->type = getbitu(sisnetmsg->sbsmsg.msg ,8,6);
    return 1;
}
