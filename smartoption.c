#include "nastag.h"

/* Appendage List */
int get_append_header(unsigned char *msgb, AppendHeader *header)
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

int get_append_decimal(unsigned char *msgb, AppendHeader *header, double *value, uint64_t *denominator)
{
    int offset = 0;
    int64_t decimal;
    int value_size;
    int denominator_len;

    switch (header->format_code)
    {
    case SHORT_FORM_DECIMAL_FORMAT_CODE:
        *denominator = 2;
        value_size = header->element_length;
        break;
    case LONG_FORM_DECIMAL_FORMAT_CODE:
        *denominator = 4;
        value_size = header->element_length;
        break;
    case EXTENDED_FORM_DECIMAL_FORMAT_CODE:
        // Value Denomiator Code
        denominator_len = 1;
        convert_big_endian_to_uint64_t(&msgb[offset], denominator, denominator_len);
        offset += denominator_len;
        value_size = header->element_length - denominator_len;
        break;
    default:
        return (-1);
    }

    // Value
    convert_big_endian_to_int64_t(&msgb[offset], &decimal, value_size);
    *value = (double)decimal / (pow(10, *denominator));

    return (0);
}

int get_append_numeric(unsigned char *msgb, AppendHeader *header, int64_t *valueInt, uint64_t *valueUint)
{
    int offset = 0;
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
        convert_big_endian_to_int64_t(&msgb[offset], valueInt, value_size);
    if (valueUint)
        convert_big_endian_to_uint64_t(&msgb[offset], valueUint, value_size);

    return (0);
}

int get_append_date(unsigned char *msgb, AppendHeader *header, uint64_t *date)
{
    uint64_t month, day, year;
    int offset = 0;

    if (header->format_code != DATE_FORMAT_CODE)
        return (-1);

    // 1. Month
    int month_size = 1;
    convert_big_endian_to_uint64_t(&msgb[offset], &month, month_size);
    offset += month_size;

    // 2. Day
    int day_size = 1;
    convert_big_endian_to_uint64_t(&msgb[offset], &day, day_size);
    offset += day_size;

    // 3. Year
    int year_size = 2;
    convert_big_endian_to_uint64_t(&msgb[offset], &year, year_size);
    offset += year_size;

    // Make DATE Value
    time_t current = time(NULL);
    struct tm _tm;

    localtime_r(&current, &_tm);

    *date = (_tm.tm_year / 100 + year) * 10000 + month * 100 + day;

    return (0);
}

int _smt_appendage_list(SMARTOPTION_TABLE *smt_table, unsigned char *msgb, int msgl, int offset)
{
    InstrumentLocate *instrumentLocate = &smt_table->instrument_locate;
    TRADE *trade = &smt_table->market_data.trade;
    VALUE_UPDATE *valueUpdate = &smt_table->market_data.value_update;
    METADATA *meta = &smt_table->reference.meta_data;
    AppendHeader header;

    while (offset < (msgl - 1))
    {
        memset(&header, 0x00, sizeof(AppendHeader));

        // 1. Header
        get_append_header(&msgb[offset], &header);
        offset += (APPEND_ELEMENT_LENGTH_LEN + APPEND_FORMATCODE_LEN + APPEND_VALUETYPECODE_LEN);

        // 2. Element
        switch (header.value_type_code)
        {
        case ROOT_SYMBOL_VALUE_TYPE: // 1 / String / Instrument Locate
            if (header.format_code != STRING_FORMAT_CODE)
                break;
            memcpy(instrumentLocate->root, &msgb[offset], header.element_length);
            break;
        case PUT_CALL_VALUE_TYPE: // 2 /Char / Instrument Locate
            if (header.format_code != CHAR_FORMAT_CODE)
                break;
            memcpy(instrumentLocate->put_or_call, &msgb[offset], header.element_length);
            break;
        case EXPIRATION_DATE_VALUE_TYPE: // 3 / Date / Instrument Locate
            get_append_date(&msgb[offset], &header, &instrumentLocate->expiration_date);
            break;
        case STRIKE_PRICE_VALUE_TYPE: // 4 / Decimal / Insturment Locate
            get_append_decimal(&msgb[offset], &header, &instrumentLocate->strike.value, &instrumentLocate->strike.denominator);
            break;
        case HIGH_PRICE_VALUE_TYPE: // 65 / Decimal / Value Update
            get_append_decimal(&msgb[offset], &header, &valueUpdate->high.value, &valueUpdate->high.denominator);
            break;
        case LOW_PRICE_VALUE_TYPE: // 66 / Decimal / Value Update
            get_append_decimal(&msgb[offset], &header, &valueUpdate->low.value, &valueUpdate->low.denominator);
            break;
        case LAST_PRICE_VALUE_TYPE: // 67 / Decimal / Value Update
            get_append_decimal(&msgb[offset], &header, &valueUpdate->last.value, &valueUpdate->last.denominator);
            break;
        case OPEN_PRICE_VALUE_TYPE: // 68 / Decimal / Value Update
            get_append_decimal(&msgb[offset], &header, &valueUpdate->open.value, &valueUpdate->open.denominator);
            break;
        case TOTAL_VOLUME_VALUE_TYPE: // 70 / Numeric / Value Update
            get_append_numeric(&msgb[offset], &header, NULL, &valueUpdate->total_volume);
            break;
        case NET_CHANGE_VALUE_TYPE: // 71 / Decimal / Value Update
            get_append_decimal(&msgb[offset], &header, &valueUpdate->net_change.value, &valueUpdate->net_change.denominator);
            break;
        case OPEN_INTEREST_VALUE_TYPE: // 72 / Numeric / Value Update
            get_append_numeric(&msgb[offset], &header, NULL, &valueUpdate->open_interest);
            break;
        case TICK_VALUE_TYPE: // 73 / Decimal / Value Update
            get_append_decimal(&msgb[offset], &header, &valueUpdate->tick.value, &valueUpdate->tick.denominator);
            break;
        case BID_VALUE_TYPE: // 74 / Decimal / Value Update
            get_append_decimal(&msgb[offset], &header, &valueUpdate->bid.value, &valueUpdate->bid.denominator);
            break;
        case ASK_VALUE_TYPE: // 75 / Decimal / Value Update
            get_append_decimal(&msgb[offset], &header, &valueUpdate->ask.value, &valueUpdate->ask.denominator);
            break;
        case PARENT_SYMBOL_LOCATE_VALUE_TYPE: //  81 / Numeric / Instrument Locate
            get_append_numeric(&msgb[offset], &header, NULL, &instrumentLocate->parent_locate_code);
            get_append_numeric(&msgb[offset], &header, NULL, &meta->parent_symbol_locate);
            break;
        case UNDERLYING_PRICE_VALUE_TYPE: // 100 / Decimal / Value Update
            get_append_decimal(&msgb[offset], &header, &valueUpdate->underlying_price.value, &valueUpdate->underlying_price.denominator);
            break;
        case UPSTREAM_CONDITION_DETAILS_VALUE_TYPE: // 143 / String / Trades, Trade Cancel
            if (header.format_code != STRING_FORMAT_CODE)
                break;
            memcpy(trade->upstream_condition_detail, &msgb[offset], header.element_length);
            break;
        case TRADE_REPORT_DETAIL_VALUE_TYPE: // 145 / Byte Enum / Trades, Trade Cancel
            if (header.format_code != BYTE_VALUE_FORMAT_CODE)
                break;
            trade->trade_report_detail = (unsigned char)msgb[offset];
            break;
        case EXTENDED_REPORT_FLAGS_VALUE_TYPE: // 146 / Numeric / Trades, Trade Cancel
            get_append_numeric(&msgb[offset], &header, NULL, &trade->extended_report_flags);
            break;
        default:
            break;
        }
        offset += header.element_length;
    }

    return (0);
}

/* System Event Message(0x20) */
int smt_0x20_decode(SMARTOPTION_TABLE *smt_table, unsigned char *msgb, int msgl)
{
    SystemEvent *systemEvent = &smt_table->system_event;
    int offset = SYSTEM_EVENT_MSG_TYPE_LEN;

    // 1. Timestamp
    convert_big_endian_to_uint64_t(&msgb[offset], &systemEvent->timestamp, SYSTEM_EVENT_TIMESTAMP_LEN);
    offset += SYSTEM_EVENT_TIMESTAMP_LEN;

    // 2. Event Code
    memcpy(systemEvent->event_code, &msgb[offset], SYSTEM_EVENT_EVENTCODE_LEN);

    // # Log Message
    sprintf(smt_table->logmsg, "Timestamp=%lu|Event Code=%s", systemEvent->timestamp, systemEvent->event_code);

    return (0);
}

/* Channel Seconds Message(0x22) */
int smt_0x22_decode(SMARTOPTION_TABLE *smt_table, unsigned char *msgb, int msgl)
{
    ChannelSeconds *channelSeconds = &smt_table->channel_seconds;
    int offset = CHANNEL_SECONDS_MSG_TYPE_LEN;

    // 1. Protocol ID
    convert_big_endian_to_uint64_t(&msgb[offset], &channelSeconds->protocol_id, CHANNEL_SECONDS_PROTOCOL_ID_LEN);
    offset += CHANNEL_SECONDS_PROTOCOL_ID_LEN;

    // 2. Channel Index
    convert_big_endian_to_uint64_t(&msgb[offset], &channelSeconds->channel_index, CHANNEL_SECONDS_CHANNEL_INDEX_LEN);
    offset += CHANNEL_SECONDS_CHANNEL_INDEX_LEN;

    // 3. Seconds
    convert_big_endian_to_uint64_t(&msgb[offset], &channelSeconds->seconds, CHANNEL_SECONDS_SECONDS_LEN);

    // # Log Message
    sprintf(smt_table->logmsg, "Protocol ID=%lu|Channel Index=%lu|Seconds=%lu", channelSeconds->protocol_id, channelSeconds->channel_index, channelSeconds->seconds);

    return (0);
}

/* Market Center Locate(0x30) */
int smt_0x30_decode(SMARTOPTION_TABLE *smt_table, unsigned char *msgb, int msgl)
{
    MarketCenterLocate *marketCenterLocate = &smt_table->market_center_locate;
    int offset = MARKET_CENTER_LOCATE_MSG_TYPE_LEN;

    // 1. Locate Code
    convert_big_endian_to_uint64_t(&msgb[offset], &marketCenterLocate->locate_code, MARKET_CENTER_LOCATE_LOCATECODE_LEN);
    offset += MARKET_CENTER_LOCATE_LOCATECODE_LEN;

    // 2. Event Code
    memcpy(marketCenterLocate->MIC, &msgb[offset], MARKET_CENTER_LOCATE_MIC_LEN);

    // # Log Message
    sprintf(smt_table->logmsg, "Locate Code=%lu|MIC=%s", marketCenterLocate->locate_code, marketCenterLocate->MIC);

    return (0);
}

/* Instrument Symbol Locate(0x33) */
int smt_0x33_decode(SMARTOPTION_TABLE *smt_table, unsigned char *msgb, int msgl)
{
    InstrumentLocate *instrumentLocate = &smt_table->instrument_locate;
    int offset = INSTRUMENT_LOCATE_MSG_TYPE_LEN;

    // 1. Locate Code
    convert_big_endian_to_uint64_t(&msgb[offset], &instrumentLocate->locate_code, INSTRUMENT_LOCATE_LOCATECODE_LEN);
    offset += INSTRUMENT_LOCATE_LOCATECODE_LEN;

    // 2. Country Code
    memcpy(instrumentLocate->country_code, &msgb[offset], INSTRUMENT_LOCATE_COUNTRYCODE_LEN);
    offset += INSTRUMENT_LOCATE_COUNTRYCODE_LEN;

    // 3. Currency Code
    memcpy(instrumentLocate->currency_code, &msgb[offset], INSTRUMENT_LOCATE_CURRENCYCODE_LEN);
    offset += INSTRUMENT_LOCATE_CURRENCYCODE_LEN;

    // 4. MIC
    memcpy(instrumentLocate->MIC, &msgb[offset], INSTRUMENT_LOCATE_MIC_LEN);
    offset += INSTRUMENT_LOCATE_MIC_LEN;

    // 5. Product Type
    convert_big_endian_to_uint64_t(&msgb[offset], &instrumentLocate->product_type, INSTRUMENT_LOCATE_PRODUCTTYPE_LEN);
    offset += INSTRUMENT_LOCATE_PRODUCTTYPE_LEN;

    // 6. Symbol Length
    convert_big_endian_to_uint64_t(&msgb[offset], &instrumentLocate->symbol_length, INSTRUMENT_LOCATE_SYMBOLLENGTH_LEN);
    offset += INSTRUMENT_LOCATE_SYMBOLLENGTH_LEN;

    // 7. Symbol
    memcpy(instrumentLocate->symbol, &msgb[offset], instrumentLocate->symbol_length);
    offset += instrumentLocate->symbol_length;

    // 8. Appendage
    _smt_appendage_list(smt_table, msgb, msgl, offset);

    // # Log Message
    sprintf(smt_table->logmsg, "Locate Code=%lu|Country Code=%s|Curreny Code=%s|MIC=%s|Product Type=%lu|Symbol Length=%lu|Symbol=%s|Root=%s|Put/Call=%s|Expiration Date=%lu|Strike Price=%.*f|Strike Denominator=%lu|Parent Locate Code=%lu", instrumentLocate->locate_code, instrumentLocate->country_code, instrumentLocate->currency_code, instrumentLocate->MIC, instrumentLocate->product_type, instrumentLocate->symbol_length, instrumentLocate->symbol, instrumentLocate->root, instrumentLocate->put_or_call, instrumentLocate->expiration_date, (int)instrumentLocate->strike.denominator, instrumentLocate->strike.value, instrumentLocate->strike.denominator, instrumentLocate->parent_locate_code);
    return (0);
}

/* Makret Data Header */
int _smt_market_header_decode(SMARTOPTION_TABLE *smt_table, unsigned char *msgb, int msgl)
{
    MarketDataHeader *header = &smt_table->market_data.header;
    int offset = MARKET_HEADER_MSG_TYPE_LEN;

    // 1. Protocol ID
    convert_big_endian_to_uint64_t(&msgb[offset], &header->protocol_id, MARKET_HEADER_PROTOCOLID_LEN);
    offset += MARKET_HEADER_PROTOCOLID_LEN;

    // 2. Channel Index
    convert_big_endian_to_uint64_t(&msgb[offset], &header->channel_index, MARKET_HEADER_CHANNEL_INDEX_LEN);
    offset += MARKET_HEADER_CHANNEL_INDEX_LEN;

    // 3. Message Flags
    header->message_flag = (unsigned char)msgb[offset];
    offset += MARKET_HEADER_MESSAGE_FLAG_LEN;

    // 4. Upstream Sequence Number
    convert_big_endian_to_uint64_t(&msgb[offset], &header->upstream_seqn, MARKET_HEADER_UPSTREAM_SEQN_LEN);
    offset += MARKET_HEADER_UPSTREAM_SEQN_LEN;

    // 5. Upstream Nanos
    convert_big_endian_to_uint64_t(&msgb[offset], &header->upstream_nanos, MARKET_HEADER_UPSTREAM_NANOS_LEN);
    offset += MARKET_HEADER_UPSTREAM_NANOS_LEN;

    sprintf(smt_table->logmsg, "Protocol ID=%lu|Channel Index=%lu|Message Flags=0x%02X|Upstream Sequence Number=%lu|Upstream Nano=%lu|", header->protocol_id, header->channel_index, (int)header->message_flag, header->upstream_seqn, header->upstream_nanos);

    return (0);
}

/* Market Data: NBBO */
int smt_nbbo_decode(SMARTOPTION_TABLE *smt_table, unsigned char *msgb, int msgl)
{
    NBBO *nbbo = &smt_table->market_data.nbbo;
    int offset = 0;
    int ii = 0;
    int price_size = 0, size_size = 0;
    NBBO_DEPTH depth;
    int denominator_len;
    int is_extended = 0;
    int64_t price;

    // 1. Market Data Standard Header
    _smt_market_header_decode(smt_table, msgb, msgl);
    offset += MARKET_HEADER_TOTAL_LEN;

    // 2. Instrument Locate
    convert_big_endian_to_uint64_t(&msgb[offset], &nbbo->instrument_locate, NBBO_INSTRUMENT_LOCATE_LEN);
    offset += NBBO_INSTRUMENT_LOCATE_LEN;

    // 3. DEPTH
    for (ii = 0; ii < nbbo->nside; ii++)
    {
        memset(&depth, 0x00, sizeof(NBBO_DEPTH));
        depth.side = (ii == 0) ? 'B' : 'S';

        // 3-1. Market Center Locate
        convert_big_endian_to_uint64_t(&msgb[offset], &depth.market_center_locate, NBBO_DEPTH_MARKET_CENTER_LEN);
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
            denominator_len = 1;
            price_size = 8;
            size_size = 4;
            is_extended = 1;
            convert_big_endian_to_uint64_t(&msgb[offset], &depth.price.denominator, denominator_len);
            offset += denominator_len;
            break;
        default:
            return (-1);
        }

        // 3-3. Price
        convert_big_endian_to_int64_t(&msgb[offset], &price, price_size);
        depth.price.value = (double)price / (pow(10, depth.price.denominator));
        offset += price_size;

        // 3-4. Size
        convert_big_endian_to_uint64_t(&msgb[offset], &depth.size, size_size);
        offset += size_size;

        // 3-5. Side
        if (is_extended)
        {
            depth.side = (unsigned char)msgb[offset];
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
    if (smt_table->type == SHORT_1_SIDED_NBBO_MSG_TYPE)
        convert_big_endian_to_uint64_t(&msgb[offset], &nbbo->condition, NBBO_RFU_LEN);
    offset += NBBO_RFU_LEN;

    // 5. Flags
    nbbo->flags = (unsigned char)msgb[offset];
    offset += NBBO_FLAGS_LEN;

    // 6. Appendage List(NBBO does not support appendage)
    //_smt_appendage_list(smt_table, msgb, msgl, offset);

    // # Log Message
    sprintf(&smt_table->logmsg[strlen(smt_table->logmsg)], "Instrument Locate=%lu|Bid Market Center Locate=%lu|Bid Price=%.*f|Bid Denominator=%lu|Bid Size=%lu|Bid Side=%c|Ask Market Center Locate=%lu|Ask Price=%.*f|Ask Denominator=%lu|Ask Size=%lu|Ask Side=%c|Flags=0x%02X", nbbo->instrument_locate, nbbo->bid.market_center_locate, (int)nbbo->bid.price.denominator, nbbo->bid.price.value, nbbo->bid.price.denominator, nbbo->bid.size, nbbo->bid.side, nbbo->ask.market_center_locate, (int)nbbo->ask.price.denominator, nbbo->ask.price.value, nbbo->ask.price.denominator, nbbo->ask.size, nbbo->ask.side, (int)nbbo->flags);

    return (0);
}

/* Market Data: TRADE */
int smt_trade_decode(SMARTOPTION_TABLE *smt_table, unsigned char *msgb, int msgl)
{
    TRADE *trade = &smt_table->market_data.trade;
    int offset = 0;
    int64_t price;
    int denominator_len;
    int price_size;
    int size_size;

    // 1. Market Data Standard Header
    _smt_market_header_decode(smt_table, msgb, msgl);
    offset += MARKET_HEADER_TOTAL_LEN;

    // 2. Instrument Locate
    convert_big_endian_to_uint64_t(&msgb[offset], &trade->instrument_locate, TRADE_INSTRUMENT_LOCATE_LEN);
    offset += TRADE_INSTRUMENT_LOCATE_LEN;

    // 3. Market Center Locate
    convert_big_endian_to_uint64_t(&msgb[offset], &trade->market_center_locate, TRADE_MARKET_CENTER_LOCATE_LEN);
    offset += TRADE_MARKET_CENTER_LOCATE_LEN;

    // 4. Trade ID
    convert_big_endian_to_uint64_t(&msgb[offset], &trade->trade_id, TRADE_TRADE_ID_LEN);
    offset += TRADE_TRADE_ID_LEN;

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
        denominator_len = 1;
        price_size = 8;
        size_size = 8;
        trade->trade_type = TRADE_TRADE_TYPE;
        convert_big_endian_to_uint64_t(&msgb[offset], &trade->price.denominator, denominator_len);
        offset += denominator_len;
        break;
    case TRADE_CANCEL_MSG_TYPE:
        // 5. Denominator
        denominator_len = 1;
        price_size = 8;
        size_size = 8;
        trade->trade_type = CANCEL_TRADE_TYPE;
        convert_big_endian_to_uint64_t(&msgb[offset], &trade->price.denominator, denominator_len);
        offset += denominator_len;
        break;
    default:
        return (-1);
    }

    // 6. Price
    convert_big_endian_to_int64_t(&msgb[offset], &price, price_size);
    trade->price.value = (double)price / (pow(10, trade->price.denominator));
    offset += price_size;

    // 7. Size
    convert_big_endian_to_uint64_t(&msgb[offset], &trade->size, size_size);
    offset += size_size;

    // 8. Price Flags
    trade->price_flags = (unsigned char)msgb[offset];
    offset += TRADE_PRICE_FLAGS_LEN;

    // 9. Eligibility Flags
    trade->eligibility_flags = (unsigned char)msgb[offset];
    offset += TRADE_ELIGIBILITY_FLAGS_LEN;

    // 10. Report Flags
    trade->report_flags = (unsigned char)msgb[offset];
    offset += TRADE_REPORT_FLAGS_LEN;

    // 11. Change Flags / Cancel Flags
    if (trade->trade_type == 'C')
    {
        trade->cancel_flags = (unsigned char)msgb[offset];
        offset += TRADE_CANCEL_FLAGS_LEN;
        trade->change_flags = (unsigned char)msgb[offset];
        offset += TRADE_CHANGE_FLAGS_LEN;
    }
    else
    {
        trade->change_flags = (unsigned char)msgb[offset];
        offset += TRADE_CHANGE_FLAGS_LEN;
    }

    // 12. Appendage List
    _smt_appendage_list(smt_table, msgb, msgl, offset);

    // # Log Message
    sprintf(&smt_table->logmsg[strlen(smt_table->logmsg)], "Instrument Locate=%lu|Market Center Locate=%lu|Trade_id=%lu|Price=%.*f|Denominator=%lu|Size=%lu|Price Flags=0x%02X|Eligibility Flags=0x%02X|Report Flags=0x%04X|Change Flags=0x%02X|Cancel Flags=0x%02X|Upstream Condition Detail=%s|Trade Report Detail=%lu|Extended Report Flags=0x%04X", trade->instrument_locate, trade->market_center_locate, trade->trade_id, (int)trade->price.denominator, trade->price.value, trade->price.denominator, trade->size, (int)trade->price_flags, (int)trade->eligibility_flags, (int)trade->report_flags, (int)trade->change_flags, (int)trade->cancel_flags, trade->upstream_condition_detail, trade->trade_report_detail, (int)trade->extended_report_flags);

    return (0);
}

/* Intrument Value Update(0x80) */
int smt_0x80_decode(SMARTOPTION_TABLE *smt_table, unsigned char *msgb, int msgl)
{
    VALUE_UPDATE *valueUpdate = &smt_table->market_data.value_update;
    int offset = 0;

    // 1. Market Data Standard Header
    _smt_market_header_decode(smt_table, msgb, msgl);
    offset += MARKET_HEADER_TOTAL_LEN;

    // 2. Instrument Locate
    convert_big_endian_to_uint64_t(&msgb[offset], &valueUpdate->instrument_locate, VALUE_UPDATE_INSTRUMENT_LOCATE_LEN);
    offset += VALUE_UPDATE_INSTRUMENT_LOCATE_LEN;

    // 3. Market Center Locate
    convert_big_endian_to_uint64_t(&msgb[offset], &valueUpdate->market_center_locate, VALUE_UPDATE_MARKET_CENTER_LOCATE_LEN);
    offset += VALUE_UPDATE_MARKET_CENTER_LOCATE_LEN;

    // 4. Value Update Flags
    convert_big_endian_to_uint64_t(&msgb[offset], &valueUpdate->value_update_flags, VALUE_UPDATE_VALUE_UPDATE_FLAGS_LEN);
    offset += VALUE_UPDATE_VALUE_UPDATE_FLAGS_LEN;

    // 12. Appendage List
    _smt_appendage_list(smt_table, msgb, msgl, offset);

    // # Log Message
    sprintf(smt_table->logmsg, "Instrument Locate=%lu|Market Center Locate=%lu|Value Update Flags=0x%04X|High=%.*f|Low=%.*f|Last=%.*f|Open=%.*f|Total Volume=%lu|Net Change=%.*f|Open Interest=%lu|Tick=%.*f|Bid=%.*f|Ask=%.*f|Underlying Price=%.*f", valueUpdate->instrument_locate, valueUpdate->market_center_locate, (int)valueUpdate->value_update_flags, (int)valueUpdate->high.denominator, valueUpdate->high.value, (int)valueUpdate->low.denominator, valueUpdate->low.value, (int)valueUpdate->last.denominator, valueUpdate->last.value, (int)valueUpdate->open.denominator, valueUpdate->open.value, valueUpdate->total_volume, (int)valueUpdate->net_change.denominator, valueUpdate->net_change.value, valueUpdate->open_interest, (int)valueUpdate->tick.denominator, valueUpdate->tick.value, (int)valueUpdate->bid.denominator, valueUpdate->bid.value, (int)valueUpdate->ask.denominator, valueUpdate->ask.value, (int)valueUpdate->underlying_price.denominator, valueUpdate->underlying_price.value);

    return (0);
}

/* Instrument Status(0x90) */
int smt_0x90_decode(SMARTOPTION_TABLE *smt_table, unsigned char *msgb, int msgl)
{
    INSTRUMENT_STATUS *instrumentStatus = &smt_table->market_data.instrument_status;
    int offset = 0;

    // 1. Market Data Standard Header
    _smt_market_header_decode(smt_table, msgb, msgl);
    offset += MARKET_HEADER_TOTAL_LEN;

    // 2. Instrument Locate
    convert_big_endian_to_uint64_t(&msgb[offset], &instrumentStatus->instrument_locate, TRADE_ACTION_INSTRUMENT_LOCATE_LEN);
    offset += TRADE_ACTION_INSTRUMENT_LOCATE_LEN;

    // 3. Market Center Locate
    convert_big_endian_to_uint64_t(&msgb[offset], &instrumentStatus->market_center_locate, TRADE_ACTION_MARKET_CENTER_LOCATE_LEN);
    offset += TRADE_ACTION_MARKET_CENTER_LOCATE_LEN;

    // 4. Status Type
    convert_big_endian_to_uint64_t(&msgb[offset], &instrumentStatus->status_type, TRADE_ACTION_STATUS_TYPE_LEN);
    offset += TRADE_ACTION_STATUS_TYPE_LEN;

    // 5. Status Code
    convert_big_endian_to_uint64_t(&msgb[offset], &instrumentStatus->status_code, TRADE_ACTION_STATUS_CODE_LEN);
    offset += TRADE_ACTION_STATUS_CODE_LEN;

    // 6. Reason Code
    convert_big_endian_to_uint64_t(&msgb[offset], &instrumentStatus->reason_code, TRADE_ACTION_REASON_CODE_LEN);
    offset += TRADE_ACTION_REASON_CODE_LEN;

    // 7. Reason Detail Length
    instrumentStatus->reason_detail_length = (unsigned char)msgb[offset];
    offset += TRADE_ACTION_REASON_DETAIL_LENGTH_LEN;

    // 8. Reason Detail
    memcpy(instrumentStatus->reason_detail, &msgb[offset], instrumentStatus->reason_detail_length);
    offset += instrumentStatus->reason_detail_length;

    // 9. Appendage List
    _smt_appendage_list(smt_table, msgb, msgl, offset);

    // # Log Message
    sprintf(smt_table->logmsg, "Instrument Locate=%lu|Market Center Locate=%lu|Status Type=%lu|Status Code=%lu|Reason Code=%lu|Status Flags=0x%02X|Reason Detail=%s", instrumentStatus->instrument_locate, instrumentStatus->market_center_locate, instrumentStatus->status_type, instrumentStatus->status_code, instrumentStatus->reason_code, (int)instrumentStatus->status_flags, instrumentStatus->reason_detail);

    return (0);
}

/* Channel Event(0xB0) */
int smt_0xB0_decode(SMARTOPTION_TABLE *smt_table, unsigned char *msgb, int msgl)
{
    CHANNEL_EVENT *channelEvent = &smt_table->market_data.channel_event;
    int offset = 0;

    // 1. Market Data Standard Header
    _smt_market_header_decode(smt_table, msgb, msgl);
    offset += MARKET_HEADER_TOTAL_LEN;

    // 2. Instrument Locate
    memcpy(channelEvent->instrument_locate, &msgb[offset], CHANNEL_EVENT_INSTRUMENT_LOCATE_LEN);
    offset += CHANNEL_EVENT_INSTRUMENT_LOCATE_LEN;

    // 3. Market Center Locate
    convert_big_endian_to_uint64_t(&msgb[offset], &channelEvent->market_center_locate, CHANNEL_EVENT_MARKET_CENTER_LOCATE_LEN);

    // 4. Appendage List
    _smt_appendage_list(smt_table, msgb, msgl, offset);

    // # Log Message
    sprintf(smt_table->logmsg, "Instrument Locate=%s|Market Center Locate=%lu", channelEvent->instrument_locate, channelEvent->market_center_locate);

    return (0);
}

/* Administrative Text(0xB2) */
int smt_0xB2_decode(SMARTOPTION_TABLE *smt_table, unsigned char *msgb, int msgl)
{
    ADMINISTRATIVE_TEXT *admin = &smt_table->market_data.administrative_text;
    int offset = 0;

    // 1. Market Data Standard Header
    _smt_market_header_decode(smt_table, msgb, msgl);
    offset += MARKET_HEADER_TOTAL_LEN;

    // 2. Text Length
    convert_big_endian_to_uint64_t(&msgb[offset], &admin->text_length, ADMIN_TEXT_TEXT_LENGTH_LEN);
    offset += ADMIN_TEXT_TEXT_LENGTH_LEN;

    // 3. Text
    memcpy(admin->text, &msgb[offset], admin->text_length);

    // # Log Message
    sprintf(smt_table->logmsg, "Text Length=%lu|Text=%s", admin->text_length, admin->text);

    return (0);
}

/* Instrument Meta Data(0xC0) */
int smt_0xC0_decode(SMARTOPTION_TABLE *smt_table, unsigned char *msgb, int msgl)
{
    METADATA *meta = &smt_table->reference.meta_data;
    int offset = INSTRUMENT_META_DATA_MSG_TYPE_LEN;

    // 1. Locate Code
    convert_big_endian_to_uint64_t(&msgb[offset], &meta->locate_code, INSTRUMENT_META_DATA_LOCATE_CODE_LEN);
    offset += INSTRUMENT_META_DATA_LOCATE_CODE_LEN;

    // 2. Appendage List
    _smt_appendage_list(smt_table, msgb, msgl, offset);

    // # Log Message
    sprintf(smt_table->logmsg, "Locate Code=%lu|Parent Locate:%lu", meta->locate_code, meta->parent_symbol_locate);

    return (0);
}

/* Option delivery component(0xC3) */
int smt_0xC3_decode(SMARTOPTION_TABLE *smt_table, unsigned char *msgb, int msgl)
{
    OPTION_DELIVERY *delivery = &smt_table->reference.option_delivery;
    int offset = OPTION_DELIVERY_COMPONENT_MSG_TYPE_LEN;

    // 1. Root code locate
    convert_big_endian_to_uint64_t(&msgb[offset], &delivery->root_code_locate, OPTION_DELIVERY_COMPONENT_ROOT_CODE_LOCATE_LEN);
    offset += OPTION_DELIVERY_COMPONENT_ROOT_CODE_LOCATE_LEN;

    // 2. Component Index
    convert_big_endian_to_uint64_t(&msgb[offset], &delivery->component_index, OPTION_DELIVERY_COMPONENT_COMPONENT_INDEX_LEN);
    offset += OPTION_DELIVERY_COMPONENT_COMPONENT_INDEX_LEN;

    // 3. Component Total
    convert_big_endian_to_uint64_t(&msgb[offset], &delivery->component_total, OPTION_DELIVERY_COMPONENT_COMPONENT_TOTAL_LEN);
    offset += OPTION_DELIVERY_COMPONENT_COMPONENT_TOTAL_LEN;

    // 4. Deliverable Units
    convert_big_endian_to_uint64_t(&msgb[offset], &delivery->deliverable_units, OPTION_DELIVERY_COMPONENT_DELIVERABLE_UNITS_LEN);
    offset += OPTION_DELIVERY_COMPONENT_DELIVERABLE_UNITS_LEN;

    // 5. Settlement Method
    convert_big_endian_to_uint64_t(&msgb[offset], &delivery->settlement_method, OPTION_DELIVERY_COMPONENT_SETTLEMENT_METHOD_LEN);
    offset += OPTION_DELIVERY_COMPONENT_SETTLEMENT_METHOD_LEN;

    // 6. Fixed Amount Denominator
    convert_big_endian_to_uint64_t(&msgb[offset], &delivery->fixed_amount.denominator, OPTION_DELIVERY_COMPONENT_FIXED_AMOUNT_DENOMINATOR_LEN);
    offset += OPTION_DELIVERY_COMPONENT_FIXED_AMOUNT_DENOMINATOR_LEN;

    // 7. Fixed Amount Numerator
    int64_t amount;
    convert_big_endian_to_int64_t(&msgb[offset], &amount, OPTION_DELIVERY_COMPONENT_FIXED_AMOUNT_NUMERATOR_LEN);
    delivery->fixed_amount.value = (double)amount / (pow(10, delivery->fixed_amount.denominator));
    offset += OPTION_DELIVERY_COMPONENT_FIXED_AMOUNT_NUMERATOR_LEN;

    // 8. Currency Code
    memcpy(delivery->currency_code, &msgb[offset], OPTION_DELIVERY_COMPONENT_CURRENCY_CODE_LEN);
    offset += OPTION_DELIVERY_COMPONENT_CURRENCY_CODE_LEN;

    // 9. Strike Percent
    int64_t percent;
    delivery->strike_percent.denominator = 2;
    convert_big_endian_to_int64_t(&msgb[offset], &percent, OPTION_DELIVERY_COMPONENT_STRIKE_PERCENT_LEN);
    delivery->strike_percent.value = (double)percent / (pow(10, delivery->strike_percent.denominator));
    offset += OPTION_DELIVERY_COMPONENT_STRIKE_PERCENT_LEN;

    // 10. Component Symbol Locate
    convert_big_endian_to_uint64_t(&msgb[offset], &delivery->component_symbol_locate, OPTION_DELIVERY_COMPONENT_COMPONENT_SYMBOL_LOCATE_LEN);
    offset += OPTION_DELIVERY_COMPONENT_COMPONENT_SYMBOL_LOCATE_LEN;

    // # Log Message
    sprintf(smt_table->logmsg, "Root Code Locate=%lu|Component Index=%lu|Component Total=%lu|Deliverable Units=%lu|Settlement Method=%lu|Fixed Amound Denominator=%lu|Fixed Amount Numerator=%.*f|Currency Code=%s|Strike Percent=%.*f|Component Symbol Locate=%lu", delivery->root_code_locate, delivery->component_index, delivery->component_total, delivery->deliverable_units, delivery->settlement_method, delivery->fixed_amount.denominator, (int)delivery->fixed_amount.denominator, delivery->fixed_amount.value, delivery->currency_code, (int)delivery->strike_percent.denominator, delivery->strike_percent.value, delivery->component_symbol_locate);

    return (0);
}

int smt_decode(SMARTOPTION_TABLE *smt_table, unsigned int msgtype, unsigned char *msgb, size_t msgl)
{
    memset(smt_table, 0x00, sizeof(SMARTOPTION_TABLE));
    smt_table->type = msgtype;

    switch (smt_table->type)
    {
    case SYSTEM_EVENT_MSG_TYPE: // 0x20
        strcpy(smt_table->name, "System Event");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_0x20_decode(smt_table, msgb, msgl);
        break;
    case CHANNEL_SECONDS_MSG_TYPE: // 0x22
        strcpy(smt_table->name, "Channel Seconds");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_0x22_decode(smt_table, msgb, msgl);
        break;
    case MARKET_CENTER_LOCATE_MSG_TYPE: // 0x30
        strcpy(smt_table->name, "Market Center Locate");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_0x30_decode(smt_table, msgb, msgl);
        break;
    case INSTRUMENT_LOCATE_MSG_TYPE: // 0x33
        strcpy(smt_table->name, "Instrument Symbol Locate");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 1;
        smt_0x33_decode(smt_table, msgb, msgl);
        break;
    case SHORT_2_SIDED_NBBO_MSG_TYPE: // 0x60
        strcpy(smt_table->name, "NBBO(S2)");
        smt_table->loglevel = FL_DEBUG;
        smt_table->logflag = 0;
        smt_table->market_data.nbbo.nside = 2;
        smt_nbbo_decode(smt_table, msgb, msgl);
        break;
    case LONG_2_SIDED_NBBO_MSG_TYPE: // 0x61
        strcpy(smt_table->name, "NBBO(L2)");
        smt_table->loglevel = FL_DEBUG;
        smt_table->logflag = 0;
        smt_table->market_data.nbbo.nside = 2;
        smt_nbbo_decode(smt_table, msgb, msgl);
        break;
    case EXTENDED_2_SIDED_NBBO_MSG_TYPE: // 0x62
        strcpy(smt_table->name, "NBBO(E2)");
        smt_table->loglevel = FL_DEBUG;
        smt_table->logflag = 0;
        smt_table->market_data.nbbo.nside = 2;
        smt_nbbo_decode(smt_table, msgb, msgl);
        break;
    case SHORT_1_SIDED_NBBO_MSG_TYPE: // 0x63
        strcpy(smt_table->name, "NBBO(S1)");
        smt_table->loglevel = FL_DEBUG;
        smt_table->logflag = 0;
        smt_table->market_data.nbbo.nside = 1;
        smt_nbbo_decode(smt_table, msgb, msgl);
        break;
    case LONG_1_SIDED_NBBO_MSG_TYPE: // 0x64
        strcpy(smt_table->name, "NBBO(L1)");
        smt_table->loglevel = FL_DEBUG;
        smt_table->logflag = 0;
        smt_table->market_data.nbbo.nside = 1;
        smt_nbbo_decode(smt_table, msgb, msgl);
        break;
    case EXTENDED_1_SIDED_NBBO_MSG_TYPE: // 0x65
        strcpy(smt_table->name, "NBBO(E1)");
        smt_table->loglevel = FL_DEBUG;
        smt_table->logflag = 0;
        smt_table->market_data.nbbo.nside = 1;
        smt_nbbo_decode(smt_table, msgb, msgl);
        break;
    case SHORT_TRADE_MSG_TYPE: // 0x70
        strcpy(smt_table->name, "Trade(S1)");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_trade_decode(smt_table, msgb, msgl);
        break;
    case LONG_TRADE_MSG_TYPE: // 0x71
        strcpy(smt_table->name, "Trade(L1)");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_trade_decode(smt_table, msgb, msgl);
        break;
    case EXTENDED_TRADE_MSG_TYPE: // 0x72
        strcpy(smt_table->name, "Trade(E1)");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_trade_decode(smt_table, msgb, msgl);
        break;
    case TRADE_CANCEL_MSG_TYPE: // 0x73
        strcpy(smt_table->name, "Cancel");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_trade_decode(smt_table, msgb, msgl);
        break;
    case VALUE_UPDATE_MSG_TYPE: // 0x80
        strcpy(smt_table->name, "Value Update");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_0x80_decode(smt_table, msgb, msgl);
        break;
    case TRADE_ACTION_MSG_TYPE: // 0x90
        strcpy(smt_table->name, "Trade Action");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_0x90_decode(smt_table, msgb, msgl);
        break;
    case CHANNEL_EVENT_MSG_TYPE: // 0xB0
        strcpy(smt_table->name, "Channel Event");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_0xB0_decode(smt_table, msgb, msgl);
        break;
    case ADMIN_TEXT_MSG_TYPE: // 0xB2
        strcpy(smt_table->name, "Admin Text");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_0xB2_decode(smt_table, msgb, msgl);
        break;
    case INSTRUMENT_META_DATA_MSG_TYPE: // 0xC0
        strcpy(smt_table->name, "Instrument Meta Data");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_0xC0_decode(smt_table, msgb, msgl);
        break;
    case OPTION_DELIVERY_COMPONENT_MSG_TYPE: // 0xC3
        strcpy(smt_table->name, "Option Delivery Component");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_0xC3_decode(smt_table, msgb, msgl);
        break;
    default:
        sprintf(smt_table->logmsg, "Unknown Type(0x%02X)", (int)smt_table->type);
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        return (0);
    }

    return (0);
}