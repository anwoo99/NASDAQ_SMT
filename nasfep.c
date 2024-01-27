#include "nastag.h"

static int nasrcv(FEP *fep, void *argv);

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
    FEP *fep;
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

    fep_init(fep, &procedure, 1);
    fep_close(fep);
    return (0);
}

static int nasrcv(FEP *fep, void *argv)
{
    MDARCH *arch = fep->arch;
    MDCTX *ctx = fep->ctx;
    TOKEN *token = argv;
    TR_PACKET *tr_packet;
    SMARTOPTION_TABLE smt_table;
    int rc = 0;

    tr_packet = (TR_PACKET *)token->rcvb;

    /* Smart Option Message Decoding */
    smt_decode(&smt_table, tr_packet->header.type, tr_packet->pkt_buff, tr_packet->pkt_l);

    /* Logging data */
    // nas_raw_log(fep, smt_table.loglevel, 0, tr_packet->pkt_buff, tr_packet->pkt_l, "[%d-Type=%s(0x%02X) SEQ:%u LEN:%d]", token->port, smt_table.name, smt_table.type, tr_packet->header.seqn, tr_packet->pkt_l);

    nas_raw_csv(fep, smt_table.loglevel, smt_table.type, smt_table.loghead, smt_table.logmsg);

    return (rc);
}
