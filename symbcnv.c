#include "nassmt.h"

int _conv_opra_mcode(int week, int days)
{
    char opra_mcode[7][7] = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 'A', 'B', 'C', 'D', 'E', 0}, // 1 weekly
        {0, 'F', 'G', 'H', 'I', 'J', 0}, // 2 weekly
        {0, 'K', 'L', 'M', 'N', 'O', 0}, // 3 weekly
        {0, 'P', 'Q', 'R', 'S', 'T', 0}, // 4 weekly
        {0, 'U', 'V', 'W', 'X', 'Y', 0}, // 5 weekly
        {0, 0, 0, 0, 0, 0, 0}};

    if (week > 6 || days > 6)
        return (-1);

    return (opra_mcode[week][days]);
}

int _convert_symbol(FEP *fep, char *corise_symbol, InstrumentLocate *inst)
{
    int year, month, mday, week;
    char expd[256];
    char mcod;

    memset(corise_symbol, 0x00, SYMB_LEN);

    // Parse the expiration date to y/m/d(ex.20240214)
    year = YEAR(inst->expiration_date) % 100; // 24
    month = MONTH(inst->expiration_date);     // 02
    mday = MDAY(inst->expiration_date);       // 14

    // Get week info
    sprintf(expd, "%lu", inst->expiration_date);
    week = getweek(expd);

    // Get OPRA Month Code
    mcod = _conv_opra_mcode(week, mday);

    // Remove the zeros of strike price not needed

    // Make corise symbol
    sprintf(corise_symbol, "%s%c")

    return (0);
}