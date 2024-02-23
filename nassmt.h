#ifndef _NASSMT_H_
#define _NASSMT_H_

#include "context.h"
#include <math.h>
#include <limits.h>

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
    int is_append;
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
// Packet Header
typedef struct
{
    uint64_t session;
    uint64_t seqn;
    uint64_t msg_count;
} PACKET_HEADER;

// Message Block
typedef struct
{
    size_t msgl;
    char data[MAX_BUFFER_SIZE];
} MSG_BLOCK;

// Downstream Packet
typedef struct
{
    PACKET_HEADER header;
    MSG_BLOCK *msg_blocks;
} DWN_PACKET;

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

// @ DECIMAL
typedef struct
{
    uint64_t denominator;
    double value;
} DECIMAL;

/***********************/
/* SmartOption Message */
/***********************/
// @ MAX_SIZE
#define MAX_SYMB_LEN 256

// Format Code
#define SHORT_FORM_DECIMAL_FORMAT_CODE 1
#define LONG_FORM_DECIMAL_FORMAT_CODE 2
#define EXTENDED_FORM_DECIMAL_FORMAT_CODE 3
#define BYTE_VALUE_FORMAT_CODE 7
#define SHORT_VALUE_FORMAT_CODE 8
#define INT32_VALUE_FORMAT_CODE 9
#define INT64_VALUE_FORMAT_CODE 10
#define STRING_FORMAT_CODE 15
#define DATE_FORMAT_CODE 16
#define BOOLEAN_FORMAT_CODE 21
#define CHAR_FORMAT_CODE 22

// Value Type Code
#define ROOT_SYMBOL_VALUE_TYPE 1
#define PUT_CALL_VALUE_TYPE 2
#define EXPIRATION_DATE_VALUE_TYPE 3
#define STRIKE_PRICE_VALUE_TYPE 4
#define HIGH_PRICE_VALUE_TYPE 65
#define LOW_PRICE_VALUE_TYPE 66
#define LAST_PRICE_VALUE_TYPE 67
#define OPEN_PRICE_VALUE_TYPE 68
#define TOTAL_VOLUME_VALUE_TYPE 70
#define NET_CHANGE_VALUE_TYPE 71
#define OPEN_INTEREST_VALUE_TYPE 72
#define TICK_VALUE_TYPE 73
#define BID_VALUE_TYPE 74
#define ASK_VALUE_TYPE 75
#define PARENT_SYMBOL_LOCATE_VALUE_TYPE 81
#define UNDERLYING_PRICE_VALUE_TYPE 100
#define UPSTREAM_CONDITION_DETAILS_VALUE_TYPE 143
#define TRADE_REPORT_DETAIL_VALUE_TYPE 145
#define EXTENDED_REPORT_FLAGS_VALUE_TYPE 146

typedef struct
{
    uint64_t element_length;
    uint64_t format_code;
    uint64_t value_type_code;
} AppendHeader;

// 1. System Event
#define SYSTEM_EVENT_MSG_TYPE 0x20

typedef struct
{
    uint64_t msgtype;
    uint64_t timestamp;
    char event_code[4 + 1];
} SystemEvent;

// 2. Channel Seconds
#define CHANNEL_SECONDS_MSG_TYPE 0x22

typedef struct
{
    uint64_t msgtype;
    uint64_t protocol_id;
    uint64_t channel_index;
    uint64_t seconds;
} ChannelSeconds;

// 3. Market Center Locate
#define MARKET_CENTER_LOCATE_MSG_TYPE 0x30

typedef struct
{
    uint64_t msgtype;
    uint64_t locate_code;
    char MIC[4 + 1];
} MarketCenterLocate;

// 4. Instrument Locate
#define INSTRUMENT_LOCATE_MSG_TYPE 0x33

#define UNKNOWN_PRODUCT 0
#define EQUITY_PRODUCT 1
#define OPTION_PRODUCT 2
#define INDEX_PRODUCT 3
#define WORLD_CURRENCY_PRODUCT 10
#define FIXED_INCOME_PRODUCT 12
#define FUTURES_CONTRACT_PRODUCT 14
#define OPTION_ROOT_PRODUCT 23

typedef struct
{
    uint64_t msgtype;
    uint64_t locate_code;
    char country_code[3 + 1];
    char currency_code[3 + 1];
    char MIC[4 + 1];
    uint64_t product_type;
    uint64_t symbol_length;
    char symbol[MAX_SYMB_LEN];

    /* Appendage List Information */
    char root[MAX_SYMB_LEN];     // Root Symbol(code = 1, string)
    char put_or_call[2];         // Put or Call(code = 2, char)
    uint64_t expiration_date;    // Expiration Date(code = 3, date)
    DECIMAL strike;              // Strike Price(code = 4, decimal)
    uint64_t parent_locate_code; // Parent Symbol Locate Code(code = 81, numeric)
} InstrumentLocate;

// **NBBO(5~10)
typedef struct
{
    uint64_t market_center_locate;
    DECIMAL price;
    uint64_t size;
    char side;
} NBBO_DEPTH;

typedef struct
{
    uint64_t instrument_locate;
    NBBO_DEPTH bid; // 매수
    NBBO_DEPTH ask; // 매도
    uint64_t condition;
    uint64_t flags;
    int nside;
} NBBO;

// 5. Two Sided NBBO Update - Short Form
#define SHORT_2_SIDED_NBBO_MSG_TYPE 0x60

// 6. Two Sided NBBO Update - Long Form
#define LONG_2_SIDED_NBBO_MSG_TYPE 0x61

// 7. Two Sided NBBO Update - Extended Form
#define EXTENDED_2_SIDED_NBBO_MSG_TYPE 0x62

// 8. One Sided NBBO Update - Short Form
#define SHORT_1_SIDED_NBBO_MSG_TYPE 0x63

// 9. One Sided NBBO Update - Long Form
#define LONG_1_SIDED_NBBO_MSG_TYPE 0x64

// 10. One Sided NBBO Update - Extended Form
#define EXTENDED_1_SIDED_NBBO_MSG_TYPE 0x65

// **TRADE(11~14)
typedef struct
{
    uint64_t instrument_locate;
    uint64_t market_center_locate;
    uint64_t trade_id;
    DECIMAL price;
    uint64_t size;
    uint64_t price_flags;
    uint64_t eligibility_flags;
    uint64_t report_flags;
    uint64_t change_flags;
    uint64_t cancel_flags;
    char trade_type;

    /* Appendage List Information */
    char upstream_condition_detail[MAX_BUFFER_SIZE];
    uint64_t trade_report_detail;
    uint64_t extended_report_flags;
} TRADE;

#define TRADE_TRADE_TYPE 'T'
#define CANCEL_TRADE_TYPE 'C'

// **Eligibility Flag
#define EL_LAST_QUOTE_PRICE_FLAG 0x08
#define EL_TOTAL_VOLUME_FLAG 0x10
#define EL_HIGH_LOW_PRICE_FLAG 0x20
#define EL_OPEN_PRICE_FLAG 0x40
#define EL_CLOSE_PRICE_FLAG 0x80

// **Change Flag
#define CF_MARKET_CENTER_VOLUME 0x02
#define CF_MARKET_CENTER_LAST_SALE_PRICE 0x08

// 11. Trade Short Form
#define SHORT_TRADE_MSG_TYPE 0x70

// 12. Trade Long Form
#define LONG_TRADE_MSG_TYPE 0x71

// 13. Trade Extended Form
#define EXTENDED_TRADE_MSG_TYPE 0x72

// 14. Trade Cancel
#define TRADE_CANCEL_MSG_TYPE 0x73

// 15. Instrument Value Update
#define VALUE_UPDATE_MSG_TYPE 0x80

typedef struct
{
    uint64_t instrument_locate;
    uint64_t market_center_locate;
    uint64_t value_update_flags;

    /* Appendage List Information */
    DECIMAL high;
    DECIMAL low;
    DECIMAL last;
    DECIMAL open;
    uint64_t total_volume;
    DECIMAL net_change;
    uint64_t open_interest;
    DECIMAL tick;
    DECIMAL bid;
    DECIMAL ask;
    DECIMAL underlying_price;
} VALUE_UPDATE;

// 16. Instrument Status
#define INSTRUMENT_STATUS_MSG_TYPE 0x90

typedef struct
{
    uint64_t instrument_locate;
    uint64_t market_center_locate;
    uint64_t status_type;
    uint64_t status_code;
    uint64_t reason_code;
    uint64_t status_flags;
    uint64_t reason_detail_length;
    char reason_detail[MAX_BUFFER_SIZE];
} INSTRUMENT_STATUS;

// 17. Channel Event
#define CHANNEL_EVENT_MSG_TYPE 0xB0

typedef struct
{
    char event_code[4 + 1];
    uint64_t market_center_locate;
} CHANNEL_EVENT;

// 18. Administrative Text
#define ADMIN_TEXT_MSG_TYPE 0xB2

typedef struct
{
    uint64_t text_length;
    char text[512];
} ADMINISTRATIVE_TEXT;

// **Market Data Standard Header
typedef struct
{
    uint64_t msgtype;
    uint64_t protocol_id;   // Source Protocol ID
    uint64_t channel_index; // Source Channel Index
    uint64_t message_flag;
    uint64_t upstream_seqn;
    uint64_t upstream_nanos;
} MarketDataHeader;

typedef struct
{
    MarketDataHeader header;
    NBBO nbbo;
    TRADE trade;
    VALUE_UPDATE value_update;
    INSTRUMENT_STATUS instrument_status;
    CHANNEL_EVENT channel_event;
    ADMINISTRATIVE_TEXT administrative_text;
} MakretData;

// 19. Instrument Meta Data
#define INSTRUMENT_META_DATA_MSG_TYPE 0xC0

typedef struct
{
    uint64_t msgtype;
    uint64_t locate_code;

    /* Appendage List Information */
    uint64_t parent_symbol_locate;
} METADATA;

// 20. Option Delivery Component
#define OPTION_DELIVERY_COMPONENT_MSG_TYPE 0xC3

typedef struct
{
    uint64_t msgtype;
    uint64_t root_code_locate;
    uint64_t component_index;
    uint64_t component_total;
    uint64_t deliverable_units;
    uint64_t settlement_method;
    uint64_t fixed_amount_denominator;
    uint64_t fixed_amount_numerator;
    char currency_code[3 + 1];
    DECIMAL strike_percent;
    uint64_t component_symbol_locate;
} OPTION_DELIVERY;

// *Miscellaneous and Refernce Data Message
typedef struct
{
    METADATA meta_data;
    OPTION_DELIVERY option_delivery;
} ReferenceData;

#define MAX_LOG_MSG 1024 * 16
#define SMT_NBBO_CLASS 0x01
#define SMT_TRADE_CLASS 0x02
#define SMT_DEFAULT_CLASS 0x04

// Total Table
typedef struct smartoption_table
{
    FEP *fep;
    TOKEN *token;
    int (*proc)(struct smartoption_table *);
    char name[256];
    int loglevel;
    int logflag;
    uint64_t type;
    int class;
    char loghead[MAX_LOG_MSG];
    char logmsg[MAX_LOG_MSG];
    char raw_data[MAX_LOG_MSG];
    uint64_t raw_data_l;
    SystemEvent system_event;                // 0x20
    ChannelSeconds channel_seconds;          // 0x22
    MarketCenterLocate market_center_locate; // 0x30
    InstrumentLocate instrument_locate;      // 0x33
    MakretData market_data;
    ReferenceData reference;
} SMARTOPTION_TABLE;
#define SMARTOPTION_TABLE_SIZE sizeof(SMARTOPTION_TABLE)

/***********************************/
/* Instrument Locate shared memory */
/***********************************/
#define MAX_CHANNEL 1024
#define MAX_MARKET_CENTER 5000
#define MAX_TRADE_ID 5000
typedef struct
{
    unsigned char *bit;
    int *inst_size;
    InstrumentLocate *inst_list;
    int *cs_size;
    ChannelSeconds *cs_list;
    int *mcl_size;
    MarketCenterLocate *mcl_list;
    int *trade_size;
    TRADE *trade_list;
} SHM_SMART;

/************/
/* smtfep.c */
/************/
MDFOLD *smtfold(FEP *fep, uint64_t locate_code);
int smt_push(FEP *fep, MDFOLD *folder, char *check);

/*****************/
/* smartoption.c */
/*****************/
int smt_decode(SMARTOPTION_TABLE *smt_table);

/************/
/* smtshm.c */
/************/
int initsmt(FEP *fep, int key, int clr);

InstrumentLocate *createInst(SMARTOPTION_TABLE *smt_table);
InstrumentLocate *updateInst(SMARTOPTION_TABLE *smt_table);
InstrumentLocate *readInst(SMARTOPTION_TABLE *smt_table, uint64_t locate_code);
InstrumentLocate *findParent(SMARTOPTION_TABLE *smt_table, uint64_t product_type);
int deleteInst(SMARTOPTION_TABLE *smt_table, uint64_t locate_code);

ChannelSeconds *createCs(SMARTOPTION_TABLE *smt_table);
ChannelSeconds *readCs(SMARTOPTION_TABLE *smt_table, uint64_t protocol_id, uint64_t channel_index);
ChannelSeconds *updateCs(SMARTOPTION_TABLE *smt_table);
int deleteCs(SMARTOPTION_TABLE *smt_table, uint64_t protocol_id, uint64_t channel_index);

MarketCenterLocate *createMcl(SMARTOPTION_TABLE *smt_table);
MarketCenterLocate *readMcl(SMARTOPTION_TABLE *smt_table, uint64_t locate_code);
MarketCenterLocate *updateMcl(SMARTOPTION_TABLE *smt_table);
int deleteMcl(SMARTOPTION_TABLE *smt_table, uint64_t locate_code);

TRADE *createTrade(SMARTOPTION_TABLE *smt_table);
TRADE *readTrade(SMARTOPTION_TABLE *smt_table, uint64_t locate_code, uint64_t trade_id);
TRADE *updateTrade(SMARTOPTION_TABLE *smt_table);
int deleteTrade(SMARTOPTION_TABLE *smt_table, uint64_t locate_code, uint64_t trade_id);

/****************/
/* smt function */
/****************/
int smt_0x22(SMARTOPTION_TABLE *smt_table);  // Channel Seconds
int smt_0x30(SMARTOPTION_TABLE *smt_table);  // Market Center Locate
int smt_0x33(SMARTOPTION_TABLE *smt_table);  // Instrument Locate
int smt_trade(SMARTOPTION_TABLE *smt_table); // Trade Message
int smt_nbbo(SMARTOPTION_TABLE *smt_table);  // NBBO Message

/*************/
/* smttool.c */
/*************/
// Convert Function
void convert_big_endian_to_uint64_t(char *from, uint64_t *to, size_t size);
void convert_big_endian_to_int64_t(char *from, int64_t *to, size_t size);
void convert_nanosec_to_time_t(uint64_t *from, time_t *to);
int getweek(int date);
int getwday(int date);
time_t gettime_from_mid_sec(uint64_t seconds_from_midnight);

// Message Buffer
int read_msg_buff(MSGBUFF *msgbuff, FIXEDFLD *fixedfld);
int finish_msg_buff(MSGBUFF *msgbuff);
int restore_msg_buff(MSGBUFF *msgbuff, size_t size);
void initialize_msg_buff(MSGBUFF *msgbuff);

// TR Packet
int allocate_tr_packet(TR_PACKET *tr_packet, char *message_data, size_t message_length);
void initialize_tr_packet(TR_PACKET *tr_packet);
void initialize_smart(SMARTOPTION_TABLE *smt_table, FEP *fep, TOKEN *token);

// msgb -> fixedfld
int msg2fixedfld(FIXEDFLD *fixedfld, char *msgb, int offset);
void decode_fixedfld(FIXEDFLD *fixedfld, char *msgb, int *offset, int fldnum);
void decode_fixedfld_all(FIXEDFLD *fixedfld, char *msgb, int *offset);

// Logger
void nas_smt_log(FEP *fep, SMARTOPTION_TABLE *smt_table, const char *format, ...);
void nas_smt_csv(FEP *fep, SMARTOPTION_TABLE *smt_table);

/*************/
/* moldudp.c */
/*************/
int parser_moldudp64_message_block(MSGBUFF *msgbuff, MSG_BLOCK *msgblock);

/*************/
/* symbcnv.c */
/*************/
int _convert_symbol(FEP *fep, char *corise_symbol, InstrumentLocate *inst);

#endif
