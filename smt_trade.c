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
    TRADE *trade = &smt_table->market_data.trade;
    ChannelSeconds *cs;
    uint32_t xymd, xhms, kymd, khms;

    // Get Symbol from Locate code
    if ((folder = smtfold(fep, trade->locate_code)) == NULL)
    {
        return (-1);
    }

    // Get Quote location
    quote = &folder->quot;

    // Get Channel Seconds Information
    if ((cs = readCs(smt_table, trade->protocol_id, trade->channel_index)) == NULL)
    {
        return (-1);
    }

    if (trade->change_flags & CF_MARKET_CENTER_VOLUME)
    {
        if (trade->size <= quote->tvol)
            return (0);

        // Trade Current Volume
        quote->evol = quote->tvol - trade->size;

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
        quote->tymd = xhms;

    quote->xymd = xymd;
    quote->xhms = xhms;
    quote->kymd = kymd;
    quote->khms = khms;

    // Open Price
    if (trade->eligibility_flags & EL_OPEN_PRICE_FLAG)
    {
        quote->open = trade->price;
        quote->otim = quote->xhms;
    }

    // High Price
    if ((quote->high != trade->price) && (trade->eligibility_flags & EL_HIGH_LOW_PRICE_FLAG))
    {
        quote->high = trade->price;
        quote->htim = quote->xhms;
    }

    // Low Price
    if ((quote->lowp != trade->price) && (trade->eligibility_flags & EL_HIGH_LOW_PRICE_FLAG))
    {
        quote->lowp = trade->price;
        quote->ltim = quote->xhms;
    }

    // Close Price
    if (trade->eligibility_flags & EL_CLOSE_PRICE_FLAG)
    {
        quote->setp = trade->price;
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