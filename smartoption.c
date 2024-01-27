#include "nastag.h"
#include "smart.h"

/* Appendage List */
int get_append_header(char *msgb, AppendHeader *header)
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

int get_append_decimal(char *msgb, AppendHeader *header, double *value, uint64_t *denominator)
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

int get_append_numeric(char *msgb, AppendHeader *header, int64_t *valueInt, uint64_t *valueUint)
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

int get_append_date(char *msgb, AppendHeader *header, uint64_t *date)
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

int _smt_appendage_list(SMARTOPTION_TABLE *smt_table, char *msgb, int msgl, int *offset)
{
    InstrumentLocate *instrumentLocate = &smt_table->instrument_locate;
    TRADE *trade = &smt_table->market_data.trade;
    VALUE_UPDATE *valueUpdate = &smt_table->market_data.value_update;
    METADATA *meta = &smt_table->reference.meta_data;
    AppendHeader header;

    while (*offset < (msgl - 1))
    {
        memset(&header, 0x00, sizeof(AppendHeader));

        // 1. Header
        get_append_header(&msgb[*offset], &header);
        *offset += (APPEND_ELEMENT_LENGTH_LEN + APPEND_FORMATCODE_LEN + APPEND_VALUETYPECODE_LEN);

        // 2. Element
        switch (header.value_type_code)
        {
        case ROOT_SYMBOL_VALUE_TYPE: // 1 / String / Instrument Locate
            if (header.format_code != STRING_FORMAT_CODE)
                break;
            memcpy(instrumentLocate->root, &msgb[*offset], header.element_length);
            break;
        case PUT_CALL_VALUE_TYPE: // 2 /Char / Instrument Locate
            if (header.format_code != CHAR_FORMAT_CODE)
                break;
            memcpy(instrumentLocate->put_or_call, &msgb[*offset], header.element_length);
            break;
        case EXPIRATION_DATE_VALUE_TYPE: // 3 / Date / Instrument Locate
            get_append_date(&msgb[*offset], &header, &instrumentLocate->expiration_date);
            break;
        case STRIKE_PRICE_VALUE_TYPE: // 4 / Decimal / Insturment Locate
            get_append_decimal(&msgb[*offset], &header, &instrumentLocate->strike.value, &instrumentLocate->strike.denominator);
            break;
        case HIGH_PRICE_VALUE_TYPE: // 65 / Decimal / Value Update
            get_append_decimal(&msgb[*offset], &header, &valueUpdate->high.value, &valueUpdate->high.denominator);
            break;
        case LOW_PRICE_VALUE_TYPE: // 66 / Decimal / Value Update
            get_append_decimal(&msgb[*offset], &header, &valueUpdate->low.value, &valueUpdate->low.denominator);
            break;
        case LAST_PRICE_VALUE_TYPE: // 67 / Decimal / Value Update
            get_append_decimal(&msgb[*offset], &header, &valueUpdate->last.value, &valueUpdate->last.denominator);
            break;
        case OPEN_PRICE_VALUE_TYPE: // 68 / Decimal / Value Update
            get_append_decimal(&msgb[*offset], &header, &valueUpdate->open.value, &valueUpdate->open.denominator);
            break;
        case TOTAL_VOLUME_VALUE_TYPE: // 70 / Numeric / Value Update
            get_append_numeric(&msgb[*offset], &header, NULL, &valueUpdate->total_volume);
            break;
        case NET_CHANGE_VALUE_TYPE: // 71 / Decimal / Value Update
            get_append_decimal(&msgb[*offset], &header, &valueUpdate->net_change.value, &valueUpdate->net_change.denominator);
            break;
        case OPEN_INTEREST_VALUE_TYPE: // 72 / Numeric / Value Update
            get_append_numeric(&msgb[*offset], &header, NULL, &valueUpdate->open_interest);
            break;
        case TICK_VALUE_TYPE: // 73 / Decimal / Value Update
            get_append_decimal(&msgb[*offset], &header, &valueUpdate->tick.value, &valueUpdate->tick.denominator);
            break;
        case BID_VALUE_TYPE: // 74 / Decimal / Value Update
            get_append_decimal(&msgb[*offset], &header, &valueUpdate->bid.value, &valueUpdate->bid.denominator);
            break;
        case ASK_VALUE_TYPE: // 75 / Decimal / Value Update
            get_append_decimal(&msgb[*offset], &header, &valueUpdate->ask.value, &valueUpdate->ask.denominator);
            break;
        case PARENT_SYMBOL_LOCATE_VALUE_TYPE: //  81 / Numeric / Instrument Locate
            get_append_numeric(&msgb[*offset], &header, NULL, &instrumentLocate->parent_locate_code);
            get_append_numeric(&msgb[*offset], &header, NULL, &meta->parent_symbol_locate);
            break;
        case UNDERLYING_PRICE_VALUE_TYPE: // 100 / Decimal / Value Update
            get_append_decimal(&msgb[*offset], &header, &valueUpdate->underlying_price.value, &valueUpdate->underlying_price.denominator);
            break;
        case UPSTREAM_CONDITION_DETAILS_VALUE_TYPE: // 143 / String / Trades, Trade Cancel
            if (header.format_code != STRING_FORMAT_CODE)
                break;
            memcpy(trade->upstream_condition_detail, &msgb[*offset], header.element_length);
            break;
        case TRADE_REPORT_DETAIL_VALUE_TYPE: // 145 / Byte Enum / Trades, Trade Cancel
            if (header.format_code != BYTE_VALUE_FORMAT_CODE)
                break;
            trade->trade_report_detail = (unsigned char)msgb[*offset];
            break;
        case EXTENDED_REPORT_FLAGS_VALUE_TYPE: // 146 / Numeric / Trades, Trade Cancel
            get_append_numeric(&msgb[*offset], &header, NULL, &trade->extended_report_flags);
            break;
        default:
            break;
        }
        *offset += header.element_length;
    }

    return (0);
}

void _smt_decode_field(FIXEDFLD *fixedfld, char *msgb, int *offset, int fldnum)
{
    msg2fixedfld(&fixedfld[fldnum], msgb, *offset);
    *offset += *(fixedfld[fldnum].field_length);
}

void _smt_decode_fields(FIXEDFLD *fixedfld, char *msgb, int *offset)
{
    int fldnum = 0;

    // Iterate through fields and decode
    while (fixedfld[fldnum].field_length != NULL)
    {
        _smt_decode_field(fixedfld, msgb, offset, fldnum);
        fldnum++;
    }
}

/* Makret Data Header */
int _smt_market_header_decode(SMARTOPTION_TABLE *smt_table, char *msgb, int msgl)
{
    MarketDataHeader *header = &smt_table->market_data.header;
    int offset = MARKET_HEADER_MSG_TYPE_LEN;
    FIXEDFLD fixedfld[] = {
        {"Message Type", FIXEDFLD_BITMASK, &MARKET_HEADER_MSG_TYPE_LEN, (void *)&header->msgtype},
        {"Protocol ID", FIXEDFLD_UINT, &MARKET_HEADER_PROTOCOLID_LEN, (void *)&header->protocol_id},
        {"Channel Index", FIXEDFLD_UINT, &MARKET_HEADER_CHANNEL_INDEX_LEN, (void *)&header->channel_index},
        {"Message Flags", FIXEDFLD_BITMASK, &MARKET_HEADER_CHANNEL_INDEX_LEN, (void *)&header->message_flag},
        {"Upstream Sequence Number", FIXEDFLD_UINT, &MARKET_HEADER_UPSTREAM_SEQN_LEN, (void *)&header->upstream_seqn},
        {"Upstream Nanos", FIXEDFLD_UINT, &MARKET_HEADER_UPSTREAM_NANOS_LEN, (void *)&header->upstream_nanos},
        {NULL, 0, NULL, NULL} // 마지막 요소는 중괄호 없이 초기화
    };

    // Decode fields
    _smt_decode_fields(fixedfld, msgb, &offset);

    // # Log Message(CSV)
    _smt_csv(smt_table, fixedfld);

    return (0);
}

int smt_all_decode(SMARTOPTION_TABLE *smt_table, FIXEDFLD *fixedfld, char *msgb, int msgl, int is_market)
{
    int offset = 0;

    if (is_market)
    {
        _smt_market_header_decode(smt_table, msgb, msgl);
        offset += MARKET_HEADER_TOTAL_LEN;
    }

    // Decode fields
    _smt_decode_fields(fixedfld, msgb, &offset);

    // Appendage List
    _smt_appendage_list(smt_table, msgb, msgl, &offset);

    // Log Message(CSV)
    _smt_csv(smt_table, fixedfld);

    return 0;
}

/* Market Data: NBBO */
int smt_nbbo_decode(SMARTOPTION_TABLE *smt_table, char *msgb, int msgl)
{
    NBBO *nbbo = &smt_table->market_data.nbbo;
    int offset = 0;
    int ii = 0;
    int price_size = 0, size_size = 0;
    NBBO_DEPTH depth;
    int64_t price;
    FIXEDFLD fixedfld[] = {
        {"Instrument Locate", FIXEDFLD_UINT, &NBBO_INSTRUMENT_LOCATE_LEN, (void *)&nbbo->instrument_locate},
        {"Bid Market Center Locate", FIXEDFLD_UINT, &NBBO_DEPTH_MARKET_CENTER_LEN, (void *)&nbbo->bid.market_center_locate},
        {"Bid Denominator", FIXEDFLD_UINT, &NBBO_DEPTH_DENOMINATOR_LEN, (void *)&nbbo->bid.price.denominator},
        {"Bid Price", FIXEDFLD_DECIMAL, &price_size, (void *)&nbbo->bid.price},
        {"Bid Size", FIXEDFLD_UINT, &size_size, (void *)&nbbo->bid.size},
        {"Ask Market Center Locate", &NBBO_DEPTH_MARKET_CENTER_LEN, (void *)&nbbo->ask.market_center_locate},
        {"Ask Denominator", FIXEDFLD_UINT, &NBBO_DEPTH_DENOMINATOR_LEN, (void *)&nbbo->ask.price.denominator},
        {"Ask Price", FIXEDFLD_DECIMAL, &price_size, (void *)&nbbo->ask.price},
        {"Ask Size", FIXEDFLD_UINT, &size_size, (void *)&nbbo->ask.size},
        {"RFU/Condition", FIXEDFLD_BITMASK, &NBBO_RFU_LEN, (void *)&nbbo->condition},
        {"Flags", FIXEDFLD_BITMASK, &NBBO_FLAGS_LEN, (void *)&nbbo->flags},
        {NULL, 0, NULL, NULL}};

    // 1. Market Data Standard Header
    _smt_market_header_decode(smt_table, msgb, msgl);
    offset += MARKET_HEADER_TOTAL_LEN;

    // 2. Instrument Locate
    _smt_decode_field(fixedfld, msgb, &offset, 0);

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
            price_size = 8;
            size_size = 4;
            convert_big_endian_to_uint64_t(&msgb[offset], &depth.price.denominator, NBBO_DEPTH_DENOMINATOR_LEN);
            offset += NBBO_DEPTH_DENOMINATOR_LEN;
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
        if (nbbo->nside == 1)
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
    _smt_decode_field(fixedfld, msgb, &offset, 9);

    // 5. Flags
    _smt_decode_field(fixedfld, msgb, &offset, 10);

    // 6. Appendage List(NBBO does not support appendage)
    //_smt_appendage_list(smt_table, msgb, msgl, offset);

    // # Log Message(CSV)
    _smt_csv(smt_table, fixedfld);

    return (0);
}

/* Market Data: TRADE */
int smt_trade_decode(SMARTOPTION_TABLE *smt_table, char *msgb, int msgl)
{
    TRADE *trade = &smt_table->market_data.trade;
    int offset = 0;
    int64_t price;
    int price_size = 0;
    int size_size = 0;
    FIXEDFLD fixedfld[] = {
        {"Instrument Locate", FIXEDFLD_UINT, &TRADE_INSTRUMENT_LOCATE_LEN, (void *)&trade->instrument_locate},
        {"Market Center Locate", FIXEDFLD_UINT, &TRADE_INSTRUMENT_LOCATE_LEN, (void *)&trade->market_center_locate},
        {"Trade ID", FIXEDFLD_UINT, &TRADE_TRADE_ID_LEN, (void *)&trade->trade_id},
        {"Price Denominator", FIXEDFLD_UINT, &TRADE_DENOMINATOR_LEN, (void *)&trade->price.denominator},
        {"Price", FIXEDFLD_DECIMAL, &price_size, (void *)&trade->price},
        {"Size", FIXEDFLD_UINT, &size_size, (void *)&trade->size},
        {"Price Flags", FIXEDFLD_BITMASK, &TRADE_PRICE_FLAGS_LEN, (void *)&trade->price_flags},
        {"Eligibility Flags", FIXEDFLD_BITMASK, &TRADE_ELIGIBILITY_FLAGS_LEN, (void *)&trade->eligibility_flags},
        {"Report Flags", FIXEDFLD_BITMASK, &TRADE_REPORT_FLAGS_LEN, (void *)&trade->report_flags},
        {"Change Flags", FIXEDFLD_BITMASK, &TRADE_CHANGE_FLAGS_LEN, (void *)&trade->change_flags},
        {"Cancel Flags", FIXEDFLD_BITMASK, &TRADE_CANCEL_FLAGS_LEN, (void *)&trade->cancel_flags},
        {"Upstream Condition Detail", FIXEDFLD_STRING, NULL, (void *)&trade->upstream_condition_detail},
        {"Trade Report Detail", FIXEDFLD_UINT, NULL, (void *)&trade->trade_report_detail},
        {"Extended Report Flag", FIXEDFLD_BITMASK, NULL, (void *)&trade->extended_report_flags},
        {NULL, 0, NULL, NULL}};

    // 1. Market Data Standard Header
    _smt_market_header_decode(smt_table, msgb, msgl);
    offset += MARKET_HEADER_TOTAL_LEN;

    // 2. Instrument Locate
    _smt_decode_field(fixedfld, msgb, &offset, 0);

    // 3. Market Center Locate
    _smt_decode_field(fixedfld, msgb, &offset, 1);

    // 4. Trade ID
    _smt_decode_field(fixedfld, msgb, &offset, 2);

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
        _smt_decode_field(fixedfld, msgb, &offset, 3);
        break;
    case TRADE_CANCEL_MSG_TYPE:
        // 5. Denominator
        price_size = 8;
        size_size = 8;
        trade->trade_type = CANCEL_TRADE_TYPE;
        _smt_decode_field(fixedfld, msgb, &offset, 3);
        break;
    default:
        return (-1);
    }

    // 6. Price
    _smt_decode_field(fixedfld, msgb, &offset, 4);

    // 7. Size
    _smt_decode_field(fixedfld, msgb, &offset, 5);

    // 8. Price Flags
    _smt_decode_field(fixedfld, msgb, &offset, 6);

    // 9. Eligibility Flags
    _smt_decode_field(fixedfld, msgb, &offset, 7);

    // 10. Report Flags
    _smt_decode_field(fixedfld, msgb, &offset, 8);

    // 11. Change Flags / Cancel Flags
    if (trade->trade_type != CANCEL_TRADE_TYPE)
    {
        _smt_decode_field(fixedfld, msgb, &offset, 9);
    }
    else
    {
        _smt_decode_field(fixedfld, msgb, &offset, 10);
    }

    // 12. Appendage List
    _smt_appendage_list(smt_table, msgb, msgl, offset);

    // # Log Message(CSV)
    _smt_csv(smt_table, fixedfld);

    return (0);
}

int smt_decode(SMARTOPTION_TABLE *smt_table, unsigned int msgtype, char *msgb, size_t msgl)
{
    int is_market = 0;

    memset(smt_table, 0x00, sizeof(SMARTOPTION_TABLE));
    smt_table->type = msgtype;

    switch (smt_table->type)
    {
    case SYSTEM_EVENT_MSG_TYPE: // 0x20
    {
        strcpy(smt_table->name, "System Event");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        FIXEDFLD fixedfld[] = {
            {"Message Type", FIXEDFLD_BITMASK, &SYSTEM_EVENT_MSG_TYPE_LEN, (void *)&smt_table->system_event.msgtype},
            {"Timestamp", FIXEDFLD_UINT, &SYSTEM_EVENT_TIMESTAMP_LEN, (void *)&smt_table->system_event.timestamp},
            {"Event Code", FIXEDFLD_STRING, &SYSTEM_EVENT_EVENTCODE_LEN, (void *)&smt_table->system_event.event_code},
            {NULL, 0, NULL, NULL}};
        smt_all_decode(smt_table, fixedfld, msgb, msgl, is_market);	
        break;
    }
    case CHANNEL_SECONDS_MSG_TYPE: // 0x22
    {
        strcpy(smt_table->name, "Channel Seconds");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        FIXEDFLD fixedfld[] = {
            {"Message Type", FIXEDFLD_BITMASK, &CHANNEL_SECONDS_MSG_TYPE_LEN, (void *)&smt_table->channel_seconds.msgtype},
            {"Protocol ID", FIXEDFLD_UINT, &CHANNEL_SECONDS_PROTOCOL_ID_LEN, (void *)&smt_table->channel_seconds.protocol_id},
            {"Channel Index", FIXEDFLD_UINT, &CHANNEL_SECONDS_CHANNEL_INDEX_LEN, (void *)&smt_table->channel_seconds.channel_index},
            {"Seconds", FIXEDFLD_UINT, &CHANNEL_SECONDS_SECONDS_LEN, (void *)&smt_table->channel_seconds.seconds},
            {NULL, 0, NULL, NULL}};
        smt_all_decode(smt_table, fixedfld, msgb, msgl, is_market);
        break;
    }
    case MARKET_CENTER_LOCATE_MSG_TYPE: // 0x30
    {
        strcpy(smt_table->name, "Market Center Locate");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        FIXEDFLD fixedfld[] = {
            {"Message Type", FIXEDFLD_BITMASK, &MARKET_CENTER_LOCATE_MSG_TYPE_LEN, (void *)&smt_table->market_center_locate.msgtype},
            {"Locate Code", FIXEDFLD_UINT, &MARKET_CENTER_LOCATE_LOCATECODE_LEN, (void *)&smt_table->market_center_locate.locate_code},
            {"MIC", FIXEDFLD_STRING, &MARKET_CENTER_LOCATE_MIC_LEN, (void *)&smt_table->market_center_locate.MIC},
            {NULL, 0, NULL, NULL}};
        smt_all_decode(smt_table, fixedfld, msgb, msgl, is_market);
        break;
    }
    case INSTRUMENT_LOCATE_MSG_TYPE: // 0x33
    {
        strcpy(smt_table->name, "Instrument Symbol Locate");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 1;
        FIXEDFLD fixedfld[] = {
            {"Message Type", FIXEDFLD_BITMASK, &MARKET_CENTER_LOCATE_MSG_TYPE_LEN, (void *)&smt_table->instrument_locate.msgtype},
            {"Locate Code", FIXEDFLD_UINT, &INSTRUMENT_LOCATE_LOCATECODE_LEN, (void *)&smt_table->instrument_locate.locate_code},
            {"Country Code", FIXEDFLD_STRING, &INSTRUMENT_LOCATE_COUNTRYCODE_LEN, (void *)smt_table->instrument_locate.country_code},
            {"Currecny Code", FIXEDFLD_STRING, &INSTRUMENT_LOCATE_CURRENCYCODE_LEN, (void *)smt_table->instrument_locate.currency_code},
            {"MIC", FIXEDFLD_STRING, &INSTRUMENT_LOCATE_MIC_LEN, (void *)smt_table->instrument_locate.MIC},
            {"Product Type", FIXEDFLD_UINT, &INSTRUMENT_LOCATE_PRODUCTTYPE_LEN, (void *)&smt_table->instrument_locate.product_type},
            {"Symbol Length", FIXEDFLD_UINT, &INSTRUMENT_LOCATE_SYMBOLLENGTH_LEN, (void *)&smt_table->instrument_locate.symbol_length},
            {"Symbol", FIXEDFLD_STRING, &smt_table->instrument_locate.symbol_length, (void *)smt_table->instrument_locate.symbol},
            {"Root Symbol", FIXEDFLD_STRING, NULL, (void *)smt_table->instrument_locate.root},
            {"Put/Call", FIXEDFLD_STRING, NULL, (void *)smt_table->instrument_locate.put_or_call},
            {"Expiration Date", FIXEDFLD_UINT, NULL, (void *)&smt_table->instrument_locate.expiration_date},
            {"Strike Price", FIXEDFLD_DECIMAL, NULL, (void *)&smt_table->instrument_locate.strike},
            {"Parent Locate Code", FIXEDFLD_UINT, NULL, (void *)&smt_table->instrument_locate.parent_locate_code},
            {NULL, 0, NULL, NULL}};
        smt_all_decode(smt_table, fixedfld, msgb, msgl, is_market);
        break;
    }
    case SHORT_2_SIDED_NBBO_MSG_TYPE: // 0x60
    {
        strcpy(smt_table->name, "NBBO(S2)");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_table->market_data.nbbo.nside = 2;
        smt_nbbo_decode(smt_table, msgb, msgl);
        break;
    }
    case LONG_2_SIDED_NBBO_MSG_TYPE: // 0x61
    {
        strcpy(smt_table->name, "NBBO(L2)");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_table->market_data.nbbo.nside = 2;
        smt_nbbo_decode(smt_table, msgb, msgl);
        break;
    }
    case EXTENDED_2_SIDED_NBBO_MSG_TYPE: // 0x62
    {
        strcpy(smt_table->name, "NBBO(E2)");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_table->market_data.nbbo.nside = 2;
        smt_nbbo_decode(smt_table, msgb, msgl);
        break;
    }
    case SHORT_1_SIDED_NBBO_MSG_TYPE: // 0x63
    {
        strcpy(smt_table->name, "NBBO(S1)");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_table->market_data.nbbo.nside = 1;
        smt_nbbo_decode(smt_table, msgb, msgl);
        break;
    }
    case LONG_1_SIDED_NBBO_MSG_TYPE: // 0x64
    {
        strcpy(smt_table->name, "NBBO(L1)");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_table->market_data.nbbo.nside = 1;
        smt_nbbo_decode(smt_table, msgb, msgl);
        break;
    }
    case EXTENDED_1_SIDED_NBBO_MSG_TYPE: // 0x65
    {
        strcpy(smt_table->name, "NBBO(E1)");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_table->market_data.nbbo.nside = 1;
        smt_nbbo_decode(smt_table, msgb, msgl);
        break;
    }
    case SHORT_TRADE_MSG_TYPE: // 0x70
    {
        strcpy(smt_table->name, "Trade(S1)");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_trade_decode(smt_table, msgb, msgl);
        break;
    }
    case LONG_TRADE_MSG_TYPE: // 0x71
    {
        strcpy(smt_table->name, "Trade(L1)");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_trade_decode(smt_table, msgb, msgl);
        break;
    }
    case EXTENDED_TRADE_MSG_TYPE: // 0x72
    {
        strcpy(smt_table->name, "Trade(E1)");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_trade_decode(smt_table, msgb, msgl);
        break;
    }
    case TRADE_CANCEL_MSG_TYPE: // 0x73
    {
        strcpy(smt_table->name, "Cancel");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        smt_trade_decode(smt_table, msgb, msgl);
        break;
    }
    case VALUE_UPDATE_MSG_TYPE: // 0x80
    {
        strcpy(smt_table->name, "Value Update");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        is_market = 1;
        FIXEDFLD fixedfld[] = {
            {"Instrument Locate", FIXEDFLD_UINT, &VALUE_UPDATE_INSTRUMENT_LOCATE_LEN, (void *)&smt_table->market_data.value_update.instrument_locate},
            {"Market Center Locate", FIXEDFLD_UINT, &VALUE_UPDATE_MARKET_CENTER_LOCATE_LEN, (void *)&smt_table->market_data.value_update.market_center_locate},
            {"Value Update Flags", FIXEDFLD_BITMASK, &VALUE_UPDATE_VALUE_UPDATE_FLAGS_LEN, (void *)&smt_table->market_data.value_update.value_update_flags},
            {"High Price", FIXEDFLD_DECIMAL, NULL, (void *)&smt_table->market_data.value_update.high},
            {"Low Price", FIXEDFLD_DECIMAL, NULL, (void *)&smt_table->market_data.value_update.low},
            {"Last Price", FIXEDFLD_DECIMAL, NULL, (void *)&smt_table->market_data.value_update.last},
            {"Open Price", FIXEDFLD_DECIMAL, NULL, (void *)&smt_table->market_data.value_update.open},
            {"Total Volume", FIXEDFLD_UINT, NULL, (void *)&smt_table->market_data.value_update.total_volume},
            {"Net Change", FIXEDFLD_DECIMAL, NULL, (void *)&smt_table->market_data.value_update.net_change},
            {"Open Interest", FIXEDFLD_UINT, NULL, (void *)&smt_table->market_data.value_update.open_interest},
            {"Tick", FIXEDFLD_DECIMAL, NULL, (void *)&smt_table->market_data.value_update.tick},
            {"Bid", FIXEDFLD_DECIMAL, NULL, (void *)&smt_table->market_data.value_update.bid},
            {"Ask", FIXEDFLD_DECIMAL, NULL, (void *)&smt_table->market_data.value_update.ask},
            {"Underlying Price", FIXEDFLD_DECIMAL, NULL, (void *)&smt_table->market_data.value_update.underlying_price},
            {NULL, 0, NULL, NULL}};
        smt_all_decode(smt_table, fixedfld, msgb, msgl, is_market);
        break;
    }
    case INSTRUMENT_STATUS_MSG_TYPE: // 0x90
    {
        strcpy(smt_table->name, "Trade Action");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        is_market = 1;
        FIXEDFLD fixedfld[] = {
            {"Instrument Locate", FIXEDFLD_UINT, &INSTRUMENT_STATUS_INSTRUMENT_LOCATE_LEN, (void *)&smt_table->market_data.instrument_status.instrument_locate},
            {"Market Center Locate", FIXEDFLD_UINT, &INSTRUMENT_STATUS_MARKET_CENTER_LOCATE_LEN, (void *)&smt_table->market_data.instrument_status.market_center_locate},
            {"Status Type", FIXEDFLD_UINT, &INSTRUMENT_STATUS_STATUS_TYPE_LEN, (void *)&smt_table->market_data.instrument_status.status_type},
            {"Status Code", FIXEDFLD_UINT, &INSTRUMENT_STATUS_STATUS_CODE_LEN, (void *)&smt_table->market_data.instrument_status.status_code},
            {"Reason Code", FIXEDFLD_UINT, &INSTRUMENT_STATUS_REASON_CODE_LEN, (void *)&smt_table->market_data.instrument_status.reason_code},
            {"Status Flags", FIXEDFLD_BITMASK, &INSTRUMENT_STATUS_STATUS_FLAGS_LEN, (void *)&smt_table->market_data.instrument_status.status_flags},
            {"Reason Detail Length", FIXEDFLD_UINT, &INSTRUMENT_STATUS_REASON_DETAIL_LENGTH_LEN, (void *)&smt_table->market_data.instrument_status.reason_detail_length},
            {"Reason Detail", FIXEDFLD_STRING, &smt_table->market_data.instrument_status.reason_detail_length, (void *)smt_table->market_data.instrument_status.reason_detail},
            {NULL, 0, NULL, NULL}};
        smt_all_decode(smt_table, fixedfld, msgb, msgl, is_market);
        break;
    }
    case CHANNEL_EVENT_MSG_TYPE: // 0xB0
    {
        strcpy(smt_table->name, "Channel Event");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        is_market = 1;
        FIXEDFLD fixedfld[] = {
            {"Instrument Locate", FIXEDFLD_CHAR, &CHANNEL_EVENT_INSTRUMENT_LOCATE_LEN, (void *)smt_table->market_data.channel_event.instrument_locate},
            {"Market Center Locate", FIXEDFLD_UINT, &CHANNEL_EVENT_MARKET_CENTER_LOCATE_LEN, (void *)&smt_table->market_data.channel_event.market_center_locate},
            {NULL, 0, NULL, NULL}};
        smt_all_decode(smt_table, fixedfld, msgb, msgl, is_market);
        break;
    }
    case ADMIN_TEXT_MSG_TYPE: // 0xB2
    {
        strcpy(smt_table->name, "Admin Text");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        is_market = 1;
        FIXEDFLD fixedfld[] = {
            {"Text Length", FIXEDFLD_UINT, &ADMIN_TEXT_TEXT_LENGTH_LEN, (void *)&smt_table->market_data.administrative_text.text_length},
            {"Text", FIXEDFLD_STRING, &smt_table->market_data.administrative_text.text_length, (void *)smt_table->market_data.administrative_text.text},
            {NULL, 0, NULL, NULL}};
        smt_all_decode(smt_table, fixedfld, msgb, msgl, is_market);
        break;
    }
    case INSTRUMENT_META_DATA_MSG_TYPE: // 0xC0
    {
        strcpy(smt_table->name, "Instrument Meta Data");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        FIXEDFLD fixedfld[] = {
            {"Message Type", FIXEDFLD_BITMASK, &INSTRUMENT_META_DATA_MSG_TYPE_LEN, (void *)&smt_table->reference.meta_data.msgtype},
            {"Locate Code", FIXEDFLD_UINT, &INSTRUMENT_META_DATA_LOCATE_CODE_LEN, (void *)&smt_table->reference.meta_data.locate_code},
            {"Parent Symbol Locate", FIXEDFLD_UINT, NULL, (void *)&smt_table->reference.meta_data.parent_symbol_locate},
            {NULL, 0, NULL, NULL}};
        smt_all_decode(smt_table, fixedfld, msgb, msgl, is_market);
        break;
    }
    case OPTION_DELIVERY_COMPONENT_MSG_TYPE: // 0xC3
    {
        strcpy(smt_table->name, "Option Delivery Component");
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        FIXEDFLD fixedfld[] = {
            {"Message Type", FIXEDFLD_BITMASK, &OPTION_DELIVERY_COMPONENT_MSG_TYPE_LEN, (void *)&smt_table->reference.option_delivery.msgtype},
            {"Root Code Locate", FIXEDFLD_UINT, &OPTION_DELIVERY_COMPONENT_ROOT_CODE_LOCATE_LEN, (void *)&smt_table->reference.option_delivery.root_code_locate},
            {"Component Index", FIXEDFLD_UINT, &OPTION_DELIVERY_COMPONENT_COMPONENT_INDEX_LEN, (void *)&smt_table->reference.option_delivery.component_index},
            {"Component Total", FIXEDFLD_UINT, &OPTION_DELIVERY_COMPONENT_COMPONENT_TOTAL_LEN, (void *)&smt_table->reference.option_delivery.component_total},
            {"Deliverable Units", FIXEDFLD_UINT, &OPTION_DELIVERY_COMPONENT_DELIVERABLE_UNITS_LEN, (void *)&smt_table->reference.option_delivery.deliverable_units},
            {"Settlement Method", FIXEDFLD_UINT, &OPTION_DELIVERY_COMPONENT_SETTLEMENT_METHOD_LEN, (void *)&smt_table->reference.option_delivery.settlement_method},
            {"Fixed Amount Denominator", FIXEDFLD_UINT, &OPTION_DELIVERY_COMPONENT_FIXED_AMOUNT_DENOMINATOR_LEN, (void *)&smt_table->reference.option_delivery.fixed_amount_denominator},
            {"Fixed Amount Numerator", FIXEDFLD_UINT, (void *)&OPTION_DELIVERY_COMPONENT_FIXED_AMOUNT_NUMERATOR_LEN, (void *)&smt_table->reference.option_delivery.fixed_amount_numerator},
            {"Currecny Code", FIXEDFLD_STRING, (void *)&OPTION_DELIVERY_COMPONENT_CURRENCY_CODE_LEN, smt_table->reference.option_delivery.currency_code},
            {"Strike Percent", FIXEDFLD_DECIMAL, &OPTION_DELIVERY_COMPONENT_STRIKE_PERCENT_LEN, (void *)&smt_table->reference.option_delivery.strike_percent},
            {"Component Symbol Locate", FIXEDFLD_UINT, &OPTION_DELIVERY_COMPONENT_COMPONENT_SYMBOL_LOCATE_LEN, (void *)&smt_table->reference.option_delivery.component_symbol_locate},
            {NULL, 0, NULL, NULL}};
        smt_all_decode(smt_table, fixedfld, msgb, msgl, is_market);
        break;
    }
    default:
    { 
        sprintf(smt_table->logmsg, "Unknown Type(0x%02X)", smt_table->type);
        smt_table->loglevel = FL_PROGRESS;
        smt_table->logflag = 0;
        return (0);
    }
    }

    return (0);
}
