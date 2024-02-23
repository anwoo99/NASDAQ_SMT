/*******************************************/
/* Mold UDP64 Protocol Specification V1.00 */
/*******************************************/

#include "nassmt.h"

// Header
uint64_t UDP_DWN_HDR_SESSION_LEN = 10;
uint64_t UDP_DWN_HDR_SEQN_LEN = 8;
uint64_t UDP_DWN_HDR_MSGCNT_LEN = 2;

// Message Block
uint64_t UDP_DWN_MSGBLK_MSGLEN_LEN = 2;

/*
 * Function: parser_moldudp64_message_block()
 * ------------------------------------------
 * 'Message Block' 파싱
 */
int parser_moldudp64_message_block(MSGBUFF *msgbuff, MSG_BLOCK *msgblock)
{
    int retv = 0;
    FIXEDFLD fixedfld[] = {
        {"Message Length", FIXEDFLD_UINT, &UDP_DWN_MSGBLK_MSGLEN_LEN, &msgblock->msgl, 0},
        {"Message Data", FIXEDFLD_STRING, &msgblock->msgl, msgblock->data, 0}};

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

int parser_moldudp64_header(MSGBUF *msgbuff, PACKET_HEADER *packetheader)
{
    int retv = 0;
    FIXEDFLD fixedfld[] = {
        {"Session", FIXEDFLD_UINT, &UDP_DWN_HDR_SESSION_LEN, &packetheader->session, 0},
        {"Sequence Number", FIXEDFLD_UINT, &UDP_DWN_HDR_SEQN_LEN, &packetheader->seqn, 0},
        {"Message Count", FIXEDFLD_UINT, &UDP_DWN_HDR_MSGCNT_LEN, &packetheader->msg_count, 0}};

    return (0);
}

int parser_moldudp64_dwn_packet(MSGBUFF *msgbuff, DWN_PACKET *dwnpacket)
{
    int retv = 0;
    FIXEDFLD fixedfld[] = {

    };

    return (0);
}