#include "nassmt.h"

/*
 * Function: _conv_wcode()
 * ------------------------------
 * Week + Days = OPRA Weekly Code
 */
int _conv_wcode(int week, int days)
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

/*
 * Function: _conv_mcode()
 * ----------------------------
 * Month = OPRA Monthly Code
 */
int _conv_mcode(int month)
{
    char fut_month[] = {
        'F', 'G', 'H', 'J', 'K', 'M', 'N', 'Q', 'U', 'V', 'X', 'Z', 0, 0};

    if (month < 1 || month > 12)
        return (-1);

    return (fut_month[month - 1]);
}

/*
 * Function: _conv_opra_wcode()
 * ------------------------------
 * NASDAQ Ticker -> Corise Ticker
 */
int _convert_symbol(FEP *fep, char *corise_symbol, InstrumentLocate *inst)
{
    int year, month, mday, week;
    char expd[256];
    char wcod, mcod;

    memset(corise_symbol, 0x00, SYMB_LEN);

    // Parse the expiration date to y/m/d(ex.20240214)
    year = YEAR(inst->expiration_date) % 100; // 24
    month = MONTH(inst->expiration_date);     // 02
    mday = MDAY(inst->expiration_date);       // 14

    // Get week info
    sprintf(expd, "%lu", inst->expiration_date);
    week = getweek(expd);

    // Get Weekly Code
    wcod = _conv_wcode(week, mday);

    if (wcod <= 0)
        return (-1);

    // Get Monthly Code
    mcod = _conv_mcode(month);

    if (mcod <= 0)
        return (-1);

    // Make corise symbol
    sprintf(corise_symbol, "%c%s%c%d %c%.*f", wcod, inst->root, inst->mcode, year % 10, inst->put_or_call[0], inst->strike.denominator, inst->strike.value);

    return (0);
}