#include "nassmt.h"

static int _smt_equity(FEP *fep, TOKEN *token, SMARTOPTION_TABLE *smt_table);
static int _smt_root(FEP *fep, TOKEN *token, SMARTOPTION_TABLE *smt_table);
static int _smt_option(FEP *fep, TOKEN *token, SMARTOPTION_TABLE *smt_table);

static FOLDER *_init_symbol(FEP *fep, char *symbol);

int smt_0x33(SMARTOPTION_TABLE *smt_table)
{
    InstrumentLocate *inst = &smt_table->instrument_locate;

    // EQUITY -> OPTION ROOT -> OPTION PRODUCT 순서
    switch (inst->product_type)
    {
    case EQUITY_PRODUCT: // 1단계
        _smt_equity(smt_table->fep, smt_table->token, (SMARTOPTION_TABLE *)smt_table);
        break;
    case OPTION_ROOT_PRODUCT: // 2단계
        _smt_root(smt_table->fep, smt_table->token, (SMARTOPTION_TABLE *)smt_table);
        break;
    case OPTION_PRODUCT: // 3단계
        _smt_option(smt_table->fep, smt_table->token, (SMARTOPTION_TABLE *)smt_table);
        break;
    case INDEX_PRODUCT:
    case WORLD_CURRENCY_PRODUCT:
    case FIXED_INCOME_PRODUCT:
    case FUTURES_CONTRACT_PRODUCT:
    case UNKNOWN_PRODUCT:
    default:
        break;
    }

    return (0);
}

static int _smt_equity(FEP *fep, TOKEN *token, SMARTOPTION_TABLE *smt_table)
{
    SYMBOL *symbol;
    InstrumentLocate *inst = &smt_table->instrument_locate;

    // Symbol Check
    symbol = fep_symbget(fep, inst->symbol);

    if (symbol == NULL)
    {
        clrbit(fep->bit, (unsigned int)inst->locate_code);
        fep_log(fep, FL_DEBUG, "There is no '%s' symbol in %s-symb.csv", inst->symbol, fep->exnm);
        return (-1);
    }

    // Instrument Locate bit 등록
    setbit(fep->bit, (unsigned int)inst->locate_code);

    return (0);
}

static int _smt_root(FEP *fep, TOKEN *token, SMARTOPTION_TABLE *smt_table)
{
    InstrumentLocate *inst = &smt_table->instrument_locate;

    // Parent Locate Check
    if (chkbit(fep->bit, inst->parent_locate_code) == 0)
    {
        fep_log(fep, FL_DEBUG, "Cannot find the matching parent locate code for '%s' root", inst->symbol);
        return (-1);
    }

    // Instrument Locate bit 등록
    setbit(fep->bit, inst->locate_code);

    return (0);
}

static int _smt_option(FEP *fep, TOKEN *token, SMARTOPTION_TABLE *smt_table)
{
    InstrumentLocate *inst = &smt_table->instrument_locate;
    FOLDER *folder;
    MDMSTR *mstr, t_mstr;
    char check[8];

    if (fep->xchg->type != OPTION)
        return (-1);

    // Parent Locate Check
    if (chkbit(fep->bit, inst->parent_locate_code) == 0)
    {
        fep_log(fep, FL_DEBUG, "Cannot find the matching parent locate code for '%s' option", inst->symbol);
        return (-1);
    }

    // 만기일 확인 후 월물 제한

    // Symbol 등록
    folder = _init_symbol(fep, inst->symbol);
    mstr = &folder->mstr;

    // Master 정보가 바뀐 것이 있는지 확인하고자 temp용 변수 생성
    memcpy(&t_mstr, mstr, sizeof(MDMSTR));

    // Locate Code 등록
    sprintf(mstr->scid, "%lu", inst->locate_code);

    // Currency Code 등록
    sprintf(mstr->curr[0], "%s", inst->currency_code);

    // Product Type
    mstr->styp = (int)inst->msgtype;

    // Put or Call
    mstr->corp = inst->put_or_call[0];

    // Expiration Date
    mstr->zymd = (uint32_t)inst->expiration_date;

    // Strike Price
    mstr->strk = inst->strike.value;

    // Decimal Denominator
    mstr->zdiv = (int)inst->strike.denominator;

    if (memcmp(&t_mstr, mstr, sizeof(MDMSTR)) != 0)
    {
        mstr->uymd = token->xymd;
        mstr->uhms = token->xhms;
        memset(check, 0, sizeof(check));
        check[MSTR] = 1;
        fep->cast[MSTR] = 1;
        // naspush(fep, folder, NULL, check);
    }

    return (0);
}

static FOLDER *_init_symbol(FEP *fep, char *symbol)
{
    FOLDER *folder = NULL;
    MDMSTR *mstr;

    /* 이미 해당 symbol에 관한 Folder가 존재하는지 확인 */
    folder = getfolder(fep, symbol);

    if (folder != NULL)
        return (folder);

    /* Folder가 없다면 새로운 폴더 생성 */
    folder = newfolder(fep, symbol);

    if (folder == NULL)
        return (NULL);

    /* 마스터 구조체 초기화 */
    mstr = &folder->mstr;
    mstr->pmul = 1;
    mstr->xdiv = 1;
    mstr->ydiv = 1;
    mstr->zdiv = 0;
    mstr->csiz = 1;
    mstr->feed = 1;

    fep_log(fep, FL_PROGRESS, "The new symbol '%s' was added.", symbol);
    return (folder);
}
