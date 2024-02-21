#include "nassmt.h"

#define MAX_STACK_FRAMES 128

typedef struct
{
    char errname[512];
    char reason[1024];
    char where[1024];
} ERROR_INFO;

FEP *fep;

static int smtrcv(FEP *fep, void *argv);
void signal_handler(int signo);
void print_trace(ERROR_INFO *error_info);
void stopit(int signo);
void usage(const char *program_name)
{
    printf("Usage:\n");
    printf("  %s <exchange_name>\n", program_name);
    exit(EXIT_FAILURE);
}

MDPROC procedure =
    {NULL, smtrcv, NULL, NULL, NULL, NULL}; // function's table

int main(int argc, char **argv)
{
    MDARCH *arch;
    char exchange_name[512];

    if (argc < 2)
    {
        usage(basename(argv[0]));
    }

    strcpy(exchange_name, argv[1]);

    fep = fep_open(exchange_name, MD_RDWR | MD_CREAT);

    if (fep == NULL)
    {
        if (isatty(1))
            fprintf(stderr, "Cannot open FEP interface for '%s'\n", exchange_name);
        return (-1);
    }

    arch = fep->arch;
    arch->intv = fep->xchg->intv;

    signal(SIGSEGV, signal_handler); // Segmentaion Fault 발생 시, 오류 발생 지점 logging
    signal(SIGINT, stopit);
    signal(SIGQUIT, stopit);
    signal(SIGTERM, stopit);

    initsmt(fep, IPCK(fep->xchg->ipck, COOKER, 1), 0); // Shared memory for SMART Option
    fep_scidinit(fep);                                 // Instrument Locate Shared Memory initialize

    fep_init(fep, &procedure, 1);
    fep_close(fep);
    return (0);
}

void smtsett(FEP *fep, MDFOLD *folder, uint32_t symd, double price, char *check)
{
    MDMSTR *m = &folder->mstr;
    MDQUOT *q = &folder->quot;
    MDQUOT *a = &folder->quat;
    float base = 0., prev = 0.;
    uint32_t pymd;

    if (symd == 0)
        symd = folder->mstr.prev.symd;

    if (folder->mstr.prev.symd > symd)
        return;

    m->base = price;
    m->clos = price;

    if (folder->mstr.prev.symd != symd)
    {
        prev = m->prev.setp;
        pymd = m->prev.symd;
        memset(&m->prev, 0, sizeof(m->prev));
        fep_time(fep, 0, &folder->mstr.prev.uymd, &folder->mstr.prev.uhms, NULL, NULL);
        if (q->base != 0.)
            prev = q->base;
        m->prev.base = prev; // prior settlement price
        m->prev.pymd = pymd; // prior trading day
    }

    m->prev.symd = symd;
    m->prev.setp = price;
    m->cymd = symd;

    if (q->tymd == symd)
    {
        base = (q->base != 0.) ? q->base : prev;
        q->xymd = (q->xymd == 0) ? symd : q->xymd;
        q->open = (q->open == 0.) ? price : q->open;
        q->high = (q->high == 0.) ? price : q->high;
        q->low = (q->low == 0.) ? price : q->low;
        q->last = (q->last == 0.) ? price : q->last;
        q->diff = q->last - base;
        q->rate = (q->diff * 100) / base;
        prev = base;

        m->prev.symd = symd;
        m->prev.base = q->base;
        m->prev.open = (a->open > 0) ? a->open : q->open;
        m->prev.high = (q->high < a->high) ? a->high : q->high;
        m->prev.low = (a->low > 0 && a->low < q->low) ? a->low : q->low;
        m->prev.last = q->last;
        m->prev.tvol = q->tvol + a->tvol;
        m->prev.opin = q->opin;
    }

    if (m->prev.open == 0.)
        m->prev.open = price;
    if (m->prev.high == 0.)
        m->prev.high = price;
    if (m->prev.low == 0.)
        m->prev.low = price;
    if (m->prev.last == 0.)
        m->prev.last = price;
    if ((prev = m->prev.base) != 0.)
    {
        m->prev.diff = m->prev.setp - prev;
        m->prev.rate = (m->prev.diff * 100) / prev;
        if (m->uplp != 0. && m->prev.setp == m->uplp)
            m->prev.sign = _UL_;
        else if (m->dnlp != 0. && m->prev.setp == m->dnlp)
            m->prev.sign = _DL_;
        else if (m->prev.diff > 0)
            m->prev.sign = _UP_;
        else if (m->prev.diff < 0)
            m->prev.sign = _DN_;
        else
            m->prev.sign = _NC_;
    }
    if (check != NULL)
        check[MSTR] = 1;
    else
        putfolder(fep, folder, MSTR);

    fep_log(fep, FL_PROGRESS, "STEP-3 %s smtsett symd:%d  P:%g ", folder->symb, m->prev.symd, m->prev.setp);
}

int smt_push(FEP *fep, MDFOLD *folder, char *check)
{
    /* Memory Update */
    if (check[SETT] & CHK_UPDATE)
    {
        smtsett(fep, folder, folder->quot.symd, folder->quot.setp, check);
        check[SETT] &= ~CHK_UPDATE;
    }
    if ((check[MSTR] & CHK_UPDATE) || folder->mstr.xage > 0)
    {
        folder->mstr.xage = 0;
        putfolder(fep, folder, MSTR);
        check[MSTR] &= ~CHK_UPDATE;
    }
    if (check[QUOT] & CHK_UPDATE)
    {
        folder->quot.pask = folder->dept.ask[0].pask;
        folder->quot.pbid = folder->dept.bid[0].pbid;
        folder->quot.vask = folder->dept.ask[0].vask;
        folder->quot.vbid = folder->dept.bid[0].vbid;
        fep_push(fep, folder, QUOT);
        check[QUOT] &= ~CHK_UPDATE;
    }
    if (check[DEPT] & CHK_UPDATE)
    {
        //upddept(fep, folder, d);
        check[DEPT] &= ~CHK_UPDATE;
    }
    if (check[CANC] & CHK_UPDATE)
    {
        check[CANC] &= ~CHK_UPDATE;
    }

    /* Feed Start */
    if (check[MSTR] & CHK_FEED)
    {
        fep_feed(fep, folder, MSTR, NULL);
        check[MSTR] &= ~CHK_FEED;
    }
    if (check[QUOT] & CHK_FEED)
    {
        fep_log(fep, FL_PROGRESS, "1) fep_feed now: SYMB:%s V:%d T:%d P:%f", folder->quot.symb, folder->quot.evol, folder->quot.tvol, folder->quot.last);
        fep_feed(fep, folder, QUOT, NULL);
        check[QUOT] &= ~CHK_FEED;
    }
    if (check[DEPT] & CHK_FEED)
    {
        fep_feed(fep, folder, DEPT, NULL);
        check[DEPT] &= ~CHK_FEED;
    }
    if (check[SETT] & CHK_FEED)
    {
        fep_feed(fep, folder, SETT, NULL);
        check[SETT] &= ~CHK_FEED;
    }
    if (check[CANC] & CHK_FEED)
    {
        fep_feed(fep, folder, CANC, NULL);
        check[CANC] &= ~CHK_FEED;
    }
    return (0);
}

/*
 * Function: smtfold()
 * --------------------------------------
 * Locate Code 기반 Corise Symbol 폴더 탐색
 */
MDFOLD *smtfold(FEP *fep, uint64_t locate_code)
{
    MDFOLD *folder;
    char symbol[MAX_SYMB_LEN];
    char search[128];

    sprintf(search, "%lu", locate_code);

    if (fep_scid2symb(fep, search, symbol) != 0)
        return (NULL);

    folder = getfolder(fep, symbol);
    return (folder);
}

/*
 * Function: smtrcv()
 * -------------------------------------
 * smtrcv 송신 패킷 IPC 수신
 */
static int smtrcv(FEP *fep, void *argv)
{
    MDARCH *arch = fep->arch;
    MDCTX *ctx = fep->ctx;
    TOKEN *token = argv;
    SMARTOPTION_TABLE smt_table;
    int rc = 0;

    /* Initialize smart option table */
    initialize_smart(&smt_table, fep, token);

    /* Smart Option Message Decoding */
    smt_decode(&smt_table);

    /* Logging data */
    // nas_smt_log(fep, &smt_table, "[%d-Type=%s(0x%02X) SEQ:%u LEN:%d]", token->port, smt_table.name, smt_table.type, tr_packet->header.seqn, tr_packet->pkt_l);

    /* Logging Raw Data in CSV Format */
    nas_smt_csv(fep, &smt_table);

    // Call the function
    if (smt_table.proc != NULL)
        rc = (*smt_table.proc)(&smt_table);

    return (rc);
}

void stopit(int signo)
{
    fep_close(fep);
    exit(0);
}

void print_trace(ERROR_INFO *error_info)
{
    void *array[MAX_STACK_FRAMES];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace(array, MAX_STACK_FRAMES);
    strings = backtrace_symbols(array, size);

    int offset = 0;
    for (i = 0; i < size; i++)
    {
        int printed = snprintf(&error_info->where[offset], sizeof(error_info->where) - offset, "%s\n", strings[i]);
        if (printed > 0)
        {
            offset += printed;
        }
    }

    free(strings);
}

void signal_handler(int signo)
{
    ERROR_INFO error_info;

    memset(&error_info, 0x00, sizeof(ERROR_INFO));

    switch (signo)
    {
    case SIGSEGV:
        strncpy(error_info.errname, "Segmentation Fault", sizeof(error_info.errname) - 1);
        print_trace(&error_info);
        break;
    default:
        break;
    }

    fep_log(fep, FL_MUST,
            "\n############################################################\n"
            "1) Error: %s\n"
            "2) Where:\n%s"
            "############################################################",
            error_info.errname, error_info.where);

    stopit(signo);
}
