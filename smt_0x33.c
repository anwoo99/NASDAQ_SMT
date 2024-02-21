#include "nassmt.h"

static int _smt_equity(FEP *fep, TOKEN *token, SMARTOPTION_TABLE *smt_table);
static int _smt_root(FEP *fep, TOKEN *token, SMARTOPTION_TABLE *smt_table);
static int _smt_option(FEP *fep, TOKEN *token, SMARTOPTION_TABLE *smt_table);

static FOLDER *_init_symbol(FEP *fep, InstrumentLocate *inst);

static char day_orth[32][3] = {"st", "nd", "rd", "th", "th", "th", "th", "th", "th", "th", "th", "th", "th", "th", "th", "th", "th", "th", "th", "th", "st", "nd", "rd", "th", "th", "th", "th", "th", "th", "th", "st", ""};
static char month_str[13][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", ""};

int smt_0x33(SMARTOPTION_TABLE *smt_table)
{
    InstrumentLocate *inst = &smt_table->instrument_locate;

    // EQUITY -> OPTION ROOT -> OPTION PRODUCT 순서
    switch (inst->product_type)
    {
    case EQUITY_PRODUCT: // 1단계
        _smt_equity(smt_table->fep, smt_table->token, smt_table);
        break;
    case OPTION_ROOT_PRODUCT: // 2단계
        _smt_root(smt_table->fep, smt_table->token, smt_table);
        break;
    case OPTION_PRODUCT: // 3단계
        _smt_option(smt_table->fep, smt_table->token, smt_table);
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
        deleteInst(smt_table, inst->locate_code);
        fep_log(fep, FL_DEBUG, "There is no '%s' symbol in %s-symb.csv", inst->symbol, fep->exnm);
        return (-1);
    }

    // Instrument Locate 등록
    updateInst(smt_table);

    return (0);
}

static int _smt_root(FEP *fep, TOKEN *token, SMARTOPTION_TABLE *smt_table)
{
    InstrumentLocate *inst = &smt_table->instrument_locate;

    // Parent Locate Check
    if (readInst(smt_table, inst->parent_locate_code) == NULL)
    {
        deleteInst(smt_table, inst->locate_code);
        fep_log(fep, FL_DEBUG, "Cannot find the matching parent locate code for '%s' root", inst->symbol);
        return (-1);
    }

    // Instrument Locate 등록
    updateInst(smt_table);

    return (0);
}

static int _smt_option(FEP *fep, TOKEN *token, SMARTOPTION_TABLE *smt_table)
{
    InstrumentLocate *inst = &smt_table->instrument_locate;
    InstrumentLocate *parent;
    FOLDER *folder;
    MDMSTR *mstr, t_mstr;
    SYMBOL *symbol;
    int year, month, mday;
    char check[8];

    if (fep->xchg->type != OPTION)
        return (-1);

    // Parent Locate Check
    if (readInst(smt_table, inst->parent_locate_code) == NULL)
    {
        fep_log(fep, FL_DEBUG, "Cannot find the matching parent locate code for '%s' option", inst->symbol);
        return (-1);
    }

    // Parent Locate 정보 가져오기
    parent = findParent(smt_table, EQUITY_PRODUCT);

    // Parnet Locate의 Symbol을 토대로 XXXX-symb.csv 파일 정보 가져오기
    symbol = fep_symbget(fep, parent->symbol);

    // 만기일 확인 후 월물 제한
    /*
     *
     */

    // Symbol 등록(Nasdaq 측 소수점자리수가 너무 큼 - 수동 설정)
    inst->strike.denominator = symbol->zdiv;
    folder = _init_symbol(fep, inst);

    if (folder == NULL)
        return (-1);

    mstr = &folder->mstr;

    // Master 정보가 바뀐 것이 있는지 확인하고자 temp용 변수 생성
    memcpy(&t_mstr, mstr, sizeof(MDMSTR));

    // Locate Code 등록
    sprintf(mstr->scid, "%lu", inst->locate_code);

    // Root 등록
    strcpy(mstr->root, inst->root);

    // Clearing Symbol
    strcpy(mstr->clrs, symbol->csym);

    // Currency Code 등록
    sprintf(mstr->curr[0], "%s", parent->currency_code);

    // Product Type
    mstr->styp = (int)inst->msgtype;

    // Put or Call
    mstr->corp = inst->put_or_call[0];

    // Expiration Date
    mstr->zymd = (uint32_t)inst->expiration_date;
    year = YEAR(mstr->zymd);
    month = MONTH(mstr->zymd);
    mday = MDAY(mstr->zymd);

    // Strike Price
    mstr->strk = inst->strike.value;

    // Decimal Denominator
    mstr->zdiv = symbol->zdiv;

    // Price Increment
    mstr->pinc = pow(0.1, mstr->zdiv);

    // Short Exchange Name(MIC)
    strcpy(mstr->exnm, parent->MIC);

    // Exchange ID
    mstr->exid = fep->exid;

    // Underlying Symbol and Locate Code(finding Equity)
    sprintf(mstr->unid, "%lu", parent->locate_code);
    sprintf(mstr->unps, "%.31s", parent->symbol);

    // Set ECYM
    sprintf(mstr->ecym, "%d%s %s'%d", mday, day_orth[mday - 1], month_str[month - 1], year % 100);

    // Set ENAM
    sprintf(mstr->enam, "%s %s %c%.*f", symbol->enam, mstr->ecym, inst->put_or_call[0], mstr->zdiv, inst->strike.value);

    // Set session time
    mstr->session.frhm = symbol->session.frhm; // trading hour
    mstr->session.tohm = symbol->session.tohm; // trading hour
    mstr->session.fwdy = symbol->session.fwdy; // trading start day of week
    mstr->session.twdy = symbol->session.twdy; // trading end day of week
    mstr->session.hfhm = symbol->session.hfhm; // trading halt time
    mstr->session.hthm = symbol->session.hthm; // trading halt time

    if (mstr->session.fwdy == 0 || mstr->session.twdy == 0)
    {
        mstr->session.fwdy = 1; // Monday
        mstr->session.twdy = 5; // Friday
    }

    // Feed Number
    if (atoi(symbol->feed) > 0)
        mstr->feed = atoi(symbol->feed);

    if (memcmp(&t_mstr, mstr, sizeof(MDMSTR)) != 0)
    {
        mstr->uymd = token->xymd;
        mstr->uhms = token->xhms;
        fep_scidupd(fep, folder);
        memset(check, 0, sizeof(check));
        check[MSTR] |= CHK_UPDATE;
        check[MSTR] |= CHK_FEED;
        // naspush(fep, folder, NULL, check);
    }

    return (0);
}

static FOLDER *_init_symbol(FEP *fep, InstrumentLocate *inst)
{
    FOLDER *folder = NULL;
    MDMSTR *mstr;
    char corise_symbol[SYMB_LEN], local_symbol[SYMB_LEN];

    strcpy(local_symbol, inst->symbol);

    if (_convert_symbol(fep, corise_symbol, inst) == -1)
        return (NULL);

    /* 이미 해당 symbol에 관한 Folder가 존재하는지 확인 */
    folder = getfolder(fep, corise_symbol);

    if (folder != NULL)
    {
        /* 만일 기존 폴더에 저장된 Symbol과 다를 경우 품목 중복임을 알림 */
        if (strcmp(folder->mstr.code, local_symbol) != 0)
        {
            fep_log(fep, FL_PROGRESS, "Symbol Mismatch(%s)! folder[%s] <-> current[%s]", corise_symbol, folder->mstr.code, local_symbol);
            return (NULL);
        }

        return (folder);
    }

    /* Folder가 없다면 새로운 폴더 생성 */
    folder = newfolder(fep, corise_symbol);

    if (folder == NULL)
        return (NULL);

    /* 마스터 구조체 초기화 */
    mstr = &folder->mstr;
    strcpy(mstr->code, local_symbol);
    mstr->pmul = 1;
    mstr->xdiv = 1;
    mstr->ydiv = 1;
    mstr->zdiv = 0;
    mstr->csiz = 1;
    mstr->feed = 1;

    fep_log(fep, FL_PROGRESS, "The new symbol '%s(%s)' was added.", corise_symbol, local_symbol);
    return (folder);
}
