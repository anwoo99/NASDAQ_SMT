#include "nastag.h"

void convert_big_endian_to_uint64_t(unsigned char *from, uint64_t *to, size_t size)
{
    int ii;

    *to = 0;
    for (ii = 0; ii < size; ii++)
    {
        *to = (*to << 8) | from[ii];
    }
}

void convert_big_endian_to_int64_t(unsigned char *from, int64_t *to, size_t size)
{
    int ii;

    *to = 0;
    for (ii = 0; ii < size; ii++)
    {
        *to = (*to << 8) | from[ii];
    }

    // Handle sign extension
    if (from[0] & 0x80)
    {
        *to |= (INT64_C(-1) << (8 * size));
    }
}

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

int msg2fixedfld(FIXEDFLD *fixedfld, int offset, size_t size)
{
    

}

/***************************/
/* Message Buffer Function */
/***************************/
int uint64_t_read_msg_buff(MSGBUFF *msgbuff, uint64_t *value, size_t size)
{
    if (msgbuff->rest_size < size)
        return MSG_BUFFER_SCARCED;

    convert_big_endian_to_uint64_t(&msgbuff->buffer[msgbuff->offset], value, size);

    msgbuff->rest_size -= size;
    msgbuff->offset += size;
    msgbuff->msgl = size;
    return 0;
}

int copy_read_msg_buff(MSGBUFF *msgbuff, unsigned char *value, size_t size)
{
    if (msgbuff->rest_size < size)
        return MSG_BUFFER_SCARCED;

    memcpy(value, &msgbuff->buffer[msgbuff->offset], size);

    msgbuff->rest_size -= size;
    msgbuff->offset += size;
    msgbuff->msgl = size;
    return 0;
}

int finish_read_msg_buff(MSGBUFF *msgbuff)
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
    finish_read_msg_buff(msgbuff);
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
int allocate_tr_packet(TR_PACKET *tr_packet, unsigned char *message_data, size_t message_length)
{
    tr_packet->header.type = message_data[0];
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
        snprintf(logpath, sizeof(logpath), "%s/%s_RAW_MASTER-%d.log", LOG_DIR, fep->exnm, tm.tm_wday);
        break;
    case 2:
        snprintf(logpath, sizeof(logpath), "%s/%s_RAW_TRADE-%d.log", LOG_DIR, fep->exnm, tm.tm_wday);
        break;
    default:
        snprintf(logpath, sizeof(logpath), "%s/%s_RAW-%d.log", LOG_DIR, fep->exnm, tm.tm_wday);
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

void nas_raw_csv(FEP *fep, int level, uint64_t type, char *header, const char *format, ...)
{
    FILE *logF;
    time_t clock;
    struct tm tm, tx;
    struct stat lstat;
    char loghead[1024 * 8], logmsg[1024 * 8], logpath[128], mode[8];
    long fileSize;
    char *msgptr;
    va_list vl;

    if (level > fep->llog || type == 0)
        return;

    clock = time(0);
    clock += fep->e2lt;
    localtime_r(&clock, &tm);

    sprintf(logpath, "%s/%s_0x%02X-%d.log", LOG_DIR, fep->exnm, type, tm.tm_wday);
    sprintf(mode, "a");

    if (stat(logpath, &lstat) == 0)
    {
        clock = lstat.st_mtime + fep->e2lt;
        localtime_r(&clock, &tx);
        if (tx.tm_yday != tm.tm_yday)
            strcpy(mode, "w");
    }

    sprintf(logmsg, "%02d/%02d %02d:%02d:%02d, %s, ", tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, fep->procname);

    va_start(vl, format);
    msgptr = &logmsg[strlen(logmsg)];
    vsprintf(msgptr, format, vl);
    va_end(vl);

    if ((logF = fopen(logpath, mode)) != NULL)
    {
        if (lstat.st_size == 0)
        {
            strcpy(loghead, "Datetime,Process Name,");
            memcpy(&loghead[strlen(loghead)], header);
            fprintf(logF, "%s\n", loghead);
        }

        fprintf(logF, "%s\n", logmsg);
        fclose(logF);
    }
}
