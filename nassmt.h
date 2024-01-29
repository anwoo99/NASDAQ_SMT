#ifndef _NASTAG_H_
#define _NASTAG_H_

#include "context.h"
#include <math.h>

/************************************/
/* Message Buffer Defined by Corise */
/************************************/
#define MAX_BUFFER_SIZE (1024 * 16)
#define MSG_INVALID_RESTORE_SIZE 0x0004 // 0000 0000 0000 0100
#define MSG_BUFFER_SCARCED 0x0008       // 0000 0000 0000 1000

typedef struct
{
    char buffer[MAX_BUFFER_SIZE];
    int offset;
    size_t read_size;
    size_t rest_size;
    size_t msgl;
} MSGBUFF;

/*******************************/
/* FIXEDFLD for NASDAQ message */
/*******************************/

typedef struct
{
    char field_name[MAX_BUFFER_SIZE];
    int field_type;
    uint64_t *field_length;
    void *value;
} FIXEDFLD;

#define MAX_FIXEDFLD_SIZE 1024
#define FIXEDFLD_UINT 0x01
#define FIXEDFLD_INT 0x02
#define FIXEDFLD_DECIMAL 0x04
#define FIXEDFLD_STRING 0x08
#define FIXEDFLD_CHAR 0x10
#define FIXEDFLD_BITMASK 0x20

/********************************/
/* TR Message Defined by Corise */
/********************************/
typedef struct
{
    uint32_t seqn;
    uint64_t type;
} TR_HEADER;

typedef struct
{
    TR_HEADER header;
    char pkt_buff[MAX_BUFFER_SIZE];
    size_t pkt_l;
} TR_PACKET;
#define TR_PACKET_LEN sizeof(TR_PACKET)

/*******************************/
/* MoldUDP64 Downstream Packet */
/*******************************/
// 1. Header
#define UDP_DWN_HDR_SESSION_LEN 10
#define UDP_DWN_HDR_SEQN_LEN 8
#define UDP_DWN_HDR_MSGCNT_LEN 2

// 2. Message Block
typedef struct
{
    size_t msgl;
    char data[MAX_BUFFER_SIZE];
} MSG_BLOCK;

// 3. Heartbeats
#define UDP_DWN_HRTBT_MSGCNT_VALUE 0x0000

// 4. End of Session
#define UDP_DWN_EOS_MSGCNT_VALUE 0xFFFF

/****************************/
/* MoldUDP64 Request Packet */
/****************************/
#define UDP_REQ_SESSION_LEN 10
#define UDP_REQ_SEQN_LEN 8
#define UDP_REQ_REQMSGCNT_LEN 2

/*************/
/* nastool.c */
/*************/
// Convert Function
void convert_big_endian_to_uint64_t(char *from, uint64_t *to, size_t size);
void convert_big_endian_to_int64_t(char *from, int64_t *to, size_t size);
void convert_nanosec_to_time_t(uint64_t *from, time_t *to);

// Message Buffer
int read_msg_buff(MSGBUFF *msgbuff, FIXEDFLD *fixedfld);
int finish_msg_buff(MSGBUFF *msgbuff);
int restore_msg_buff(MSGBUFF *msgbuff, size_t size);
void initialize_msg_buff(MSGBUFF *msgbuff);

// TR Packet
int allocate_tr_packet(TR_PACKET *tr_packet, char *message_data, size_t message_length);
void initialize_tr_packet(TR_PACKET *tr_packet);

// msgb -> fixedfld
int msg2fixedfld(FIXEDFLD *fixedfld, char *msgb, int offset);
void decode_fixedfld(FIXEDFLD *fixedfld, char *msgb, int *offset, int fldnum);
void decode_fixedfld_all(FIXEDFLD *fixedfld, char *msgb, int *offset);

// Logger
void nas_raw_log(FEP *fep, int level, int flag, char *msgb, int msgl, const char *format, ...);
void nas_raw_csv(FEP *fep, int level, int type, char *header, char *message);

/*************/
/* moldudp.c */
/*************/
int parser_moldudp64_message_block(MSGBUFF *msgbuff, MSG_BLOCK *msgblock);

/*****************/
/* smartoption.c */
/*****************/
int smt_decode(SMARTOPTION_TABLE *smt_table, uint64_t msgtype, char *msgb, size_t msgl);

#endif
