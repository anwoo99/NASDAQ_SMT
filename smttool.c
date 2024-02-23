#include "nassmt.h"

/********************/
/* Convert Function */
/********************/

/*
 * Function: convert_big_endian_to_int64_t()
 * -----------------------------------------
 * 빅엔디안 -> Signed 정수
 */
void convert_big_endian_to_int64_t(char *from, int64_t *to, size_t size)
{
    int ii;

    *to = 0;
    for (ii = 0; ii < size; ii++)
    {
        *to = (*to << 8) | (unsigned char)from[ii];
    }

    /*
     * INT64_C(value): define a constant of type int64_t
     * INT64_C(-1) == 0xFFFFFFFFFFFFFFFF(총 64비트 = 8바이트)
     */
    if (from[0] & 0x80)
    {
        // Handle sign extension(부호 확장)
        *to |= (INT64_C(-1) << (8 * size));
    }
}

/*
 * Function: convert_big_endian_to_uint64_t()
 * -----------------------------------------
 * 빅엔디안 -> unsigned 정수
 */
void convert_big_endian_to_uint64_t(char *from, uint64_t *to, size_t size)
{
    convert_big_endian_to_int64_t(from, (int64_t *)to, size);
}

/*
 * Function: convert_nanosec_to_time_t()
 * -----------------------------------------
 * 나노초 -> time_t
 */
void convert_nanosec_to_time_t(uint64_t *from, time_t *to)
{
    time_t seconds = *from / 1000000000;

    // Get current time in Eastern Time
    struct tm easternTime;
    time_t currentTime = time(NULL);
    localtime_r(&currentTime, &easternTime);

    // Set the time components (hours, minutes, seconds)
    easternTime.tm_hour = seconds / 3600;
    easternTime.tm_min = (seconds % 3600) / 60;
    easternTime.tm_sec = seconds % 60;

    // Convert the struct tm to time_t
    *to = mktime(&easternTime);
}

/*
 * Function: getweek()
 * -----------------------------
 * Date(20240214) -> week number
 */
int getweek(int date)
{
    int week = 0;
    int year, month, mday, sat;
    time_t clock;
    struct tm *tm, time_c;
    int day_7 = 7;

    // Parse the expiration date to y/m/d(ex.20240214)
    year = YEAR(date);   // 24
    month = MONTH(date); // 02
    mday = MDAY(date);   // 14

    // Make struct tm variable
    memset(&time_c, 0, sizeof(struct tm));

    time_c.tm_year = year - 1900;
    time_c.tm_mon = month - 1;
    time_c.tm_mday = 1;

    // Convert to time_t
    clock = mktime(&time_c);

    // Calculate Saturday of the first week
    tm = localtime(&clock);
    sat = 7 - tm->tm_wday;
    if (sat == 1) // 그달 1일이 토요일부터 시작하면, 다음주를 첫주로 계산한다
        sat += 7;

    // Validate input date
    if (mday < 1 || mday > 31 || month < 1 || month > 12)
        return 0;

    if (mday <= sat)
        week = 1;
    else if (mday <= sat + day_7)
        week = 2;
    else if (mday <= sat + (day_7 * 2))
        week = 3;
    else if (mday <= sat + (day_7 * 3))
        week = 4;
    else if (mday <= sat + (day_7 * 4))
        week = 5;

    return week;
}

/*
 * Function: getwday()
 * -----------------------------
 * Date(20240214) -> 요일
 */
int getwday(int date)
{
    int year, month, mday;
    time_t clock;
    struct tm *tm, time_c;

    // Parse the expiration date to y/m/d(ex.20240214)
    year = YEAR(date);   // 24
    month = MONTH(date); // 02
    mday = MDAY(date);   // 14

    // Make struct tm variable
    memset(&time_c, 0, sizeof(struct tm));

    time_c.tm_year = year - 1900;
    time_c.tm_mon = month - 1;
    time_c.tm_mday = mday;

    // Convert to time_t
    clock = mktime(&time_c);

    // Convert to localtime
    tm = localtime(&clock);

    return (tm->tm_wday);
}

/*
 * Function: gettime_from_mid_sec()
 * -------------------------------
 * 자정부터 경과한 초 => time_t 값
 */
time_t gettime_from_mid_sec(uint64_t seconds_from_midnight)
{
    time_t curr;
    struct tm time_c;

    // 현재 시간 구하기
    time(&curr);

    // 현재 시간을 지역 시간으로 변환
    localtime_r(&curr, &time_c);

    // 오늘 자정으로 설정
    time_c.tm_hour = 0;
    time_c.tm_min = 0;
    time_c.tm_sec = 0;

    // 지정된 초를 더함
    time_c.tm_sec += seconds_from_midnight;

    // mktime을 사용하여 struct tm을 time_t로 변환
    return mktime(&time_c);
}

/********************************/
/* Fixed Field Mapping Function */
/********************************/

/*
 * Function: msg2fixedfld()
 * -----------------------------------------
 * 메세지버퍼 -> Fixed Field 파싱
 */
int msg2fixedfld(FIXEDFLD *fixedfld, char *msgb, int offset)
{
    int64_t ival;

    if (fixedfld->value == NULL)
        return (0);

    switch (fixedfld->field_type)
    {
    case FIXEDFLD_INT:
        convert_big_endian_to_int64_t(&msgb[offset], (int64_t *)(fixedfld->value), *fixedfld->field_length);
        break;
    case FIXEDFLD_UINT:
    case FIXEDFLD_BITMASK:
        convert_big_endian_to_uint64_t(&msgb[offset], (uint64_t *)(fixedfld->value), *fixedfld->field_length);
        break;
    case FIXEDFLD_STRING:
        memcpy((char *)(fixedfld->value), &msgb[offset], *(fixedfld->field_length));
        break;
    case FIXEDFLD_DECIMAL:
        convert_big_endian_to_int64_t(&msgb[offset], &ival, *(fixedfld->field_length));
        ((DECIMAL *)(fixedfld->value))->value = (double)ival / (pow(10, ((DECIMAL *)(fixedfld->value))->denominator));
        break;
    case FIXEDFLD_CHAR:
        *((char *)(fixedfld->value)) = msgb[offset];
        break;
    default:
        break;
    }

    return (0);
}

/*
 * Function: decode_fixedfld()
 * -------------------------------------
 * FIXEDFLD 개별 Decode
 */
void decode_fixedfld(FIXEDFLD *fixedfld, char *msgb, int *offset, int fldnum)
{
    msg2fixedfld(&fixedfld[fldnum], msgb, *offset);
    *offset += *(fixedfld[fldnum].field_length);
}

/*
 * Function: decode_fixedfld_all()
 * -------------------------------------
 * FIXEDFLD Array 전체 Decode
 */
void decode_fixedfld_all(FIXEDFLD *fixedfld, char *msgb, int *offset)
{
    int fldnum = 0;

    // Iterate through fields and decode
    while (fixedfld[fldnum].is_append == 0)
    {
        decode_fixedfld(fixedfld, msgb, offset, fldnum);
        fldnum++;
    }
}

/***************************/
/* Message Buffer Function */
/***************************/

/*
 * Function: read_msg_buff()
 * -------------------------------------
 * Message 버퍼 Read
 */
int read_msg_buff(MSGBUFF *msgbuff, FIXEDFLD *fixedfld)
{
    if (msgbuff->rest_size < size)
        return MSG_BUFFER_SCARCED;

    msg2fixedfld(fixedfld, msgbuff->buffer, msgbuff->offset);

    msgbuff->rest_size -= size;
    msgbuff->offset += size;
    msgbuff->msgl = size;
    return 0;
}

/*
 * Function: finish_msg_buff()
 * -------------------------------------
 * Message 버퍼 Read 종료
 */
int finish_msg_buff(MSGBUFF *msgbuff)
{
    memmove(msgbuff->buffer, &msgbuff->buffer[msgbuff->offset], msgbuff->rest_size);
    msgbuff->offset = 0;
    msgbuff->read_size = 0;
    msgbuff->msgl = 0;
    return 0;
}

/*
 * Function: restore_msg_buff()
 * -------------------------------------
 * Message 버퍼 Read 이전 상태 복구
 */
int restore_msg_buff(MSGBUFF *msgbuff, size_t size)
{
    if (size > msgbuff->offset)
        return MSG_INVALID_RESTORE_SIZE;

    msgbuff->rest_size += size;
    msgbuff->offset -= size;
    finish_msg_buff(msgbuff);
    return 0;
}

int proc_msg_buff(MSGBUFF *msgbuff, FIXEDFLD *fixedfld)
{
    int retv = 0;
    int restore_size = 0;
    int ii;

    for (ii = 0; strlen(fixedfld[ii].field_name) > 0; ii++)
    {
        retv = read_msg_buff(msgbuff, &fixedfld[ii]);

        if (retv & MSG_BUFFER_SCARCED)
        {
            if (restore_size > 0)
                restore_msg_buff(msgbuff, restore_size);

            finish_msg_buff(msgbuff);
            return (MSG_BUFFER_SCARCED);
        }

        restore_size += *fixedfld[ii].field_length;
    }

    return (0);
}

/*
 * Function: initialize_msg_buff()
 * -------------------------------------
 * Message 버퍼 초기화
 */
void initialize_msg_buff(MSGBUFF *msgbuff)
{
    memset(msgbuff->buffer, 0x00, sizeof(msgbuff->buffer));
    msgbuff->offset = 0;
    msgbuff->read_size = 0;
    msgbuff->rest_size = 0;
    msgbuff->msgl = 0;
}

/**********************/
/* TR Packet Function */
/**********************/

/*
 * Function: allocate_tr_packet()
 * -------------------------------------
 * IPC용 TR_PACKET 할당
 */
int allocate_tr_packet(TR_PACKET *tr_packet, char *message_data, size_t message_length)
{
    convert_big_endian_to_uint64_t(message_data, &tr_packet->header.type, 1);
    memcpy(tr_packet->pkt_buff, message_data, message_length);
    tr_packet->pkt_l = message_length;
    tr_packet->header.seqn += 1;

    return (0);
}

/*
 * Function: initialize_tr_packet()
 * -------------------------------------
 * IPC용 TR_PACKET 초기화
 */
void initialize_tr_packet(TR_PACKET *tr_packet)
{
    memset(tr_packet->pkt_buff, 0x00, sizeof(tr_packet->pkt_buff));
    tr_packet->header.seqn = 0;
    tr_packet->header.type = 0;
}

void initialize_smart(SMARTOPTION_TABLE *smt_table, FEP *fep, TOKEN *token)
{
    TR_PACKET *tr_packet;

    memset(smt_table, 0x00, sizeof(SMARTOPTION_TABLE));

    smt_table->fep = fep;
    smt_table->token = token;

    tr_packet = (TR_PACKET *)token->rcvb;
    smt_table->type = tr_packet->header.type;
    smt_table->raw_data_l = tr_packet->pkt_l;
    memcpy(smt_table->raw_data, tr_packet->pkt_buff, (int)tr_packet->pkt_l);
}

/**********/
/* Logger */
/**********/

// Function to create a directory if it doesn't exist
int createDirectory(char *path)
{
    struct stat st;

    // Check if the directory already exists
    if (stat(path, &st) == 0)
    {
        return 0;
    }

    // Try to create the directory
    if (mkdir(path, 0777) != 0)
    {
        return -1;
    }
    return 0;
}

/*
 * Function: nas_smt_log()
 * -------------------------------------
 * Raw Data STRING 로그
 */
void nas_smt_log(FEP *fep, SMARTOPTION_TABLE *smt_table, const char *format, ...)
{
    FILE *logF;
    time_t clock;
    struct tm tm, tx;
    struct stat lstat;
    char logdir[1024], logmsg[1024 * 8], logpath[128], mode[8];
    va_list vl;

    if (smt_table->loglevel > fep->llog)
    {
        return;
    }

    clock = time(0);
    clock += fep->e2lt;
    localtime_r(&clock, &tm);

    sprintf(logdir, "%s/Smart", LOG_DIR);

    if (createDirectory(logdir) != 0)
    {
        return;
    }

    switch (smt_table->class)
    {
    case SMT_NBBO_CLASS:
        snprintf(logpath, sizeof(logpath), "%s/%s_NBBO-%d.log", logdir, fep->exnm, tm.tm_wday);
        break;
    case SMT_TRADE_CLASS:
        snprintf(logpath, sizeof(logpath), "%s/%s_TRADE-%d.log", logdir, fep->exnm, tm.tm_wday);
        break;
    case SMT_DEFAULT_CLASS:
        snprintf(logpath, sizeof(logpath), "%s/%s_0x%02X-%d.log", logdir, fep->exnm, (unsigned int)smt_table->type, tm.tm_wday);
        break;
    default:
        return;
    }

    snprintf(mode, sizeof(mode), "a");

    if (stat(logpath, &lstat) == 0)
    {
        clock = lstat.st_mtime + fep->e2lt;
        localtime_r(&clock, &tx);

        if (tx.tm_yday != tm.tm_yday)
        {
            strcpy(mode, "w");
        }
    }

    snprintf(logmsg, sizeof(logmsg), "%02d/%02d %02d:%02d:%02d %s ", tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, fep->procname);

    va_start(vl, format);
    vsnprintf(&logmsg[strlen(logmsg)], sizeof(logmsg) - strlen(logmsg), format, vl);
    va_end(vl);

    if ((logF = fopen(logpath, mode)) != NULL)
    {
        fprintf(logF, "%s", logmsg);
        fputc('[', logF);
        fwrite(smt_table->raw_data, 1, smt_table->raw_data_l, logF);
        fputc(']', logF);
        fputc('\n', logF);

        fclose(logF);
    }
}

/*
 * Function: nas_smt_csv()
 * -------------------------------------
 * Raw Data CSV 로그
 */
void nas_smt_csv(FEP *fep, SMARTOPTION_TABLE *smt_table)
{
    FILE *logF;
    time_t clock;
    struct tm tm, tx;
    struct stat lstat;
    char logdir[1024], logmsg[1024 * 8], logheader[1024 * 8], logpath[128], mode[8];
    char rawdata[1024 * 8];
    int ii, len;

    if (smt_table->loglevel > fep->llog)
    {
        return;
    }

    clock = time(0);
    clock += fep->e2lt;
    localtime_r(&clock, &tm);

    sprintf(logdir, "%s/Smart", LOG_DIR);

    if (createDirectory(logdir) != 0)
    {
        return;
    }

    switch (smt_table->class)
    {
    case SMT_NBBO_CLASS:
        snprintf(logpath, sizeof(logpath), "%s/%s_NBBO-%d.csv", logdir, fep->exnm, tm.tm_wday);
        break;
    case SMT_TRADE_CLASS:
        snprintf(logpath, sizeof(logpath), "%s/%s_TRADE-%d.csv", logdir, fep->exnm, tm.tm_wday);
        break;
    case SMT_DEFAULT_CLASS:
        snprintf(logpath, sizeof(logpath), "%s/%s_0x%02X-%d.csv", logdir, fep->exnm, (unsigned int)smt_table->type, tm.tm_wday);
        break;
    default:
        return;
    }

    snprintf(mode, sizeof(mode), "ab");

    if (stat(logpath, &lstat) == 0)
    {
        clock = lstat.st_mtime + fep->e2lt;
        localtime_r(&clock, &tx);

        if (tx.tm_yday != tm.tm_yday)
        {
            strcpy(mode, "wb");
        }
    }

    memset(rawdata, 0x00, sizeof(rawdata));
    for (ii = 0; ii < smt_table->raw_data_l; ii++)
    {
        len = strlen(rawdata);
        sprintf(&rawdata[len], "%02X ", (unsigned char)smt_table->raw_data[ii]);
    }

    if (smt_table->loghead[strlen(smt_table->loghead) - 1] == ',')
        smt_table->loghead[strlen(smt_table->loghead) - 1] = '\0';

    if (smt_table->logmsg[strlen(smt_table->logmsg) - 1] == ',')
        smt_table->logmsg[strlen(smt_table->logmsg) - 1] = '\0';

    sprintf(logheader, "Date,Time,%s,Raw Data", smt_table->loghead);
    sprintf(logmsg, "%02d/%02d,%02d:%02d:%02d,%s,%s", tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, smt_table->logmsg, rawdata);

    if ((logF = fopen(logpath, mode)) != NULL)
    {
        fseek(logF, 0, SEEK_END);

        if (ftell(logF) == 0)
            fprintf(logF, "%s\n", logheader);

        fprintf(logF, "%s\n", logmsg);
        fclose(logF);
    }
}
