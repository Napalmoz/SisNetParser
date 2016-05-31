#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <pthread.h>
#include <ntripclient.h>
#include <rtklib.h>
#include <sbas.h>
#include <sisnetparser.h>

/* get fields (big-endian) ---------------------------------------------------*/
//#define U1(p) (*((unsigned char *)(p)))
//#define I1(p) (*((char *)(p)))

//typedef struct {        /* SisNet message type */
//    sbsmsg_t sbsmsg;        //Сообщение SBAS
//    bool waas_crc_flag;     //флаг CRC (true:ok,false:error)
//    unsigned int waas_crc;  //CRC SBAS сообщения 24 bit
//    gtime_t recive_time;    //время приема SisNet сообщения
//    int type;               //тип сообщения SBAS
//} sisnetmsg_t;

//void ParseMsg(unsigned char *msg, int n);
//unsigned int ParseSisNetMsg(unsigned char *msg, int n, sisnetmsg_t *sisnetmsg);

int main()
{
    //char *s  = (char*)"192.168.111.17";
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

    unsigned char *msg = (unsigned char*)"*MSG,1898,122152,531000140700440180000000000000B40E4084000003BBBBBBBBBBBBAFCB6800*E4\r\n";
    unsigned char *pp = (unsigned char *)malloc(1024);
    int count = 86;
    memcpy(pp,msg,count);

    unsigned char ch = pp[1];

    //ParseMsg(msg, count);

    sisnetmsg_t sisnetMsg;
    gtime_t time;
    char tstr[32];

    ParseSisNetMsg(msg, count, &sisnetMsg);

    //return 0;

    while(true)
    {
        my_buf_struct = client->GetBuf();
        if(buyte_count != my_buf_struct.buyte_count)
        {
            //fwrite(my_buf_struct.buf, my_buf_struct.size_msg, 1, stdout);
            //ParseMsg((unsigned char*)my_buf_struct.buf, my_buf_struct.size_msg);
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

//static unsigned short U2(unsigned char *p)
//{
//    unsigned short value;
//    unsigned char *q=(unsigned char *)&value+1;
//    int i;
//    for (i=0;i<2;i++) *q--=*p++;
//    return value;
//}
//static unsigned int U4(unsigned char *p)
//{
//    unsigned int value;
//    unsigned char *q=(unsigned char *)&value+3;
//    int i;
//    for (i=0;i<4;i++) *q--=*p++;
//    return value;
//}
//static int I4(unsigned char *p)
//{
//    return (int)U4(p);
//}
//static float R4(unsigned char *p)
//{
//    float value;
//    unsigned char *q=(unsigned char *)&value+3;
//    int i;
//    for (i=0;i<4;i++) *q--=*p++;
//    return value;
//}
//static double R8(unsigned char *p)
//{
//    double value;
//    unsigned char *q=(unsigned char *)&value+7;
//    int i;
//    for (i=0;i<8;i++) *q--=*p++;
//    return value;
//}

//extern int sbsupdatecorr(const sbsmsg_t *msg, nav_t *nav)
//{
//    int type=getbitu(msg->msg,8,6),stat=-1;

//    trace(3,"sbsupdatecorr: type=%d\n",type);

//    //if (msg->week==0) return -1;

//    switch (type) {
////        case  0: stat=decode_sbstype2 (msg,&nav->sbssat); break;
////        case  1: stat=decode_sbstype1 (msg,&nav->sbssat); break;
////        case  2:
////        case  3:
////        case  4:
////        case  5: stat=decode_sbstype2 (msg,&nav->sbssat); break;
////        case  6: stat=decode_sbstype6 (msg,&nav->sbssat); break;
////        case  7: stat=decode_sbstype7 (msg,&nav->sbssat); break;
////        case  9: stat=decode_sbstype9 (msg,nav);          break;
////        case 18: stat=decode_sbstype18(msg,nav ->sbsion); break;
////        case 24: stat=decode_sbstype24(msg,&nav->sbssat); break;
////        case 25: stat=decode_sbstype25(msg,&nav->sbssat); break;
////        case 26: stat=decode_sbstype26(msg,nav ->sbsion); break;
////        case 63: break; /* null message */

//        /*default: trace(2,"unsupported sbas message: type=%d\n",type); break;*/
//    }
//    //return stat?type:-1;
//    return type;
//}

///* decode NVS xf5-raw: raw measurement data ----------------------------------*/
//static int decode_xf5raw(raw_t *raw)
//{
//    gtime_t time;
//    double tadj=0.0,toff=0.0,tn;
//    int dTowInt;
//    double dTowUTC, dTowGPS, dTowFrac, L1, P1, D1;
//    double gpsutcTimescale;
//    unsigned char rcvTimeScaleCorr, sys, carrNo;
//    int i,j,prn,sat,n=0,nsat,week;
//    unsigned char *p=raw->buff+2;
//    char *q,tstr[32],flag;

//    //trace(4,"decode_xf5raw: len=%d\n",raw->len);

//    /* time tag adjustment option (-TADJ) */
//    if ((q=strstr(raw->opt,"-tadj"))) {
//        sscanf(q,"-TADJ=%lf",&tadj);
//    }
//    dTowUTC =R8(p);
//    week = U2(p+8);
//    gpsutcTimescale = R8(p+10);
//    /* glonassutcTimescale = R8(p+18); */
//    rcvTimeScaleCorr = I1(p+26);

//    /* check gps week range */
//    if (week>=4096) {
//        //trace(2,"nvs xf5raw obs week error: week=%d\n",week);
//        return -1;
//    }
//    week=adjgpsweek(week);

//    if ((raw->len - 31)%30) {

//        /* Message length is not correct: there could be an error in the stream */
//        //trace(2,"nvs xf5raw len=%d seems not be correct\n",raw->len);
//        return -1;
//    }
//    nsat = (raw->len - 31)/30;

//    dTowGPS = dTowUTC + gpsutcTimescale;

//    /* Tweak pseudoranges to allow Rinex to represent the NVS time of measure */
//    dTowInt  = 10.0*floor((dTowGPS/10.0)+0.5);
//    dTowFrac = dTowGPS - (double) dTowInt;
//    time=gpst2time(week, dTowInt*0.001);

//    /* time tag adjustment */
//    if (tadj>0.0) {
//        tn=time2gpst(time,&week)/tadj;
//        toff=(tn-floor(tn+0.5))*tadj;
//        time=timeadd(time,-toff);
//    }
//    /* check time tag jump */
//    if (raw->time.time&&fabs(timediff(time,raw->time))>86400.0) {
//        time2str(time,tstr,3);
//        trace(2,"nvs xf5raw time tag jump error: time=%s\n",tstr);
//        return 0;
//    }
//    if (fabs(timediff(time,raw->time))<=1e-3) {
//        time2str(time,tstr,3);
//        trace(2,"nvs xf5raw time tag duplicated: time=%s\n",tstr);
//        return 0;
//    }
//    for (i=0,p+=27;(i<nsat) && (n<MAXOBS); i++,p+=30) {
//        raw->obs.data[n].time  = time;
//        sys = (U1(p)==1)?SYS_GLO:((U1(p)==2)?SYS_GPS:((U1(p)==4)?SYS_SBS:SYS_NONE));
//        prn = U1(p+1);
//        if (sys == SYS_SBS) prn += 120; /* Correct this */
//        if (!(sat=satno(sys,prn))) {
//            trace(2,"nvs xf5raw satellite number error: sys=%d prn=%d\n",sys,prn);
//            continue;
//        }
//        carrNo = I1(p+2);
//        L1 = R8(p+ 4);
//        P1 = R8(p+12);
//        D1 = R8(p+20);

//        /* check range error */
//        if (L1<-1E10||L1>1E10||P1<-1E10||P1>1E10||D1<-1E5||D1>1E5) {
//            trace(2,"nvs xf5raw obs range error: sat=%2d L1=%12.5e P1=%12.5e D1=%12.5e\n",
//                  sat,L1,P1,D1);
//            continue;
//        }
//        raw->obs.data[n].SNR[0]=(unsigned char)(I1(p+3)*4.0+0.5);
//        if (sys==SYS_GLO) {
//            raw->obs.data[n].L[0]  =  L1 - toff*(FREQ1_GLO+DFRQ1_GLO*carrNo);
//        } else {
//            raw->obs.data[n].L[0]  =  L1 - toff*FREQ1;
//        }
//        raw->obs.data[n].P[0]    = (P1-dTowFrac)*CLIGHT*0.001 - toff*CLIGHT; /* in ms, needs to be converted */
//        raw->obs.data[n].D[0]    =  (float)D1;

//        /* set LLI if meas flag 4 (carrier phase present) off -> on */
//        flag=U1(p+28);
//        raw->obs.data[n].LLI[0]=(flag&0x08)&&!(raw->halfc[sat-1][0]&0x08)?1:0;
//        raw->halfc[sat-1][0]=flag;

//#if 0
//        if (raw->obs.data[n].SNR[0] > 160) {
//            time2str(time,tstr,3);
//            trace(2,"%s, obs.data[%d]: SNR=%.3f  LLI=0x%02x\n",  tstr,
//                n, (raw->obs.data[n].SNR[0])/4.0, U1(p+28) );
//        }
//#endif
//        raw->obs.data[n].code[0] = CODE_L1C;
//        raw->obs.data[n].sat = sat;

//        for (j=1;j<NFREQ+NEXOBS;j++) {
//            raw->obs.data[n].L[j]=raw->obs.data[n].P[j]=0.0;
//            raw->obs.data[n].D[j]=0.0;
//            raw->obs.data[n].SNR[j]=raw->obs.data[n].LLI[j]=0;
//            raw->obs.data[n].code[j]=CODE_NONE;
//        }
//        n++;
//    }
//    raw->time=time;
//    raw->obs.n=n;
//    return 1;
//}

///* decode sisnet ------------------------------------------------------*/
//static int decode_sisnet(raw_t *raw)
//{
//    int i=0;
//    unsigned int b;

//    char *p= (char*)raw->buff+5;

//    sscanf(p,"%d,%d,",&raw->sbsmsg.week,&raw->sbsmsg.tow);

//    p = strrchr(p, ',')+1;

//    for (i=0;i<29;i++)
//    {
//        b=0;
//        sscanf(p,"%2x", &b);
//        p+=2;
//        raw->sbsmsg.msg[i] = b;
//    }

//    raw->sbsmsg.msg[28]&=0xC0;

//    raw->sbsmsg.prn=120;	//SISNET PRN

//    return 3;
//}

//extern int input_sisnet(raw_t *raw, unsigned char data)
//{
//    int len;

//    if (raw->nbyte==0) {
//        if (data!='*') return 0;
//    }

//    raw->buff[raw->nbyte++]=data;

//    if( data=='\r' )
//    {
//        len = raw->nbyte;
//        raw->buff[raw->nbyte++]='\0';
//        raw->nbyte = 0;
//        //printf("SBAS: %s", raw->buff);
//        if(len>50)
//            return decode_sisnet(raw);
//        return 0;
//    }
//    return 0;
//}

//extern int input_raw(raw_t *raw, int format, unsigned char data)
//{
//    //trace(5,"input_raw: format=%d data=0x%02x\n",format,data);

//    switch (format) {
////        case STRFMT_OEM4 : return input_oem4 (raw,data);
////        case STRFMT_OEM3 : return input_oem3 (raw,data);
////        case STRFMT_UBX  : return input_ubx  (raw,data);
////        case STRFMT_SS2  : return input_ss2  (raw,data);
////        case STRFMT_CRES : return input_cres (raw,data);
////        case STRFMT_STQ  : return input_stq  (raw,data);
////        case STRFMT_GW10 : return input_gw10 (raw,data);
////        case STRFMT_JAVAD: return input_javad(raw,data);
////        case STRFMT_NVS  : return input_nvs  (raw,data);
////        case STRFMT_BINEX: return input_bnx  (raw,data);
////        case STRFMT_LEXR : return input_lexr (raw,data);
//        case STRFMT_SBAS : return input_sisnet (raw,data);
//    }
//    return 0;
//}

//static int decoderaw(rtksvr_t *svr, int index)
//{
//    obs_t *obs;
//    nav_t *nav;
//    sbsmsg_t *sbsmsg=NULL;
//    int i,ret,sat,fobs=0;

//    //tracet(4,"decoderaw: index=%d\n",index);

//    //rtksvrlock(svr);

//    for (i=0;i<svr->nb[index];i++) {

//        /* input rtcm/receiver raw data from stream */
//        if (svr->format[index]==STRFMT_RTCM2) {
//            //ret=input_rtcm2(svr->rtcm+index,svr->buff[index][i]);
//            ret=input_sisnet(svr->raw+index, svr->buff[index][i]);
//            obs=&svr->rtcm[index].obs;
//            nav=&svr->rtcm[index].nav;
//            sat=svr->rtcm[index].ephsat;
//        }
//        else if (svr->format[index]==STRFMT_RTCM3) {
//            //ret=input_rtcm3(svr->rtcm+index,svr->buff[index][i]);
//            obs=&svr->rtcm[index].obs;
//            nav=&svr->rtcm[index].nav;
//            sat=svr->rtcm[index].ephsat;
//        }
//        else {
//            //ret=input_raw(svr->raw+index,svr->format[index],svr->buff[index][i]);
//            ret=input_sisnet(svr->raw+index, svr->buff[index][i]);
//            obs=&svr->raw[index].obs;
//            nav=&svr->raw[index].nav;
//            sat=svr->raw[index].ephsat;
//            sbsmsg=&svr->raw[index].sbsmsg;
//        }
//#if 0 /* record for receiving tick */
//        if (ret==1) {
//            trace(0,"%d %10d T=%s NS=%2d\n",index,tickget(),
//                  time_str(obs->data[0].time,0),obs->n);
//        }
//#endif
//        /* update rtk server */
//        //if (ret>0) updatesvr(svr,ret,obs,nav,sat,sbsmsg,index,fobs);

//        /* observation data received */
//        if (ret==1) {
//            if (fobs<MAXOBSBUF) fobs++; else svr->prcout++;
//        }
//    }
//    svr->nb[index]=0;

//    //rtksvrunlock(svr);

//    return fobs;
//}

//extern int sbsdecodemsg(gtime_t time, int prn, const unsigned int *words,
//                        sbsmsg_t *sbsmsg)
//{
//    int i,j;
//    unsigned char f[29];
//    double tow;

//    //trace(5,"sbsdecodemsg: prn=%d\n",prn);

//    if (time.time==0) return 0;
//    tow=time2gpst(time,&sbsmsg->week);
//    sbsmsg->tow=(int)(tow+DTTOL);
//    sbsmsg->prn=prn;
//    for (i=0;i<7;i++) for (j=0;j<4;j++) {
//        sbsmsg->msg[i*4+j]=(unsigned char)(words[i]>>((3-j)*8));
//    }
//    sbsmsg->msg[28]=(unsigned char)(words[7]>>18)&0xC0;
//    for (i=28;i>0;i--) f[i]=(sbsmsg->msg[i]>>6)+(sbsmsg->msg[i-1]<<2);
//    f[0]=sbsmsg->msg[0]>>6;

//    return crc24q(f,29)==(words[7]&0xFFFFFF); /* check crc */
//}

/* decode NVS rxm-sfrb: subframe buffer --------------------------------------*/
//static int decode_xe5bit(raw_t *raw)
//{
//    int prn;
//    int iBlkStartIdx, iExpLen, iIdx;
//    unsigned int words[10];
//    unsigned char uiDataBlocks, uiDataType;
//    unsigned char *p=raw->buff;

//    trace(4,"decode_xe5bit: len=%d\n",raw->len);

//    p += 2;         /* Discard preamble and message identifier */
//    uiDataBlocks = U1(p);

//    if (uiDataBlocks>=16) {
//        trace(2,"nvs xf5bit message error: data blocks %u\n", uiDataBlocks);
//        return -1;
//    }
//    iBlkStartIdx = 1;
//    for (iIdx = 0; iIdx < uiDataBlocks; iIdx++) {
//        iExpLen = (iBlkStartIdx+10);
//        if ((raw->len) < iExpLen) {
//            trace(2,"nvs xf5bit message too short (expected at least %d)\n", iExpLen);
//            return -1;
//        }
//        uiDataType = U1(p+iBlkStartIdx+1);

//        switch (uiDataType) {
//            case 1: /* Glonass */
//                iBlkStartIdx += 19;
//                break;
//            case 2: /* GPS */
//                iBlkStartIdx += 47;
//                break;
//            case 4: /* SBAS */
//                prn = U1(p+(iBlkStartIdx+2)) + 120;

//                /* sat = satno(SYS_SBS, prn); */
//                /* sys = satsys(sat,&prn); */
//                memset(words, 0, 10*sizeof(unsigned int));
//                for (iIdx=0, iBlkStartIdx+=7; iIdx<10; iIdx++, iBlkStartIdx+=4) {
//                    words[iIdx]=U4(p+iBlkStartIdx);
//                }
//                words[7] >>= 6;
//                return sbsdecodemsg(raw->time,prn,words,&raw->sbsmsg) ? 3 : 0;
//            default:
//                trace(2,"nvs xf5bit SNS type unknown (got %d)\n", uiDataType);
//                return -1;
//        }
//    }
//    return 0;
//}

//void SisNetMsgToWAAS(sbsmsg_t *sbsmsg, unsigned int *words)
//{
//    //"*MSG,1898,122152,531000140700440180000000000000B40E4084000003BBBBBBBBBBBBAFCB6800*E4\r\n";
//    int i,j;
//    unsigned int result = 0;
//    for (i=0;i<7;i++)
//    {
//        for (j=0;j<4;j++)
//        {
//            //sbsmsg->msg[i*4+j]=(unsigned char)(words[i]>>((3-j)*8));
//            result += (unsigned int)(sbsmsg->msg[i*4+j] << ((3-j)*8));
//        }
//        words[i] = result;
//        result = 0;
//    }
//    result = (unsigned int)(sbsmsg->msg[28] << 18);
//    words[7] = result;
//    unsigned int hvost = 0xafcb6800;
//    hvost >>= 18;
//    hvost &= 0xC0;
//    i += hvost;
//    //words[7] = 0xafcb6800;
//    words[7] = 0xafcb68;
//    //words[7] = 0x2bf2da;


//    //sbsmsg->msg[28]=(unsigned char)(words[7]>>18)&0xC0;
//}

//void SisNetMsgToWAAS(unsigned char *msg, unsigned int *words)
//{
//    int i=0;
//    unsigned int b;

//    char *p= (char*)msg+17;
//    //char *p= (char*)msg+19;

//    //sscanf(p,"%d,%d,",&raw->sbsmsg.week,&raw->sbsmsg.tow);

//    //p = strrchr(p, ',')+1;

//    for (i=0; i < 9; i++)
//    {
//        b=0;
//        sscanf(p,"%8x", &b);
//        p+=8;
//        words[i] = b;
//        //raw->sbsmsg.msg[i] = b;
//    }
//}

//unsigned int GetCRCWAAS(unsigned char *msg)
//{
//    unsigned int b;
//    char *p= (char*)msg+73;
//    sscanf(p,"%7x", &b);
//    b = (b>>2)&0xFFFFFF;
//}

//bool CheckCRCWAAS(sbsmsg_t *sbsmsg, unsigned int crc)
//{
//    unsigned char f[29];
//    int i;

//    for (i=28;i>0;i--) f[i]=(sbsmsg->msg[i]>>6)+(sbsmsg->msg[i-1]<<2);
//    f[0]=sbsmsg->msg[0]>>6;

//    return crc24q(f,29)==crc;
//}

///* Раскодировка Сиснте сообщения ---------------------------------------------------------
//* args   : unsigned char *msg       I   строка Сиснет сообщения
//*          int n                    I   длина строки Сиснет сообщения
//*          sisnetmsg_t *sbsmsg      O   структура сиснет соощения
//* return : status (1:ok,0:error) - ошибки при раскодировании
//*-----------------------------------------------------------------------------*/
//unsigned int ParseSisNetMsg(unsigned char *msg, int n, sisnetmsg_t *sisnetmsg)
//{
//    //иниц. структуру
//    sisnetmsg->waas_crc_flag = false;
//    sisnetmsg->type = -1;

//    //взять системное время записать в sbsmsg->recive_time;

//    //проверяем длину сообщ. Sisnet
//    if(n < 80)
//    {
//        return 0;
//    }
//    sisnetmsg->waas_crc = GetCRCWAAS(msg);

//    raw_t *raw = new raw_t;
//    memcpy(raw->buff,msg,n);
//    raw->nbyte = n;
//    if(!input_sisnet(raw, raw->buff[n-2]))
//        return 0;
//    sisnetmsg->sbsmsg = raw->sbsmsg;
//    if(sisnetmsg->sbsmsg.week == 0)
//        return 0;
//    sisnetmsg->waas_crc_flag = CheckCRCWAAS(&sisnetmsg->sbsmsg, sisnetmsg->waas_crc);
//    sisnetmsg->type = getbitu(sisnetmsg->sbsmsg.msg ,8,6);
//}

//void ParseMsg(unsigned char *msg, int n)
//{
//    char tstr[32];
//    int i;
//    unsigned int words[10];


//    //SisNetMsgToWAAS(msg, words);

//    raw_t *raw = new raw_t;
//    memcpy(raw->buff,msg,n);
//    raw->nbyte = n;
//    //raw->buff = &msg;
//    input_sisnet(raw, raw->buff[n-2]);
//    SisNetMsgToWAAS(&raw->sbsmsg, words);

//    //sbsupdatecorr(&raw->sbsmsg, NULL);
//    gtime_t time;

//    if(raw->sbsmsg.week != 0)
//    {
//        time = gpst2time(raw->sbsmsg.week, raw->sbsmsg.tow);
//        time2str(time,tstr,3);
//        //printf("%s %d\r\n", sbsupdatecorr(&raw->sbsmsg, NULL), tstr);
//        printf("%s ", tstr);
//        printf("%d\r\n", sbsupdatecorr(&raw->sbsmsg, NULL));
//    }

//    //fwrite(my_buf_struct.buf, my_buf_struct.size_msg, 1, stdout);

//    sbsmsg_t sbas_msg;

//    int s = sbsdecodemsg(time, 120, words, &sbas_msg);

//    //memcpy(raw->buff, raw->sbsmsg.msg, 29);
//    //memcpy(raw->buff, msg+17, n-17);

//    //decode_xe5bit(raw);

//    unsigned char f[29];

//    for (i=28;i>0;i--) f[i]=(raw->sbsmsg.msg[i]>>6)+(raw->sbsmsg.msg[i-1]<<2);
//    f[0]=raw->sbsmsg.msg[0]>>6;

//    //for (i=28;i>0;i--) f[i]=(sbas_msg.msg[i]>>6)+(sbas_msg.msg[i-1]<<2);
//    //f[0]=sbas_msg.msg[0]>>6;

//    //int j = crc24q(f,29)==(raw->sbsmsg.msg[7]&0xFFFFFF); /* check crc */
//    //int j = crc24q(f,29)==(words[7]&0xFFFFFF); /* check crc */
//    unsigned int j = crc24q(f,29);
//    int jj = crc24q(raw->sbsmsg.msg,29);
//    //int jj = crc24q(sbas_msg.msg,29);
//    int y = words[7]&0xFFFFFF;
//    //101011111100101101101000
//    int k = j + 1 + y;

//    //unsigned int crc = (0xBAFCB68>>2)&0xFFFFFF;
//    //unsigned int crc = (0xAFCB68<<2)&0xFFFFFF;
////    unsigned int crc = GetCRCWAAS(msg);
////    if(crc == j)
////        printf("%s\r\n", "OK CRC");
////    else
////        printf("%s\r\n", "ERROR CRC");

////    k += crc;
////    //decode_xf5raw(raw);
//    return;




//    //int i = 0;
//    rtksvr_t *svr = (rtksvr_t*)malloc(sizeof(rtksvr_t));
//    //svr->buffsize = 8192;

//    svr->buffsize=4096; /* >=4096byte */
//    svr->nmeacycle=1000; /* >=1s */
//    //for (i=0;i<3;i++) svr->nmeapos[i]=nmeapos?nmeapos[i]:0.0;

////    for (i=0;i<svr->nstr-1;i++) svr->conv[i]=conv[i];

////    if (!(svr->buff=(unsigned char *)malloc(svr->buffsize))||
////        !(svr->pbuf=(unsigned char *)malloc(svr->buffsize))) {
////        free(svr->buff); free(svr->pbuf);
////        return 0;
////    }

//    //rtksvr_t *svr = new rtksvr_t;
//    //rtksvr_t *svr;
//    unsigned char *p = (unsigned char *)malloc(1024);
//    unsigned char *q = (unsigned char *)malloc(1024);
//    svr->buff[i]=(unsigned char *)malloc(svr->buffsize);
//    svr->pbuf[i]=(unsigned char *)malloc(svr->buffsize);

//    //unsigned char *p,*q;
//    //char *p,*q;
//    //i = 0;
//    p=svr->buff[i]+svr->nb[i];
//    q=svr->buff[i]+svr->buffsize;
//    //memcpy(p,msg,n);
//    p = msg;
//    svr->nb[i]+=n;

//    n=n<svr->buffsize-svr->npb[i]?n:svr->buffsize-svr->npb[i];
//    memcpy(svr->pbuf[i]+svr->npb[i],p,n);
//    svr->npb[i]+=n;

//    //svr->raw[0].buff;

//    //fwrite(svr->raw[0].buff, n, 1, stdout);
//    decoderaw(svr, 0);
//    //input_sisnet(svr->raw+index, svr->buff[0][0]);
//}
