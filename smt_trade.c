#include "nassmt.h"

static int _smt_trade_trade(FEP *fep, TOKEN *token, SMARTOPTION_TABLE *smt_table);
static int _smt_cancel_trade(FEP *fep, TOKEN *token, SMARTOPTION_TABLE *smt_table);

int smt_trade(SMARTOPTION_TABLE *smt_table)
{
    TRADE *trade = &smt_table->market_data.trade;

    switch (trade->trade_type)
    {
    case TRADE_TRADE_TYPE:
        _smt_trade_trade(smt_table->fep, smt_table->token, smt_table);
        break;
    case CANCEL_TRADE_TYPE:
        _smt_cancel_trade(smt_table->fep, smt_table->token, smt_table);
        break;
    default:
        break;
    }

    return (0);
}

static int _smt_trade_trade(FEP *fep, TOKEN *token, SMARTOPTION_TABLE *smt_table)
{
    MDFOLD *folder = NULL;
    MDQUOT *quote = NULL;
    MarketDataHeader *header = &smt_table->market_data.header;
    TRADE *trade = &smt_table->market_data.trade;
    ChannelSeconds *cs;
    uint32_t xymd, xhms, kymd, khms;
    int32_t evol;
    char check[8];

    // Get Symbol from Locate code
    if ((folder = smtfold(fep, trade->instrument_locate)) == NULL)
    {
        return (-1);
    }

    // Get Quote location
    quote = &folder->quot;

    // Get Channel Seconds Information
    if ((cs = readCs(smt_table, header->protocol_id, header->channel_index)) == NULL)
    {
        return (-1);
    }

    if (trade->change_flags & CF_MARKET_CENTER_VOLUME)
    {
        if (trade->size <= quote->tvol)
            return (0);

        // Trade Current Volume
        evol = quote->tvol - trade->size;
        
        if(evol >= 0)
            quote->evol = evol;
        else
            quote->evol = trade->size;

        // Trade Total Volume
        quote->tvol = trade->size;
    }

    if (trade->change_flags & CF_MARKET_CENTER_LAST_SALE_PRICE)
    {
        // Trade Last Price
        quote->last = trade->price.value;
    }

    // Set the current exchange time;
    time_t curr = gettime_from_mid_sec(cs->seconds);
    fep_time(fep, curr, &xymd, &xhms, &kymd, &khms);

    if (quote->kymd == kymd && quote->khms == khms)
        quote->seqn++;
    else
        quote->seqn = 0;

    if (quote->tymd < xymd)
        quote->tymd = xymd;

    quote->xymd = xymd;
    quote->xhms = xhms;
    quote->kymd = kymd;
    quote->khms = khms;

    // Open Price
    if (trade->eligibility_flags & EL_OPEN_PRICE_FLAG)
    {
        quote->open = trade->price.value;
        quote->otim = quote->xhms;
    }

    // High Price
    if ((quote->high < trade->price.value) && (trade->eligibility_flags & EL_HIGH_LOW_PRICE_FLAG))
    {
        quote->high = trade->price.value;
        quote->htim = quote->xhms;
    }

    // Low Price
    if ((quote->low > trade->price.value) && (trade->eligibility_flags & EL_HIGH_LOW_PRICE_FLAG))
    {
        quote->low = trade->price.value;
        quote->ltim = quote->xhms;
    }

    // Close Price
    if (trade->eligibility_flags & EL_CLOSE_PRICE_FLAG)
    {
        quote->setp = trade->price.value;
        quote->symd = quote->xymd;
        check[SETT] = 1;
        fep->cast[SETT] = 1;
        return (0);
    }

    // Total Volume
    if (trade->eligibility_flags & EL_TOTAL_VOLUME_FLAG)
    {
        quote->tvol = trade->size;
    }

    check[QUOT] = 1;
    fep->cast[QUOT] = 1;

    return (0);
}

static int _smt_cancel_trade(FEP *fep, TOKEN *token, SMARTOPTION_TABLE *smt_table)
{
    return (0);
}