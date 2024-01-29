#ifndef _SMATFLD_H_
#define _SMTFLD_H_

#include "nassmt.h"

// @ DECIMAL
typedef struct
{
    uint64_t denominator;
    double value;
} DECIMAL;

/****************/
/* Field Length */
/****************/
// 0. Appendage List
uint64_t APPEND_ELEMENT_LENGTH_LEN = 1;
uint64_t APPEND_FORMATCODE_LEN = 1;
uint64_t APPEND_VALUETYPECODE_LEN = 1;

// 1. System Event
uint64_t SYSTEM_EVENT_MSG_TYPE_LEN = 1;
uint64_t SYSTEM_EVENT_TIMESTAMP_LEN = 8;
uint64_t SYSTEM_EVENT_EVENTCODE_LEN = 4;

// 2. Channel Seconds
uint64_t CHANNEL_SECONDS_MSG_TYPE_LEN = 1;
uint64_t CHANNEL_SECONDS_PROTOCOL_ID_LEN = 1;
uint64_t CHANNEL_SECONDS_CHANNEL_INDEX_LEN = 1;
uint64_t CHANNEL_SECONDS_SECONDS_LEN = 4;

// 3. Market Center Locate
uint64_t MARKET_CENTER_LOCATE_MSG_TYPE_LEN = 1;
uint64_t MARKET_CENTER_LOCATE_LOCATECODE_LEN = 2;
uint64_t MARKET_CENTER_LOCATE_MIC_LEN = 4;

// 4. Instrument Locate
uint64_t INSTRUMENT_LOCATE_MSG_TYPE_LEN = 1;
uint64_t INSTRUMENT_LOCATE_LOCATECODE_LEN = 4;
uint64_t INSTRUMENT_LOCATE_COUNTRYCODE_LEN = 2;
uint64_t INSTRUMENT_LOCATE_CURRENCYCODE_LEN = 3;
uint64_t INSTRUMENT_LOCATE_MIC_LEN = 4;
uint64_t INSTRUMENT_LOCATE_PRODUCTTYPE_LEN = 1;
uint64_t INSTRUMENT_LOCATE_SYMBOLLENGTH_LEN = 1;

// 5. NBBO
uint64_t NBBO_INSTRUMENT_LOCATE_LEN = 4;
uint64_t NBBO_DEPTH_MARKET_CENTER_LEN = 2;
uint64_t NBBO_DEPTH_DENOMINATOR_LEN = 1;
uint64_t NBBO_FLAGS_LEN = 1;
uint64_t NBBO_SIDE_LEN = 1;
uint64_t NBBO_RFU_LEN = 1;

// 6. Trade
uint64_t TRADE_INSTRUMENT_LOCATE_LEN = 4;
uint64_t TRADE_MARKET_CENTER_LOCATE_LEN = 2;
uint64_t TRADE_TRADE_ID_LEN = 4;
uint64_t TRADE_PRICE_FLAGS_LEN = 1;
uint64_t TRADE_ELIGIBILITY_FLAGS_LEN = 1;
uint64_t TRADE_REPORT_FLAGS_LEN = 2;
uint64_t TRADE_CHANGE_FLAGS_LEN = 1;
uint64_t TRADE_CANCEL_FLAGS_LEN = 1;
uint64_t TRADE_DENOMINATOR_LEN = 1;

// 7. Instrument Value Update
uint64_t VALUE_UPDATE_INSTRUMENT_LOCATE_LEN = 4;
uint64_t VALUE_UPDATE_MARKET_CENTER_LOCATE_LEN = 2;
uint64_t VALUE_UPDATE_VALUE_UPDATE_FLAGS_LEN = 4;

// 8. Instrument Status
uint64_t INSTRUMENT_STATUS_INSTRUMENT_LOCATE_LEN = 4;
uint64_t INSTRUMENT_STATUS_MARKET_CENTER_LOCATE_LEN = 2;
uint64_t INSTRUMENT_STATUS_STATUS_TYPE_LEN = 1;
uint64_t INSTRUMENT_STATUS_STATUS_CODE_LEN = 1;
uint64_t INSTRUMENT_STATUS_REASON_CODE_LEN = 1;
uint64_t INSTRUMENT_STATUS_STATUS_FLAGS_LEN = 1;
uint64_t INSTRUMENT_STATUS_REASON_DETAIL_LENGTH_LEN = 1;

// 9. Channel Event
uint64_t CHANNEL_EVENT_INSTRUMENT_LOCATE_LEN = 4;
uint64_t CHANNEL_EVENT_MARKET_CENTER_LOCATE_LEN = 2;

// 10. Administrative Text
uint64_t ADMIN_TEXT_TEXT_LENGTH_LEN = 2;

// 10. Market Header
uint64_t MARKET_HEADER_MSG_TYPE_LEN = 1;
uint64_t MARKET_HEADER_PROTOCOLID_LEN = 1;
uint64_t MARKET_HEADER_CHANNEL_INDEX_LEN = 1;
uint64_t MARKET_HEADER_MESSAGE_FLAG_LEN = 1;
uint64_t MARKET_HEADER_UPSTREAM_SEQN_LEN = 4;
uint64_t MARKET_HEADER_UPSTREAM_NANOS_LEN = 4;
uint64_t MARKET_HEADER_TOTAL_LEN = 12;

// 11. Instruement Meta Data
uint64_t INSTRUMENT_META_DATA_MSG_TYPE_LEN = 1;
uint64_t INSTRUMENT_META_DATA_LOCATE_CODE_LEN = 4;

// 12. Option Delivery Component
uint64_t OPTION_DELIVERY_COMPONENT_MSG_TYPE_LEN = 1;
uint64_t OPTION_DELIVERY_COMPONENT_ROOT_CODE_LOCATE_LEN = 4;
uint64_t OPTION_DELIVERY_COMPONENT_COMPONENT_INDEX_LEN = 4;
uint64_t OPTION_DELIVERY_COMPONENT_COMPONENT_TOTAL_LEN = 4;
uint64_t OPTION_DELIVERY_COMPONENT_DELIVERABLE_UNITS_LEN = 4;
uint64_t OPTION_DELIVERY_COMPONENT_SETTLEMENT_METHOD_LEN = 1;
uint64_t OPTION_DELIVERY_COMPONENT_FIXED_AMOUNT_DENOMINATOR_LEN = 1;
uint64_t OPTION_DELIVERY_COMPONENT_FIXED_AMOUNT_NUMERATOR_LEN = 8;
uint64_t OPTION_DELIVERY_COMPONENT_CURRENCY_CODE_LEN = 3;
uint64_t OPTION_DELIVERY_COMPONENT_STRIKE_PERCENT_LEN = 2;
uint64_t OPTION_DELIVERY_COMPONENT_COMPONENT_SYMBOL_LOCATE_LEN = 4;

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
    char instrument_locate[4 + 1];
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

#define MAX_LOG_MSG 1024 * 8
// Total Table
typedef struct
{
    char name[256];
    int loglevel;
    int logflag;
    int (*proc)(FEP *, TOKEN *, FIXFLD *, int, int);
    uint64_t type;
    char loghead[MAX_LOG_MSG];
    char logmsg[MAX_LOG_MSG];
    SystemEvent system_event;                // 0x20
    ChannelSeconds channel_seconds;          // 0x22
    MarketCenterLocate market_center_locate; // 0x30
    InstrumentLocate instrument_locate;      // 0x33
    MakretData market_data;
    ReferenceData reference;
} SMARTOPTION_TABLE;
#define SMARTOPTION_TABLE_SIZE sizeof(SMARTOPTION_TABLE)

#endif