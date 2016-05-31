#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <pthread.h>
#include <ntripclient.h>
#include <rtklib.h>
#include <sisnetparser.h>
#include <time.h>
#include <stdio.h>

int main()
{
    char *s  = (char*)"212.15.100.143";
    char *pr = (char*)"5000";
    char *u = (char*)"user";
    char *p = (char*)"1234";
    char *m = (char*)"sbas0";
    int  md = AUTO;
    int reconect_time = 5;


    NtCl *client = new NtCl(s, pr, u, p, m, md, reconect_time);
    client->Run();

    Buffer my_buf_struct;
    int buyte_count = -1;

    sisnetmsg_t sisnetMsg;
    gtime_t time;
    struct tm * timeinfo;
    char tstr[32];


//    unsigned char *msg = (unsigned char*)"*MSG,1898,122152,531000140700440180000000000000B40E4084000003BBBBBBBBBBBBAFCB6800*E4\r\n";
//    //unsigned char *msg = (unsigned char*)"*MSG,1898,122152,531000000000000000000000000000000000000000000000000000002FCB6800*E4\r\n";
//    int count = 86;
//    ParseSisNetMsg(msg, count, &sisnetMsg);

    while(true)
    {
        my_buf_struct = client->GetBuf();
        if(buyte_count != my_buf_struct.buyte_count)
        {
            ParseSisNetMsg((unsigned char*)my_buf_struct.buf, my_buf_struct.size_msg, &sisnetMsg);
            if(sisnetMsg.sbsmsg.week != 0)
            {
                time = gpst2time(sisnetMsg.sbsmsg.week, sisnetMsg.sbsmsg.tow);
                time2str(time,tstr,3);
                timeinfo = localtime( &sisnetMsg.recive_time );
                printf("%s ", tstr);
                printf("%d ", sisnetMsg.type);
                printf ("%s ", asctime (timeinfo));
                if(sisnetMsg.waas_crc_flag)
//                if(sisnetMsg.is_msg_null)
                    printf("%s\r\n", "OK CRC");
                else
                    printf("%s\r\n", "ERROR CRC");
            }
            buyte_count = my_buf_struct.buyte_count;
        }
    }
    return 0;
}
