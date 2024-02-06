#include "nassmt.h"

#define MAX_STACK_FRAMES 128

typedef struct
{
    char errname[512];
    char reason[1024];
    char where[1024];
} ERROR_INFO;

FEP *fep;

static int nasrcv(FEP *fep, void *argv);
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
    {NULL, nasrcv, NULL, NULL, NULL, NULL}; // function's table

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

    memset(fep->bit, 0x00, )

    arch = fep->arch;
    arch->intv = fep->xchg->intv;

    signal(SIGSEGV, signal_handler); // Segmentaion Fault 발생 시, 오류 발생 지점 logging
    signal(SIGINT, stopit);
    signal(SIGQUIT, stopit);
    signal(SIGTERM, stopit);

    fep_init(fep, &procedure, 1);
    fep_close(fep);
    return (0);
}

/*
 * Function: nasrcv()
 * -------------------------------------
 * nasrcv 송신 패킷 IPC 수신
 */
static int nasrcv(FEP *fep, void *argv)
{
    MDARCH *arch = fep->arch;
    MDCTX *ctx = fep->ctx;
    TOKEN *token = argv;
    TR_PACKET *tr_packet;
    SMARTOPTION_TABLE smt_table;
    int rc = 0;

    tr_packet = (TR_PACKET *)token->rcvb;

    tr2smart(&smt_table, tr_packet);

    /* Smart Option Message Decoding */
    smt_decode(&smt_table);

    /* Logging data */
    // nas_smt_log(fep, &smt_table, "[%d-Type=%s(0x%02X) SEQ:%u LEN:%d]", token->port, smt_table.name, smt_table.type, tr_packet->header.seqn, tr_packet->pkt_l);

    /* Logging Raw Data in CSV Format */
    nas_smt_csv(fep, &smt_table);
    
    rc = (*smt_table.proc)(fep, token, &smt_table);

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

    fep_log(fep, FL_ERROR,
            "\n############################################################\n"
            "1) Error: %s\n"
            "2) Where:\n%s"
            "############################################################",
            error_info.errname, error_info.where);

    stopit(signo);
}