#include "nastag.h"

/* Moldudp64 Downstream Packet Message Block Parser */
int parser_moldudp64_message_block(MSGBUFF *msgbuff, MSG_BLOCK *msgblock)
{
    int retv = 0;
    uint64_t msgl;

    // Message Block 초기화
    memset(msgblock, 0x00, sizeof(MSG_BLOCK));

    // 1. 'Message Length' Field Parsing
    retv = uint64_t_read_msg_buff(msgbuff, &msgl, UDP_DWN_MSGBLK_MSGLEN_LEN);

    if (retv & MSG_BUFFER_SCARCED)
    {
        finish_read_msg_buff(msgbuff);
        return (MSG_BUFFER_SCARCED);
    }

    msgblock->msgl = msgl;

    /* If the rest size of message buff is less than the value of 'Message Length' */
    if (msgbuff->rest_size < msgl)
    {
        restore_msg_buff(msgbuff, UDP_DWN_MSGBLK_MSGLEN_LEN);
        return (MSG_BUFFER_SCARCED);
    }

    // 2. 'Message Data' Field Parsing
    retv = copy_read_msg_buff(msgbuff, msgblock->data, msgl);

    return (0);
}