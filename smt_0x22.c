#include "nassmt.h"

int smt_0x22(SMARTOPTION_TABLE *smt_table)
{
    FEP *fep = smt_table->fep;
    ChannelSeconds *cs = &smt_table->channel_seconds;
    ChannelSeconds *retv;

    // Channel Seconds Update
    updateCs(smt_table);

    return (0);
}