#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <pthread.h>
#include <ntripclient.h>
#include <rtklib.h>
#include <sisnetparser.h>

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
    char tstr[32];
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
                printf("%s ", tstr);
                printf("%d ", sisnetMsg.type);
                if(sisnetMsg.waas_crc_flag)
                    printf("%s\r\n", "OK CRC");
                else
                    printf("%s\r\n", "ERROR CRC");
            }
            buyte_count = my_buf_struct.buyte_count;
        }
    }
    return 0;
}
