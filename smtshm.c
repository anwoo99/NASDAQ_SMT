#include "nassmt.h"

static int cmplocate(const void *a, const void *b);
static int cmpcs(const void *a, const void *b);

/*
 * Function: initsmt()
 * -----------------------
 * Smartoption용 공유메모리
 */
int initsmt(FEP *fep, int key, int clr)
{
    int shmid, shmsz;
    char *shmad;
    SHM_SMART *shm_smart;
    int is_new = 0;
    int offset = 0;
    struct shmid_ds shminfo;

    shmsz = (INT_MAX / 8 + 16) + sizeof(int) + (sizeof(InstrumentLocate) * fep->xchg->room) + sizeof(int) + (sizeof(ChannelSeconds) * MAX_CHANNEL);

    if ((shmid = shmget(key, 0, 0666)) < 0)
    {
        if ((shmid = shmget(key, shmsz, 0666 | IPC_CREAT)) < 0)
            return (-1);
        is_new = 1;
    }
    shmad = shmat(shmid, (char *)0, 0);

    if (shmad == (char *)-1)
        return (-1);

    if (clr)
        memset(shmad, 0, shmsz);

    if (is_new)
    {
        memset(shmad, 0x00, shmsz);
    }

    // 1) bit
    shm_smart = (SHM_SMART *)shmad;
    shm_smart->bit = (unsigned char *)shmad;

    // 2) inst_size
    offset += (INT_MAX / 8 + 16);
    shm_smart->inst_size = (int *)(shmad + offset);

    // 3) inst_list
    offset += sizeof(int);
    shm_smart->inst_list = (InstrumentLocate *)(shmad + offset);

    // 4) cs_size
    offset += (sizeof(InstrumentLocate) * fep->xchg->room);
    shm_smart->cs_size = (int *)(shmad + offset);

    // 4) cs_list
    offset += sizeof(int);
    shm_smart->cs_list = (ChannelSeconds *)(shmad + offset);

    fep->bit = shm_smart;

    // Get information about the shared memory segment
    if (shmctl(shmid, IPC_STAT, &shminfo) == -1)
        return (0);

    fep_log(fep, FL_MUST,
            "\n\n[Instrument Locate Shared Memory Information]\n"
            "  - Shared Memory ID: %d\n"
            "  - Size of Shared Memory: %d bytes\n"
            "  - Creation Time: %s"
            "  - Address: %p\n",
            shmid, shmsz, ctime(&shminfo.shm_ctime), (void *)fep->bit);

    return (0);
}

/*
 * Function: createInst()
 * -----------------------------------
 * smt_table 내 Instrument Locate 정보
 * 공유메모리 등록
 */
InstrumentLocate *createInst(SMARTOPTION_TABLE *smt_table)
{
    FEP *fep = smt_table->fep;
    SHM_SMART *shm_smart = (SHM_SMART *)fep->bit;
    InstrumentLocate *smt_inst = &smt_table->instrument_locate;
    InstrumentLocate *retv;

    // Instrument Locate Code가 이미 존재하면 해당 정보 리턴
    if ((retv = readInst(smt_table, smt_inst->locate_code)) != NULL)
        return (retv);

    // Set bit
    setbit(shm_smart->bit, smt_inst->locate_code);

    // Instrument Locate 정보 등록
    memcpy(&shm_smart->inst_list[*shm_smart->inst_size], smt_inst, sizeof(InstrumentLocate));
    *shm_smart->inst_size += 1;

    // bsearch를 위한 qsort
    qsort(shm_smart->inst_list, *shm_smart->inst_size, sizeof(InstrumentLocate), cmplocate);

    fep_log(fep, FL_PROGRESS, "'%s(%lu)' instrument added. Current Size:%d", smt_inst->symbol, smt_inst->locate_code, *shm_smart->inst_size);

    return (smt_inst);
}

/*
 * Function: readInst()
 * ---------------------------------------
 * 공유메모리 내 Instrument Locate 정보 읽기
 */
InstrumentLocate *readInst(SMARTOPTION_TABLE *smt_table, uint64_t locate_code)
{
    FEP *fep = smt_table->fep;
    SHM_SMART *shm_smart = (SHM_SMART *)fep->bit;
    InstrumentLocate key, *retv;

    // Check bit.
    if (chkbit(shm_smart->bit, locate_code) == 0)
        return (NULL);

    // 인자로 받은 locate code에 해당하는 데이터 찾기
    key.locate_code = locate_code;
    retv = bsearch(&key, shm_smart->inst_list, *shm_smart->inst_size, sizeof(InstrumentLocate), cmplocate);

    return (retv);
}

/*
 * Function: updateInst()
 * ----------------------------------
 * smt_table내 Instrument Locate 정보
 * 공유메모리 업데이트
 */
InstrumentLocate *updateInst(SMARTOPTION_TABLE *smt_table)
{
    FEP *fep = smt_table->fep;
    SHM_SMART *shm_smart = (SHM_SMART *)fep->bit;
    InstrumentLocate *smt_inst = &smt_table->instrument_locate;
    InstrumentLocate *retv, inst;

    // Instrument Locate Code가 존재하지 않으면 생성
    if ((retv = readInst(smt_table, smt_inst->locate_code)) == NULL)
        return (createInst(smt_table));

    // Set bit
    setbit(shm_smart->bit, smt_inst->locate_code);

    // instrument Locate 업데이트
    memcpy(retv, smt_inst, sizeof(InstrumentLocate));

    // 결과값 복사
    memset(inst, 0x00, sizeof(InstrumentLocate));
    memcpy(inst, retv, sizeof(InstrumentLocate));

    // bsearch를 위한 qsort
    qsort(shm_smart->inst_list, *shm_smart->inst_size, sizeof(InstrumentLocate), cmplocate);

    return (inst);
}

/*
 * Function: deleteInst()
 * -----------------------------------------
 * Instrument Locate 정보 삭제
 */
int deleteInst(SMARTOPTION_TABLE *smt_table, uint64_t locate_code)
{
    FEP *fep = smt_table->fep;
    SHM_SMART *shm_smart = (SHM_SMART *)fep->bit;
    InstrumentLocate *retv;
    char *from, *to;
    int index, size;

    // Instrument Locate Code가 존재하지 않으면 리턴
    if ((retv = readInst(smt_table, locate_code)) == NULL)
        return (0);

    // Clear bit
    clrbit(shm_smart->bit, locate_code);

    // Delete
    index = (int)(((unsigned long)retv - (unsigned long)shm_smart->inst_list) / sizeof(InstrumentLocate));
    size = (*shm_smart->inst_size - index - 1) * sizeof(InstrumentLocate);

    from = (char *)&retv[1];
    to = (char *)&retv[0];

    memmove(to, from, size);

    *shm_smart->inst_size -= 1;

    // bsearch를 위한 qsort
    qsort(shm_smart->inst_list, *shm_smart->inst_size, sizeof(InstrumentLocate), cmplocate);

    return (0);
}

/*
 * Function: findParent()
 * ----------------------------------------------
 * Product Type 인자 값과 일치한 Parent Locate 찾기
 */
InstrumentLocate *findParent(SMARTOPTION_TABLE *smt_table, uint64_t product_type)
{
    InstrumentLocate *smt_inst = &smt_table->instrument_locate;
    InstrumentLocate *retv, inst;

    memcpy(&inst, smt_inst, sizeof(InstrumentLocate));

    while ((retv = readInst(smt_table, inst.parent_locate_code)) != NULL)
    {
        if (retv->product_type == product_type)
            return (retv);

        memcpy(&inst, retv, sizeof(InstrumentLocate));
    }

    return (NULL);
}

/*
 * Function: createCs()
 * ---------------------------------------
 * smt_table 내 Channel Seconds 정보
 * 공유메모리 등록
 */
ChannelSeconds *createCs(SMARTOPTION_TABLE *smt_table)
{
    FEP *fep = smt_table->fep;
    SHM_SMART *shm_smart = (SHM_SMART *)fep->bit;
    ChannelSeconds *smt_cs = &smt_table->channel_seconds;
    ChannelSeconds *retv;

    // Channel Seconds가 이미 존재하면 해당 정보 리턴
    if ((retv = readCs(smt_table, smt_cs->protocol_id, smt_cs->channel_index)) != NULL)
        return (retv);

    // Instrument Locate 정보 등록
    memcpy(&shm_smart->cs_list[*shm_smart->cs_size], smt_cs, sizeof(ChannelSeconds));
    *shm_smart->cs_size += 1;

    // bsearch를 위한 qsort(Protocol ID -> Channel Index)
    qsort(shm_smart->cs_list, *shm_smart->cs_size, sizeof(ChannelSeconds), cmpcs);

    fep_log(fep, FL_PROGRESS, "Channel Seconds Added. Protocl ID(%lu) Channel Index(%lu) Current Size:%d", smt_cs->protocol_id, smt_cs->channel_index, *shm_smart->cs_size);
}

/*
 * Function: createCs()
 * ---------------------------------------
 * 공유메모리 내 Channel Seconds 정보 읽기
 */
ChannelSeconds *readCs(SMARTOPTION_TABLE *smt_table, uint64_t protocol_id, uint64_t channel_index)
{
    FEP *fep = smt_table->fep;
    SHM_SMART *shm_smart = (SHM_SMART *)fep->bit;
    ChannelSeconds key, *retv;

    // 인자값에 해당하는 데이터 찾기
    key.protocol_id = protocol_id;
    key.channel_index = channel_index;
    retv = bsearch(&key, shm_smart->cs_list, *shm_smart->cs_size, sizeof(ChannelSeconds), cmpcs);

    return (retv);
}

/*
 * Function: updateCs()
 * ----------------------------------
 * smt_table내 Channel Seconds 정보
 * 공유메모리 업데이트
 */
ChannelSeconds *updateCs(SMARTOPTION_TABLE *smt_table)
{
    FEP *fep = smt_table->fep;
    SHM_SMART *shm_smart = (SHM_SMART *)fep->bit;
    ChannelSeconds *smt_cs = &smt_table->channel_seconds;
    ChannelSeconds *retv, cs;

    // Channel Seconds가 존재하지 않으면 리턴
    if ((retv = readCs(smt_table, smt_cs->protocol_id, smt_cs->channel_index)) == NULL)
        return (createCs(smt_table));

    // Channel Seconds 업데이트
    memcpy(retv, smt_cs, sizeof(ChannelSeconds));

    // 결과값 복사
    memset(cs, 0x00, sizeof(ChannelSeconds));
    memcpy(cs, retv, sizeof(ChannelSeconds));

    // bsearch를 위한 qsort
    qsort(shm_smart->cs_list, *shm_smart->cs_size, sizeof(ChannelSeconds), cmpcs);

    fep_log(fep, FL_DEBUG, "Channel Seconds Update! Protocol ID:%lu Channel Index: %lu Seconds:%lu", cs.protocol_id, cs.channel_index, cs.seconds);

    return (cs);
}

/*
 * Function: deleteCs()
 * -----------------------------------------
 * Channel Seconds 정보 삭제
 */
int deleteCs(SMARTOPTION_TABLE *smt_table, uint64_t protocol_id, uint64_t channel_index)
{
    FEP *fep = smt_table->fep;
    SHM_SMART *shm_smart = (SHM_SMART *)fep->bit;
    ChannelSeconds *retv;
    char *from, *to;
    int index, size;

    // Instrument Locate Code가 존재하지 않으면 리턴
    if ((retv = readCs(smt_table, protocol_id, channel_index)) == NULL)
        return (0);

    // Delete
    index = (int)(((unsigned long)retv - (unsigned long)shm_smart->cs_list) / sizeof(ChannelSeconds));
    size = (*shm_smart->cs_size - index - 1) * sizeof(ChannelSeconds);

    from = (char *)&retv[1];
    to = (char *)&retv[0];

    memmove(to, from, size);

    *shm_smart->cs_size -= 1;

    // bsearch를 위한 qsort
    qsort(shm_smart->cs_list, *shm_smart->cs_size, sizeof(ChannelSeconds), cmpcs);

    return (0);
}

static int cmplocate(const void *a, const void *b)
{
    InstrumentLocate *s1 = (InstrumentLocate *)a;
    InstrumentLocate *s2 = (InstrumentLocate *)b;

    if (s1->locate_code > s2->locate_code)
        return 1;
    else if (s1->locate_code < s2->locate_code)
        return -1;
    else
        return 0;
}

static int cmpcs(const void *a, const void *b)
{
    ChannelSeconds *s1 = (ChannelSeconds *)a;
    ChannelSeconds *s2 = (ChannelSeconds *)b;

    if (s1->protocol_id > s2->protocol_id)
        return 1;
    else if (s1->protocol_id < s2->protocol_id)
        return -1;
    else
    {
        if (s1->channel_index > s2->channel_index)
            return 1;
        else if (s1->channel_index < s2->channel_index)
            return -1;
        else
            return 0;
    }
}