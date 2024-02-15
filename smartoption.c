#include "nassmt.h"

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
uint64_t CHANNEL_EVENT_EVENT_CODE_LEN = 4;
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

// 13. Default Len
uint64_t DEFAULT_LEN_1 = 1;
uint64_t DEFAULT_LEN_2 = 2;
uint64_t DEFAULT_LEN_3 = 3;
uint64_t DEFAULT_LEN_4 = 4;

/*
 * Function: _smt_csv()
 * --------------------
 * FIXEDFLD -> CSV
 */
int _smt_csv(SMARTOPTION_TABLE *smt_table, FIXEDFLD *fixedfld)
{
    int ii = 0;
    int head_l, msg_l;

    for (ii = 0; strlen(fixedfld[ii].field_name) > 0; ii++)
    {
        head_l = strlen(smt_table->loghead);

        sprintf(&smt_table->loghead[head_l], "%s,", fixedfld[ii].field_name);

        msg_l = strlen(smt_table->logmsg);

        switch (fixedfld[ii].field_type)
        {
        case FIXEDFLD_UINT:
            sprintf(&smt_table->logmsg[msg_l], "%lu,", *((uint64_t *)(fixedfld[ii].value)));
            break;
        case FIXEDFLD_INT:
            sprintf(&smt_table->logmsg[msg_l], "%ld,", *((int64_t *)(fixedfld[ii].value)));
            break;
        case FIXEDFLD_DECIMAL:
            sprintf(&smt_table->logmsg[msg_l], "%.*f,", (int)((DECIMAL *)(fixedfld[ii].value))->denominator, ((DECIMAL *)(fixedfld[ii].value))->value);
            break;
        case FIXEDFLD_STRING:
            sprintf(&smt_table->logmsg[msg_l], "%s,", (char *)(fixedfld[ii].value));
            break;
        case FIXEDFLD_CHAR:
            sprintf(&smt_table->logmsg[msg_l], "%c,", *(char *)(fixedfld[ii].value));
            break;
        case FIXEDFLD_BITMASK:
            sprintf(&smt_table->logmsg[msg_l], "0x%0*lX,", *(int *)(fixedfld[ii].field_length) * 2, *((uint64_t *)(fixedfld[ii].value)));
            break;
        default:
            strcpy(&smt_table->logmsg[msg_l], ",");
            break;
        }
    }

    return (0);
}

int get_append_header(AppendHeader *header, char *msgb)
{
    int offset = 0;

    // 1. Element Length
    convert_big_endian_to_uint64_t(&msgb[offset], &header->element_length, APPEND_ELEMENT_LENGTH_LEN);
    offset += APPEND_ELEMENT_LENGTH_LEN;

    // 2. Format Code
    convert_big_endian_to_uint64_t(&msgb[offset], &header->format_code, APPEND_FORMATCODE_LEN);
    offset += APPEND_FORMATCODE_LEN;

    // 3. Value Type Code
    convert_big_endian_to_uint64_t(&msgb[offset], &header->value_type_code, APPEND_VALUETYPECODE_LEN);

    return (0);
}

int get_append_decimal(AppendHeader *header, char *msgb, DECIMAL *value)
{
    int offset = 0;
    int64_t decimal;
    int value_size;
    int denominator_len;

    switch (header->format_code)
    {
    case SHORT_FORM_DECIMAL_FORMAT_CODE:
        value->denominator = 2;
        value_size = header->element_length;
        break;
    case LONG_FORM_DECIMAL_FORMAT_CODE:
        value->denominator = 4;
        value_size = header->element_length;
        break;
    case EXTENDED_FORM_DECIMAL_FORMAT_CODE:
        // Value Denomiator Code
        denominator_len = 1;
        convert_big_endian_to_uint64_t(&msgb[offset], &value->denominator, denominator_len);
        offset += denominator_len;
        value_size = header->element_length - denominator_len;
        break;
    default:
        return (-1);
    }

    // Value
    convert_big_endian_to_int64_t(&msgb[offset], &decimal, value_size);
    value->value = (double)decimal / (pow(10, value->denominator));

    return (0);
}

int get_append_numeric(AppendHeader *header, char *msgb, int64_t *valueInt, uint64_t *valueUint)
{
    int value_size;

    switch (header->format_code)
    {
    case BYTE_VALUE_FORMAT_CODE:
    case SHORT_VALUE_FORMAT_CODE:
    case INT32_VALUE_FORMAT_CODE:
    case INT64_VALUE_FORMAT_CODE:
        break;
    default:
        return (-1);
    }

    value_size = header->element_length;

    // Value
    if (valueInt)
        convert_big_endian_to_int64_t(msgb, valueInt, value_size);
    if (valueUint)
        convert_big_endian_to_uint64_t(msgb, valueUint, value_size);

    return (0);
}

int get_append_date(AppendHeader *header, char *msgb, uint64_t *date)
{
    uint64_t month, mday, year;
    int offset = 0;

    if (header->format_code != DATE_FORMAT_CODE)
        return (-1);

    // 1. Month
    int month_size = 1;
    convert_big_endian_to_uint64_t(&msgb[offset], &month, month_size);
    offset += month_size;

    // 2. MDay
    int mday_size = 1;
    convert_big_endian_to_uint64_t(&msgb[offset], &mday, mday_size);
    offset += mday_size;

    // 3. Year
    int year_size = 2;
    convert_big_endian_to_uint64_t(&msgb[offset], &year, year_size);
    offset += year_size;

    // Make Date
    *date = YMD(year, month, mday);

    return (0);
}

int get_append_string(AppendHeader *header, char *msgb, char *string)
{
    switch (header->format_code)
    {
    case STRING_FORMAT_CODE:
    case CHAR_FORMAT_CODE:
        break;
    default:
        return (-1);
    }

    memcpy(string, msgb, header->element_length);

    return (0);
}

/*
 * Function: smt_appendage_decode()
 * -------------------------------
 * Appendage List 파싱
 *
 * Using:
 * get_append_header(), get_append_decimal(),
 * get_append_numeric(), get_append_date(),
 * get_append_string()
 */
int smt_appendage_decode(SMARTOPTION_TABLE *smt_table, int *offset)
{
    InstrumentLocate *instrumentLocate = &smt_table->instrument_locate;
    TRADE *trade = &smt_table->market_data.trade;
    VALUE_UPDATE *valueUpdate = &smt_table->market_data.value_update;
    METADATA *meta = &smt_table->reference.meta_data;
    AppendHeader header;

    while (*offset < (smt_table->raw_data_l - 1))
    {
        memset(&header, 0x00, sizeof(AppendHeader));

        // 1. Header
        get_append_header(&header, &smt_table->raw_data[*offset]);
        *offset += (APPEND_ELEMENT_LENGTH_LEN + APPEND_FORMATCODE_LEN + APPEND_VALUETYPECODE_LEN);

        // 2. Element
        switch (header.value_type_code)
        {
        case ROOT_SYMBOL_VALUE_TYPE: // 1 / String / Instrument Locate
            get_append_string(&header, &smt_table->raw_data[*offset], instrumentLocate->root);
            break;
        case PUT_CALL_VALUE_TYPE: // 2 / Char / Instrument Locate
            get_append_string(&header, &smt_table->raw_data[*offset], instrumentLocate->put_or_call);
            break;
        case EXPIRATION_DATE_VALUE_TYPE: // 3 / Date / Instrument Locate
            get_append_date(&header, &smt_table->raw_data[*offset], &instrumentLocate->expiration_date);
            break;
        case STRIKE_PRICE_VALUE_TYPE: // 4 / Decimal / Insturment Locate
            get_append_decimal(&header, &smt_table->raw_data[*offset], &instrumentLocate->strike);
            break;
        case HIGH_PRICE_VALUE_TYPE: // 65 / Decimal / Value Update
            get_append_decimal(&header, &smt_table->raw_data[*offset], &valueUpdate->high);
            break;
        case LOW_PRICE_VALUE_TYPE: // 66 / Decimal / Value Update
            get_append_decimal(&header, &smt_table->raw_data[*offset], &valueUpdate->low);
            break;
        case LAST_PRICE_VALUE_TYPE: // 67 / Decimal / Value Update
            get_append_decimal(&header, &smt_table->raw_data[*offset], &valueUpdate->last);
            break;
        case OPEN_PRICE_VALUE_TYPE: // 68 / Decimal / Value Update
            get_append_decimal(&header, &smt_table->raw_data[*offset], &valueUpdate->open);
            break;
        case TOTAL_VOLUME_VALUE_TYPE: // 70 / Numeric / Value Update
            get_append_numeric(&header, &smt_table->raw_data[*offset], NULL, &valueUpdate->total_volume);
            break;
        case NET_CHANGE_VALUE_TYPE: // 71 / Decimal / Value Update
            get_append_decimal(&header, &smt_table->raw_data[*offset], &valueUpdate->net_change);
            break;
        case OPEN_INTEREST_VALUE_TYPE: // 72 / Numeric / Value Update
            get_append_numeric(&header, &smt_table->raw_data[*offset], NULL, &valueUpdate->open_interest);
            break;
        case TICK_VALUE_TYPE: // 73 / Decimal / Value Update
            get_append_decimal(&header, &smt_table->raw_data[*offset], &valueUpdate->tick);
            break;
        case BID_VALUE_TYPE: // 74 / Decimal / Value Update
            get_append_decimal(&header, &smt_table->raw_data[*offset], &valueUpdate->bid);
            break;
        case ASK_VALUE_TYPE: // 75 / Decimal / Value Update
            get_append_decimal(&header, &smt_table->raw_data[*offset], &valueUpdate->ask);
            break;
        case PARENT_SYMBOL_LOCATE_VALUE_TYPE: //  81 / Numeric / Instrument Locate
            get_append_numeric(&header, &smt_table->raw_data[*offset], NULL, &instrumentLocate->parent_locate_code);
            get_append_numeric(&header, &smt_table->raw_data[*offset], NULL, &meta->parent_symbol_locate);
            break;
        case UNDERLYING_PRICE_VALUE_TYPE: // 100 / Decimal / Value Update
            get_append_decimal(&header, &smt_table->raw_data[*offset], &valueUpdate->underlying_price);
            break;
        case UPSTREAM_CONDITION_DETAILS_VALUE_TYPE: // 143 / String / Trades, Trade Cancel
            get_append_string(&header, &smt_table->raw_data[*offset], trade->upstream_condition_detail);
            break;
        case TRADE_REPORT_DETAIL_VALUE_TYPE: // 145 / Byte Enum / Trades, Trade Cancel
            get_append_numeric(&header, &smt_table->raw_data[*offset], NULL, &trade->trade_report_detail);
            break;
        case EXTENDED_REPORT_FLAGS_VALUE_TYPE: // 146 / Numeric / Trades, Trade Cancel
            get_append_numeric(&header, &smt_table->raw_data[*offset], NULL, &trade->extended_report_flags);
            break;
        default:
            break;
        }
        *offset += header.element_length;
    }

    return (0);
}

/*
 * Function: smt_market_header_decode()
 * -------------------------------------
 * Market Data Header 파싱(FIXEDFLD 기반)
 */
int smt_market_header_decode(SMARTOPTION_TABLE *smt_table)
{
    MarketDataHeader *header = &smt_table->market_data.header;
    int offset = 0;
    FIXEDFLD fixedfld[] = {
        {"Message Type", FIXEDFLD_BITMASK, &MARKET_HEADER_MSG_TYPE_LEN, (void *)&header->msgtype, 0},
        {"Protocol ID", FIXEDFLD_UINT, &MARKET_HEADER_PROTOCOLID_LEN, (void *)&header->protocol_id, 0},
        {"Channel Index", FIXEDFLD_UINT, &MARKET_HEADER_CHANNEL_INDEX_LEN, (void *)&header->channel_index, 0},
        {"Message Flags", FIXEDFLD_BITMASK, &MARKET_HEADER_CHANNEL_INDEX_LEN, (void *)&header->message_flag, 0},
        {"Upstream Sequence Number", FIXEDFLD_UINT, &MARKET_HEADER_UPSTREAM_SEQN_LEN, (void *)&header->upstream_seqn, 0},
        {"Upstream Nanos", FIXEDFLD_UINT, &MARKET_HEADER_UPSTREAM_NANOS_LEN, (void *)&header->upstream_nanos, 0},
        {"", 0, NULL, NULL, -1}};

    // Decode fields
    decode_fixedfld_all(fixedfld, smt_table->raw_data, &offset);

    // # Log Message(CSV)
    _smt_csv(smt_table, fixedfld);

    return (0);
}

/*
 * Function: smt_nbbo_decode()
 * ------------------------------
 * Market:NBBO 파싱(FIXEDFLD 기반)
 */
int smt_nbbo_decode(SMARTOPTION_TABLE *smt_table)
{
    NBBO *nbbo = &smt_table->market_data.nbbo;
    int offset = 0;
    int ii = 0;
    uint64_t price_size = 0, size_size = 0;
    NBBO_DEPTH depth;
    int64_t price;
    FIXEDFLD fixedfld[] = {
        {"Instrument Locate", FIXEDFLD_UINT, &NBBO_INSTRUMENT_LOCATE_LEN, (void *)&nbbo->instrument_locate, 0},
        {"Bid Market Center Locate", FIXEDFLD_UINT, &NBBO_DEPTH_MARKET_CENTER_LEN, (void *)&nbbo->bid.market_center_locate, 0},
        {"Bid Denominator", FIXEDFLD_UINT, &NBBO_DEPTH_DENOMINATOR_LEN, (void *)&nbbo->bid.price.denominator, 0},
        {"Bid Price", FIXEDFLD_DECIMAL, &price_size, (void *)&nbbo->bid.price, 0},
        {"Bid Size", FIXEDFLD_UINT, &size_size, (void *)&nbbo->bid.size, 0},
        {"Ask Market Center Locate", FIXEDFLD_UINT, &NBBO_DEPTH_MARKET_CENTER_LEN, (void *)&nbbo->ask.market_center_locate, 0},
        {"Ask Denominator", FIXEDFLD_UINT, &NBBO_DEPTH_DENOMINATOR_LEN, (void *)&nbbo->ask.price.denominator, 0},
        {"Ask Price", FIXEDFLD_DECIMAL, &price_size, (void *)&nbbo->ask.price, 0},
        {"Ask Size", FIXEDFLD_UINT, &size_size, (void *)&nbbo->ask.size, 0},
        {"RFU/Condition", FIXEDFLD_BITMASK, &NBBO_RFU_LEN, (void *)&nbbo->condition, 0},
        {"Flags", FIXEDFLD_BITMASK, &NBBO_FLAGS_LEN, (void *)&nbbo->flags, 0},
        {"", 0, NULL, NULL, -1}};

    smt_table->class = SMT_NBBO_CLASS;

    // 1. Market Data Standard Header
    smt_market_header_decode(smt_table);
    offset += MARKET_HEADER_TOTAL_LEN;

    // 2. Instrument Locate
    decode_fixedfld(fixedfld, smt_table->raw_data, &offset, 0);

    // 3. DEPTH
    for (ii = 0; ii < nbbo->nside; ii++)
    {
        memset(&depth, 0x00, sizeof(NBBO_DEPTH));
        depth.side = (ii == 0) ? 'B' : 'S';

        // 3-1. Market Center Locate
        convert_big_endian_to_uint64_t(&smt_table->raw_data[offset], &depth.market_center_locate, NBBO_DEPTH_MARKET_CENTER_LEN);
        offset += NBBO_DEPTH_MARKET_CENTER_LEN;

        switch (smt_table->type)
        {
        case SHORT_2_SIDED_NBBO_MSG_TYPE:
        case SHORT_1_SIDED_NBBO_MSG_TYPE:
            depth.price.denominator = 2;
            price_size = 2;
            size_size = 2;
            break;
        case LONG_2_SIDED_NBBO_MSG_TYPE:
        case LONG_1_SIDED_NBBO_MSG_TYPE:
            depth.price.denominator = 4;
            price_size = 4;
            size_size = 2;
            break;
        case EXTENDED_2_SIDED_NBBO_MSG_TYPE:
        case EXTENDED_1_SIDED_NBBO_MSG_TYPE:
            // 3-2. Denominator
            price_size = 8;
            size_size = 4;
            convert_big_endian_to_uint64_t(&smt_table->raw_data[offset], &depth.price.denominator, NBBO_DEPTH_DENOMINATOR_LEN);
            offset += NBBO_DEPTH_DENOMINATOR_LEN;
            break;
        default:
            return (-1);
        }

        // 3-3. Price
        convert_big_endian_to_int64_t(&smt_table->raw_data[offset], &price, price_size);
        depth.price.value = (double)price / (pow(10, depth.price.denominator));
        offset += price_size;

        // 3-4. Size
        convert_big_endian_to_uint64_t(&smt_table->raw_data[offset], &depth.size, size_size);
        offset += size_size;

        // 3-5. Side
        if (nbbo->nside == 1)
        {
            depth.side = (unsigned char)smt_table->raw_data[offset];
            offset += NBBO_SIDE_LEN;
        }

        switch (depth.side)
        {
        case 'B':
            memcpy(&nbbo->bid, &depth, sizeof(NBBO_DEPTH));
            break;
        case 'S':
            memcpy(&nbbo->ask, &depth, sizeof(NBBO_DEPTH));
            break;
        }
    }

    // 4. RFU / Condition
    decode_fixedfld(fixedfld, smt_table->raw_data, &offset, 9);

    // 5. Flags
    decode_fixedfld(fixedfld, smt_table->raw_data, &offset, 10);

    // 6. Appendage List(NBBO does not support appendage)
    // smt_appendage_decode(smt_table, offset);

    // # Log Message(CSV)
    _smt_csv(smt_table, fixedfld);

    return (0);
}

/*
 * Function: smt_trade_decode()
 * -------------------------------
 * Market:TRADE 파싱(FIXEDFLD 기반)
 */
int smt_trade_decode(SMARTOPTION_TABLE *smt_table)
{
    TRADE *trade = &smt_table->market_data.trade;
    int offset = 0;
    uint64_t price_size = 0, size_size = 0;
    FIXEDFLD fixedfld[] = {
        {"Instrument Locate", FIXEDFLD_UINT, &TRADE_INSTRUMENT_LOCATE_LEN, (void *)&trade->instrument_locate, 0},
        {"Market Center Locate", FIXEDFLD_UINT, &TRADE_INSTRUMENT_LOCATE_LEN, (void *)&trade->market_center_locate, 0},
        {"Trade ID", FIXEDFLD_UINT, &TRADE_TRADE_ID_LEN, (void *)&trade->trade_id, 0},
        {"Price Denominator", FIXEDFLD_UINT, &TRADE_DENOMINATOR_LEN, (void *)&trade->price.denominator, 0},
        {"Price", FIXEDFLD_DECIMAL, &price_size, (void *)&trade->price, 0},
        {"Size", FIXEDFLD_UINT, &size_size, (void *)&trade->size, 0},
        {"Price Flags", FIXEDFLD_BITMASK, &TRADE_PRICE_FLAGS_LEN, (void *)&trade->price_flags, 0},
        {"Eligibility Flags", FIXEDFLD_BITMASK, &TRADE_ELIGIBILITY_FLAGS_LEN, (void *)&trade->eligibility_flags, 0},
        {"Report Flags", FIXEDFLD_BITMASK, &TRADE_REPORT_FLAGS_LEN, (void *)&trade->report_flags, 0},
        {"Change Flags", FIXEDFLD_BITMASK, &TRADE_CHANGE_FLAGS_LEN, (void *)&trade->change_flags, 0},
        {"Cancel Flags", FIXEDFLD_BITMASK, &TRADE_CANCEL_FLAGS_LEN, (void *)&trade->cancel_flags, 0},
        {"Upstream Condition Detail", FIXEDFLD_STRING, &DEFAULT_LEN_4, (void *)&trade->upstream_condition_detail, 1},
        {"Trade Report Detail", FIXEDFLD_UINT, &DEFAULT_LEN_4, (void *)&trade->trade_report_detail, 1},
        {"Extended Report Flag", FIXEDFLD_BITMASK, &DEFAULT_LEN_4, (void *)&trade->extended_report_flags, 1},
        {"", 0, NULL, NULL, -1}};

    smt_table->class = SMT_TRADE_CLASS;

    // 1. Market Data Standard Header
    smt_market_header_decode(smt_table);
    offset += MARKET_HEADER_TOTAL_LEN;

    // 2. Instrument Locate
    decode_fixedfld(fixedfld, smt_table->raw_data, &offset, 0);

    // 3. Market Center Locate
    decode_fixedfld(fixedfld, smt_table->raw_data, &offset, 1);

    // 4. Trade ID
    decode_fixedfld(fixedfld, smt_table->raw_data, &offset, 2);

    switch (smt_table->type)
    {
    case SHORT_TRADE_MSG_TYPE:
        trade->price.denominator = 2;
        price_size = 2;
        size_size = 2;
        trade->trade_type = TRADE_TRADE_TYPE;
        break;
    case LONG_TRADE_MSG_TYPE:
        trade->price.denominator = 4;
        price_size = 4;
        size_size = 2;
        trade->trade_type = TRADE_TRADE_TYPE;
        break;
    case EXTENDED_TRADE_MSG_TYPE:
        // 5. Denominator
        price_size = 8;
        size_size = 8;
        trade->trade_type = TRADE_TRADE_TYPE;
        decode_fixedfld(fixedfld, smt_table->raw_data, &offset, 3);
        break;
    case TRADE_CANCEL_MSG_TYPE:
        // 5. Denominator
        price_size = 8;
        size_size = 8;
        trade->trade_type = CANCEL_TRADE_TYPE;
        decode_fixedfld(fixedfld, smt_table->raw_data, &offset, 3);
        break;
    default:
        return (-1);
    }

    // 6. Price
    decode_fixedfld(fixedfld, smt_table->raw_data, &offset, 4);

    // 7. Size
    decode_fixedfld(fixedfld, smt_table->raw_data, &offset, 5);

    // 8. Price Flags
    decode_fixedfld(fixedfld, smt_table->raw_data, &offset, 6);

    // 9. Eligibility Flags
    decode_fixedfld(fixedfld, smt_table->raw_data, &offset, 7);

    // 10. Report Flags
    decode_fixedfld(fixedfld, smt_table->raw_data, &offset, 8);

    // 11. Change Flags / Cancel Flags
    if (trade->trade_type != CANCEL_TRADE_TYPE)
    {
        decode_fixedfld(fixedfld, smt_table->raw_data, &offset, 9);
    }
    else
    {
        decode_fixedfld(fixedfld, smt_table->raw_data, &offset, 10);
    }

    // 12. Appendage List
    smt_appendage_decode(smt_table, &offset);

    // # Log Message(CSV)
    _smt_csv(smt_table, fixedfld);

    return (0);
}

/*
 * Function: smt_default_decode()
 * ----------------------------------------------------
 * Market:NBBO, TRADE를 제외한 나머지 파싱(FIXEDFLD 기반)
 */
int smt_default_decode(SMARTOPTION_TABLE *smt_table, FIXEDFLD *fixedfld, int is_market)
{
    int offset = 0;

    smt_table->class = SMT_DEFAULT_CLASS;

    if (is_market)
    {
        smt_market_header_decode(smt_table);
        offset += MARKET_HEADER_TOTAL_LEN;
    }

    // Decode fields
    decode_fixedfld_all(fixedfld, smt_table->raw_data, &offset);

    // Appendage List
    smt_appendage_decode(smt_table, &offset);

    // Log Message(CSV)
    _smt_csv(smt_table, fixedfld);
    return 0;
}

/*
 * Function: smt_decode()
 * -----------------------------
 * Raw Data -> SMARTOPTION Table
 */
int smt_decode(SMARTOPTION_TABLE *smt_table)
{
    int is_market = 0;

    switch (smt_table->type)
    {
    case SYSTEM_EVENT_MSG_TYPE: // 0x20
    {
        strcpy(smt_table->name, "System Event");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        FIXEDFLD fixedfld[] = {
            {"Message Type", FIXEDFLD_BITMASK, &SYSTEM_EVENT_MSG_TYPE_LEN, (void *)&smt_table->system_event.msgtype, 0},
            {"Timestamp", FIXEDFLD_UINT, &SYSTEM_EVENT_TIMESTAMP_LEN, (void *)&smt_table->system_event.timestamp, 0},
            {"Event Code", FIXEDFLD_STRING, &SYSTEM_EVENT_EVENTCODE_LEN, (void *)&smt_table->system_event.event_code, 0},
            {"", 0, NULL, NULL, -1}};
        smt_default_decode(smt_table, fixedfld, is_market);
        break;
    }
    case CHANNEL_SECONDS_MSG_TYPE: // 0x22
    {
        strcpy(smt_table->name, "Channel Seconds");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        FIXEDFLD fixedfld[] = {
            {"Message Type", FIXEDFLD_BITMASK, &CHANNEL_SECONDS_MSG_TYPE_LEN, (void *)&smt_table->channel_seconds.msgtype, 0},
            {"Protocol ID", FIXEDFLD_UINT, &CHANNEL_SECONDS_PROTOCOL_ID_LEN, (void *)&smt_table->channel_seconds.protocol_id, 0},
            {"Channel Index", FIXEDFLD_UINT, &CHANNEL_SECONDS_CHANNEL_INDEX_LEN, (void *)&smt_table->channel_seconds.channel_index, 0},
            {"Seconds", FIXEDFLD_UINT, &CHANNEL_SECONDS_SECONDS_LEN, (void *)&smt_table->channel_seconds.seconds, 0},
            {"", 0, NULL, NULL, -1}};
        smt_default_decode(smt_table, fixedfld, is_market);
        smt_table->proc = smt_0x22;
        break;
    }
    case MARKET_CENTER_LOCATE_MSG_TYPE: // 0x30
    {
        strcpy(smt_table->name, "Market Center Locate");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        FIXEDFLD fixedfld[] = {
            {"Message Type", FIXEDFLD_BITMASK, &MARKET_CENTER_LOCATE_MSG_TYPE_LEN, (void *)&smt_table->market_center_locate.msgtype, 0},
            {"Locate Code", FIXEDFLD_UINT, &MARKET_CENTER_LOCATE_LOCATECODE_LEN, (void *)&smt_table->market_center_locate.locate_code, 0},
            {"MIC", FIXEDFLD_STRING, &MARKET_CENTER_LOCATE_MIC_LEN, (void *)&smt_table->market_center_locate.MIC, 0},
            {"", 0, NULL, NULL, -1}};
        smt_default_decode(smt_table, fixedfld, is_market);
        break;
    }
    case INSTRUMENT_LOCATE_MSG_TYPE: // 0x33
    {
        strcpy(smt_table->name, "Instrument Symbol Locate");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 1;
        FIXEDFLD fixedfld[] = {
            {"Message Type", FIXEDFLD_BITMASK, &MARKET_CENTER_LOCATE_MSG_TYPE_LEN, (void *)&smt_table->instrument_locate.msgtype, 0},
            {"Locate Code", FIXEDFLD_UINT, &INSTRUMENT_LOCATE_LOCATECODE_LEN, (void *)&smt_table->instrument_locate.locate_code, 0},
            {"Country Code", FIXEDFLD_STRING, &INSTRUMENT_LOCATE_COUNTRYCODE_LEN, (void *)smt_table->instrument_locate.country_code, 0},
            {"Currecny Code", FIXEDFLD_STRING, &INSTRUMENT_LOCATE_CURRENCYCODE_LEN, (void *)smt_table->instrument_locate.currency_code, 0},
            {"MIC", FIXEDFLD_STRING, &INSTRUMENT_LOCATE_MIC_LEN, (void *)smt_table->instrument_locate.MIC, 0},
            {"Product Type", FIXEDFLD_UINT, &INSTRUMENT_LOCATE_PRODUCTTYPE_LEN, (void *)&smt_table->instrument_locate.product_type, 0},
            {"Symbol Length", FIXEDFLD_UINT, &INSTRUMENT_LOCATE_SYMBOLLENGTH_LEN, (void *)&smt_table->instrument_locate.symbol_length, 0},
            {"Symbol", FIXEDFLD_STRING, &smt_table->instrument_locate.symbol_length, (void *)smt_table->instrument_locate.symbol, 0},
            {"Root Symbol", FIXEDFLD_STRING, &DEFAULT_LEN_4, (void *)smt_table->instrument_locate.root, 1},
            {"Put/Call", FIXEDFLD_STRING, &DEFAULT_LEN_4, (void *)smt_table->instrument_locate.put_or_call, 1},
            {"Expiration Date", FIXEDFLD_UINT, &DEFAULT_LEN_4, (void *)&smt_table->instrument_locate.expiration_date, 1},
            {"Strike Price", FIXEDFLD_DECIMAL, &DEFAULT_LEN_4, (void *)&smt_table->instrument_locate.strike, 1},
            {"Parent Locate Code", FIXEDFLD_UINT, &DEFAULT_LEN_4, (void *)&smt_table->instrument_locate.parent_locate_code, 1},
            {"", 0, NULL, NULL, -1}};
        smt_default_decode(smt_table, fixedfld, is_market);
        smt_table->proc = smt_0x33;
        break;
    }
    case SHORT_2_SIDED_NBBO_MSG_TYPE: // 0x60
    {
        strcpy(smt_table->name, "NBBO(S2)");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_table->market_data.nbbo.nside = 2;
        smt_nbbo_decode(smt_table);
        break;
    }
    case LONG_2_SIDED_NBBO_MSG_TYPE: // 0x61
    {
        strcpy(smt_table->name, "NBBO(L2)");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_table->market_data.nbbo.nside = 2;
        smt_nbbo_decode(smt_table);
        break;
    }
    case EXTENDED_2_SIDED_NBBO_MSG_TYPE: // 0x62
    {
        strcpy(smt_table->name, "NBBO(E2)");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_table->market_data.nbbo.nside = 2;
        smt_nbbo_decode(smt_table);
        break;
    }
    case SHORT_1_SIDED_NBBO_MSG_TYPE: // 0x63
    {
        strcpy(smt_table->name, "NBBO(S1)");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_table->market_data.nbbo.nside = 1;
        smt_nbbo_decode(smt_table);
        break;
    }
    case LONG_1_SIDED_NBBO_MSG_TYPE: // 0x64
    {
        strcpy(smt_table->name, "NBBO(L1)");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_table->market_data.nbbo.nside = 1;
        smt_nbbo_decode(smt_table);
        break;
    }
    case EXTENDED_1_SIDED_NBBO_MSG_TYPE: // 0x65
    {
        strcpy(smt_table->name, "NBBO(E1)");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_table->market_data.nbbo.nside = 1;
        smt_nbbo_decode(smt_table);
        break;
    }
    case SHORT_TRADE_MSG_TYPE: // 0x70
    {
        strcpy(smt_table->name, "Trade(S1)");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_trade_decode(smt_table);
        break;
    }
    case LONG_TRADE_MSG_TYPE: // 0x71
    {
        strcpy(smt_table->name, "Trade(L1)");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_trade_decode(smt_table);
        break;
    }
    case EXTENDED_TRADE_MSG_TYPE: // 0x72
    {
        strcpy(smt_table->name, "Trade(E1)");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_trade_decode(smt_table);
        break;
    }
    case TRADE_CANCEL_MSG_TYPE: // 0x73
    {
        strcpy(smt_table->name, "Cancel");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_trade_decode(smt_table);
        break;
    }
    case VALUE_UPDATE_MSG_TYPE: // 0x80
    {
        strcpy(smt_table->name, "Value Update");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        is_market = 1;
        FIXEDFLD fixedfld[] = {
            {"Instrument Locate", FIXEDFLD_UINT, &VALUE_UPDATE_INSTRUMENT_LOCATE_LEN, (void *)&smt_table->market_data.value_update.instrument_locate, 0},
            {"Market Center Locate", FIXEDFLD_UINT, &VALUE_UPDATE_MARKET_CENTER_LOCATE_LEN, (void *)&smt_table->market_data.value_update.market_center_locate, 0},
            {"Value Update Flags", FIXEDFLD_BITMASK, &VALUE_UPDATE_VALUE_UPDATE_FLAGS_LEN, (void *)&smt_table->market_data.value_update.value_update_flags, 0},
            {"High Price", FIXEDFLD_DECIMAL, &DEFAULT_LEN_4, (void *)&smt_table->market_data.value_update.high, 1},
            {"Low Price", FIXEDFLD_DECIMAL, &DEFAULT_LEN_4, (void *)&smt_table->market_data.value_update.low, 1},
            {"Last Price", FIXEDFLD_DECIMAL, &DEFAULT_LEN_4, (void *)&smt_table->market_data.value_update.last, 1},
            {"Open Price", FIXEDFLD_DECIMAL, &DEFAULT_LEN_4, (void *)&smt_table->market_data.value_update.open, 1},
            {"Total Volume", FIXEDFLD_UINT, &DEFAULT_LEN_4, (void *)&smt_table->market_data.value_update.total_volume, 1},
            {"Net Change", FIXEDFLD_DECIMAL, &DEFAULT_LEN_4, (void *)&smt_table->market_data.value_update.net_change, 1},
            {"Open Interest", FIXEDFLD_UINT, &DEFAULT_LEN_4, (void *)&smt_table->market_data.value_update.open_interest, 1},
            {"Tick", FIXEDFLD_DECIMAL, &DEFAULT_LEN_4, (void *)&smt_table->market_data.value_update.tick, 1},
            {"Bid", FIXEDFLD_DECIMAL, &DEFAULT_LEN_4, (void *)&smt_table->market_data.value_update.bid, 1},
            {"Ask", FIXEDFLD_DECIMAL, &DEFAULT_LEN_4, (void *)&smt_table->market_data.value_update.ask, 1},
            {"Underlying Price", FIXEDFLD_DECIMAL, &DEFAULT_LEN_4, (void *)&smt_table->market_data.value_update.underlying_price, 1},
            {"", 0, NULL, NULL, -1}};
        smt_default_decode(smt_table, fixedfld, is_market);
        break;
    }
    case INSTRUMENT_STATUS_MSG_TYPE: // 0x90
    {
        strcpy(smt_table->name, "Trade Action");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        is_market = 1;
        FIXEDFLD fixedfld[] = {
            {"Instrument Locate", FIXEDFLD_UINT, &INSTRUMENT_STATUS_INSTRUMENT_LOCATE_LEN, (void *)&smt_table->market_data.instrument_status.instrument_locate, 0},
            {"Market Center Locate", FIXEDFLD_UINT, &INSTRUMENT_STATUS_MARKET_CENTER_LOCATE_LEN, (void *)&smt_table->market_data.instrument_status.market_center_locate, 0},
            {"Status Type", FIXEDFLD_UINT, &INSTRUMENT_STATUS_STATUS_TYPE_LEN, (void *)&smt_table->market_data.instrument_status.status_type, 0},
            {"Status Code", FIXEDFLD_UINT, &INSTRUMENT_STATUS_STATUS_CODE_LEN, (void *)&smt_table->market_data.instrument_status.status_code, 0},
            {"Reason Code", FIXEDFLD_UINT, &INSTRUMENT_STATUS_REASON_CODE_LEN, (void *)&smt_table->market_data.instrument_status.reason_code, 0},
            {"Status Flags", FIXEDFLD_BITMASK, &INSTRUMENT_STATUS_STATUS_FLAGS_LEN, (void *)&smt_table->market_data.instrument_status.status_flags, 0},
            {"Reason Detail Length", FIXEDFLD_UINT, &INSTRUMENT_STATUS_REASON_DETAIL_LENGTH_LEN, (void *)&smt_table->market_data.instrument_status.reason_detail_length, 0},
            {"Reason Detail", FIXEDFLD_STRING, &smt_table->market_data.instrument_status.reason_detail_length, (void *)smt_table->market_data.instrument_status.reason_detail, 0},
            {"", 0, NULL, NULL, -1}};
        smt_default_decode(smt_table, fixedfld, is_market);
        break;
    }
    case CHANNEL_EVENT_MSG_TYPE: // 0xB0
    {
        strcpy(smt_table->name, "Channel Event");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        is_market = 1;
        FIXEDFLD fixedfld[] = {
            {"Event Code", FIXEDFLD_STRING, &CHANNEL_EVENT_EVENT_CODE_LEN, (void *)smt_table->market_data.channel_event.event_code, 0},
            {"Market Center Locate", FIXEDFLD_UINT, &CHANNEL_EVENT_MARKET_CENTER_LOCATE_LEN, (void *)&smt_table->market_data.channel_event.market_center_locate, 0},
            {"", 0, NULL, NULL, -1}};
        smt_default_decode(smt_table, fixedfld, is_market);
        break;
    }
    case ADMIN_TEXT_MSG_TYPE: // 0xB2
    {
        strcpy(smt_table->name, "Admin Text");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        is_market = 1;
        FIXEDFLD fixedfld[] = {
            {"Text Length", FIXEDFLD_UINT, &ADMIN_TEXT_TEXT_LENGTH_LEN, (void *)&smt_table->market_data.administrative_text.text_length, 0},
            {"Text", FIXEDFLD_STRING, &smt_table->market_data.administrative_text.text_length, (void *)smt_table->market_data.administrative_text.text, 0},
            {"", 0, NULL, NULL, -1}};
        smt_default_decode(smt_table, fixedfld, is_market);
        break;
    }
    case INSTRUMENT_META_DATA_MSG_TYPE: // 0xC0
    {
        strcpy(smt_table->name, "Instrument Meta Data");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        FIXEDFLD fixedfld[] = {
            {"Message Type", FIXEDFLD_BITMASK, &INSTRUMENT_META_DATA_MSG_TYPE_LEN, (void *)&smt_table->reference.meta_data.msgtype, 0},
            {"Locate Code", FIXEDFLD_UINT, &INSTRUMENT_META_DATA_LOCATE_CODE_LEN, (void *)&smt_table->reference.meta_data.locate_code, 0},
            {"Parent Symbol Locate", FIXEDFLD_UINT, &DEFAULT_LEN_4, (void *)&smt_table->reference.meta_data.parent_symbol_locate, 1},
            {"", 0, NULL, NULL, -1}};
        smt_default_decode(smt_table, fixedfld, is_market);
        break;
    }
    case OPTION_DELIVERY_COMPONENT_MSG_TYPE: // 0xC3
    {
        strcpy(smt_table->name, "Option Delivery Component");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        FIXEDFLD fixedfld[] = {
            {"Message Type", FIXEDFLD_BITMASK, &OPTION_DELIVERY_COMPONENT_MSG_TYPE_LEN, (void *)&smt_table->reference.option_delivery.msgtype, 0},
            {"Root Code Locate", FIXEDFLD_UINT, &OPTION_DELIVERY_COMPONENT_ROOT_CODE_LOCATE_LEN, (void *)&smt_table->reference.option_delivery.root_code_locate, 0},
            {"Component Index", FIXEDFLD_UINT, &OPTION_DELIVERY_COMPONENT_COMPONENT_INDEX_LEN, (void *)&smt_table->reference.option_delivery.component_index, 0},
            {"Component Total", FIXEDFLD_UINT, &OPTION_DELIVERY_COMPONENT_COMPONENT_TOTAL_LEN, (void *)&smt_table->reference.option_delivery.component_total, 0},
            {"Deliverable Units", FIXEDFLD_UINT, &OPTION_DELIVERY_COMPONENT_DELIVERABLE_UNITS_LEN, (void *)&smt_table->reference.option_delivery.deliverable_units, 0},
            {"Settlement Method", FIXEDFLD_UINT, &OPTION_DELIVERY_COMPONENT_SETTLEMENT_METHOD_LEN, (void *)&smt_table->reference.option_delivery.settlement_method, 0},
            {"Fixed Amount Denominator", FIXEDFLD_UINT, &OPTION_DELIVERY_COMPONENT_FIXED_AMOUNT_DENOMINATOR_LEN, (void *)&smt_table->reference.option_delivery.fixed_amount_denominator, 0},
            {"Fixed Amount Numerator", FIXEDFLD_UINT, (void *)&OPTION_DELIVERY_COMPONENT_FIXED_AMOUNT_NUMERATOR_LEN, (void *)&smt_table->reference.option_delivery.fixed_amount_numerator, 0},
            {"Currecny Code", FIXEDFLD_STRING, (void *)&OPTION_DELIVERY_COMPONENT_CURRENCY_CODE_LEN, smt_table->reference.option_delivery.currency_code, 0},
            {"Strike Percent", FIXEDFLD_DECIMAL, &OPTION_DELIVERY_COMPONENT_STRIKE_PERCENT_LEN, (void *)&smt_table->reference.option_delivery.strike_percent, 0},
            {"Component Symbol Locate", FIXEDFLD_UINT, &OPTION_DELIVERY_COMPONENT_COMPONENT_SYMBOL_LOCATE_LEN, (void *)&smt_table->reference.option_delivery.component_symbol_locate, 0},
            {"", 0, NULL, NULL, -1}};
        smt_default_decode(smt_table, fixedfld, is_market);
        break;
    }
    default:
    {
        sprintf(smt_table->name, "Unknown Type(0x%02X)", (unsigned int)smt_table->type);
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        return (0);
    }
    }
    return (0);
}