#include "nastag.h"

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
        *to = (*to << 8) | from[ii];
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

/***************************/
/* Message Buffer Function */
/***************************/
int read_msg_buff(MSGBUFF *msgbuff, FIXEDFLD *fixedfld)
{
    if (msgbuff->rest_size < *fixedfld->field_length)
        return MSG_BUFFER_SCARCED;

    msg2fixedfld(fixedfld, msgbuff->buffer, msgbuff->offset);

    msgbuff->rest_size -= *fixedfld->field_length;
    msgbuff->offset += *fixedfld->field_length;
    msgbuff->msgl = *fixedfld->field_length;
    return 0;
}

int finish_msg_buff(MSGBUFF *msgbuff)
{
    memmove(msgbuff->buffer, &msgbuff->buffer[msgbuff->offset], msgbuff->rest_size);
    msgbuff->offset = 0;
    msgbuff->read_size = 0;
    msgbuff->msgl = 0;
    return 0;
}

int restore_msg_buff(MSGBUFF *msgbuff, size_t size)
{
    if (size > msgbuff->offset)
        return MSG_INVALID_RESTORE_SIZE;

    msgbuff->rest_size += size;
    msgbuff->offset -= size;
    finish_msg_buff(msgbuff);
    return 0;
}

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
int allocate_tr_packet(TR_PACKET *tr_packet, char *message_data, size_t message_length)
{
    tr_packet->header.type = (unsigned char)message_data[0];
    memcpy(tr_packet->pkt_buff, message_data, message_length);
    tr_packet->pkt_l = message_length;
    tr_packet->header.seqn += 1;

    return (0);
}

void initialize_tr_packet(TR_PACKET *tr_packet)
{
    memset(tr_packet->pkt_buff, 0x00, sizeof(tr_packet->pkt_buff));
    tr_packet->header.seqn = 0;
    tr_packet->header.type = 0;
}

/**********/
/* Logger */
/**********/
void nas_raw_log(FEP *fep, int level, int flag, char *msgb, int msgl, const char *format, ...)
{
    FILE *logF;
    time_t clock;
    struct tm tm, tx;
    struct stat lstat;
    char logmsg[1024 * 8], logpath[128], mode[8];
    va_list vl;

    if (level > fep->llog)
    {
        return;
    }

    clock = time(0);
    clock += fep->e2lt;
    localtime_r(&clock, &tm);

    switch (flag)
    {
    case 1:
        snprintf(logpath, sizeof(logpath), "%s/Nasdaq/%s_RAW_MASTER-%d.log", LOG_DIR, fep->exnm, tm.tm_wday);
        break;
    case 2:
        snprintf(logpath, sizeof(logpath), "%s/Nasdaq/%s_RAW_TRADE-%d.log", LOG_DIR, fep->exnm, tm.tm_wday);
        break;
    default:
        snprintf(logpath, sizeof(logpath), "%s/Nasdaq/%s_RAW-%d.log", LOG_DIR, fep->exnm, tm.tm_wday);
        break;
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
        fwrite(msgb, 1, msgl, logF);
        fputc(']', logF);
        fputc('\n', logF);

        fclose(logF);
    }
}

void nas_raw_csv(FEP *fep, int level, int type, char *header, char *message)
{
    FILE *logF;
    time_t clock;
    struct tm tm, tx;
    struct stat lstat;
    char logmsg[1024 * 8], logheader[1024 * 8], logpath[128], mode[8];

    if (level > fep->llog)
    {
        return;
    }

    clock = time(0);
    clock += fep->e2lt;
    localtime_r(&clock, &tm);

    snprintf(logpath, sizeof(logpath), "%s/Nasdaq/%s_0x%02X-%d.csv", LOG_DIR, fep->exnm, type, tm.tm_wday);

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

    sprintf(logheader, "Date,Time,%s", header);
    sprintf(logmsg, "%02d/%02d,%02d:%02d:%02d,%s", tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, message);

    if ((logF = fopen(logpath, mode)) != NULL)
    { 
	fseek(logF, 0, SEEK_END);

        if (ftell(logF) == 0)
            fprintf(logF, "%s\n", logheader);

        fprintf(logF, "%s\n", logmsg);
        fclose(logF);
    }
}
