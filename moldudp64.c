#include "nastag.h"

uint64_t UDP_DWN_MSGBLK_MSGLEN_LEN = 2;

/* Moldudp64 Downstream Packet Message Block Parser */
int parser_moldudp64_message_block(MSGBUFF *msgbuff, MSG_BLOCK *msgblock)
{
    int retv = 0;
    FIXEDFLD fixedfld[] = {
        {"Message Length", FIXEDFLD_UINT, &UDP_DWN_MSGBLK_MSGLEN_LEN, &msgblock->msgl},
        {"Message Data", FIXEDFLD_STRING, &msgblock->msgl, msgblock->data}};

    // Message Block 초기화
    memset(msgblock, 0x00, sizeof(MSG_BLOCK));

    // 1. 'Message Length' Field Parsing
    retv = read_msg_buff(msgbuff, &fixedfld[0]);

    if (retv & MSG_BUFFER_SCARCED)
    {
        finish_msg_buff(msgbuff);
        return (MSG_BUFFER_SCARCED);
    }

    /* If the rest size of message buff is less than the value of 'Message Length' */
    if (msgbuff->rest_size < msgblock->msgl)
    {
        restore_msg_buff(msgbuff, UDP_DWN_MSGBLK_MSGLEN_LEN);
        return (MSG_BUFFER_SCARCED);
    }

    // 2. 'Message Data' Field Parsing
    retv = read_msg_buff(msgbuff, &fixedfld[1]);

    return (0);
}