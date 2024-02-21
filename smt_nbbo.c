#include "nassmt.h"

#define NBBO_LEVEL 0
#define _ASK_FLAG 0x01
#define _BID_FLAG 0x02
#define _ASK_N_BID_FLAG 0x03

int smt_nbbo(SMARTOPTION_TABLE *smt_table)
{
    FEP *fep = smt_table->fep;
    MDFOLD *folder = NULL;
    MDDEPT *depth = NULL;
    MarketDataHeader *header = &smt_table->market_data.header;
    NBBO *nbbo = &smt_table->market_data.nbbo;
    ChannelSeconds *cs;
    uint32_t xymd, xhms, kymd, khms;
    char check[8];
    int nbbo_flag = 0;

    memset(check, 0x00, sizeof(check));

    // Get Symbol from Locate Code
    if ((folder = smtfold(fep, nbbo->instrument_locate)) == NULL)
    {
        return (-1);
    }

    // Get Quote location
    depth = &folder->dept;

    // Get Channel Seconds Information
    if ((cs = readCs(smt_table, header->protocol_id, header->channel_index)) == NULL)
    {
        return (-1);
    }

    switch (nbbo->nside)
    {
    case 1:
        if (nbbo->bid.side == 'B')
            nbbo_flag |= _BID_FLAG;
        if (nbbo->ask.side == 'S')
            nbbo_flag |= _ASK_FLAG;
        break;
    case 2:
        nbbo_flag |= _ASK_N_BID_FLAG;
        break;
    default:
        return (0);
    }

    // ASK
    if (nbbo_flag & _ASK_FLAG)
    {
        BASK *bask = depth->ask;
        bask[NBBO_LEVEL].vask = nbbo->ask.size;
        bask[NBBO_LEVEL].pask = nbbo->ask.price.value;
    }

    // BID
    if (nbbo_flag & _BID_FLAG)
    {
        BBID *bbid = depth->bid;
        bbid[NBBO_LEVEL].vbid = nbbo->bid.size;
        bbid[NBBO_LEVEL].pbid = nbbo->bid.price.value;
    }

    // Set the current exchange time;
    time_t curr = gettime_from_mid_sec(cs->seconds);
    fep_time(fep, curr, &xymd, &xhms, &kymd, &khms);

    depth->tymd = xymd;
    depth->xymd = xymd;
    depth->xhms = xhms;
    depth->kymd = kymd;
    depth->khms = khms;

    check[DEPT] |= CHK_UPDATE;
    check[DEPT] |= CHK_FEED;

    smt_push(fep, folder, check);

    return (0);
}