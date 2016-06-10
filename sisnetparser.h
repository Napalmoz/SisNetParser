#ifndef SISNETPARSER
#define SISNETPARSER

typedef struct {            /* SisNet message type */
    sbsmsg_t sbsmsg;        //Сообщение SBAS
    bool waas_crc_flag;     //флаг CRC (true:ok,false:error)
    unsigned int waas_crc;  //CRC SBAS сообщения 24 bit
    time_t recive_time;     //время приема SisNet сообщения
    int type;               //тип сообщения SBAS
    bool is_msg_null;       //флаг пустого сообщения (true:сообщ. пустое,false:сообщение номальное)
} sisnetmsg_t;

time_t GetReciveTime();
unsigned int ParseSisNetMsg(unsigned char *msg, int n, sisnetmsg_t *sisnetmsg);
#endif // SISNETPARSER

