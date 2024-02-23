/******************************************/
/* MoldUDP64 Protocol Specification V1.00 */
/******************************************/

#include "nassmt.h"

// Header
uint64_t UDP_DWN_HDR_SESSION_LEN = 10;
uint64_t UDP_DWN_HDR_SEQN_LEN = 8;
uint64_t UDP_DWN_HDR_MSGCNT_LEN = 2;

// Message Block
uint64_t UDP_DWN_MSGBLK_MSGLEN_LEN = 2;

/*
 * Function: parser_moldudp64_header()
 * ------------------------------------------
 * 'Packet Header' 파싱
 */
int parser_moldudp64_header(MSGBUF *msgbuff, PACKET_HEADER *packetheader)
{
    FIXEDFLD fixedfld[] = {
        {"Session", FIXEDFLD_UINT, &UDP_DWN_HDR_SESSION_LEN, &packetheader->session, 0},
        {"Sequence Number", FIXEDFLD_UINT, &UDP_DWN_HDR_SEQN_LEN, &packetheader->seqn, 0},
        {"Message Count", FIXEDFLD_UINT, &UDP_DWN_HDR_MSGCNT_LEN, &packetheader->msg_count, 0},
        {"", 0, NULL, NULL, -1}};

    // Packet Header 초기화
    memset(packetheader, 0x00, sizeof(PACKET_HEADER));

    return (proc_msg_buff(msgbuff, fixedfld));
}

/*
 * Function: parser_moldudp64_message_block()
 * ------------------------------------------
 * 'Message Block' 파싱
 */
int parser_moldudp64_message_block(MSGBUFF *msgbuff, MSG_BLOCK *msgblock)
{
    FIXEDFLD fixedfld[] = {
        {"Message Length", FIXEDFLD_UINT, &UDP_DWN_MSGBLK_MSGLEN_LEN, &msgblock->msgl, 0},
        {"Message Data", FIXEDFLD_STRING, &msgblock->msgl, msgblock->data, 0},
        {"", 0, NULL, NULL, -1}};

    // Message Block 초기화
    memset(msgblock, 0x00, sizeof(MSG_BLOCK));

    return (proc_msg_buff(msgbuff, fixedfld));
}

/*
 * Function: parser_moldudp64_dwn_packet()
 * ------------------------------------------
 * 'MoldUDP64 Downstream packet' 파싱
 */
int parser_moldudp64_dwn_packet(MSGBUFF *msgbuff, DWN_PACKET *dwnPacket, int flag)
{
    int retv = 0;

    swtich(flag)
    {
    case FLAG_PACKET_HEADER:
        retv = parser_moldudp64_header(msgbuff, &dwnPacket->header);
        break;
    case FLAG_MSG_BLOCK:
        retv = parser_moldudp64_message_block(msgbuff, &dwnPacket->msg_block);
        break;
    }

    return (retv);
}

/*
 * Function: set_moldudp64_req_packet()
 * ------------------------------------------
 * 'MoldUDP64 Request packet' 세팅
 */
PACKET_HEADER set_moldudp64_req_packet(uint64_t session, uint64_t seqn, uint64_t msg_count)
{
    PACKET_HEADER reqPacket;

    reqPacket.session = session;
    reqPacket.seqn = seqn;
    reqPacket.msg_count = msg_count;

    return (reqPacket);
}