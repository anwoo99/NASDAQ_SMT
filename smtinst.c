#include "nassmt.h"

static int cmplocate(const void *a, const void *b);

/*
 * Function: initInst()
 * -----------------------------------------
 * Instrument Locate Code 용 공유메모리 초기화
 */
int initInst(FEP *fep, int key, int clr)
{
    int shmid, shmsz;
    char *shmad;
    SHM_INST *shm_inst;
    int is_new = 0;
    struct shmid_ds shminfo;

    shmsz = (INT_MAX / 8 + 16) + sizeof(int) + (sizeof(InstrumentLocate) * fep->xchg->room);

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

    shm_inst = (SHM_INST *)shmad;
    shm_inst->bit = (unsigned char *)shmad;
    shm_inst->inst_size = (int *)(shmad + (INT_MAX / 8 + 16));
    shm_inst->inst_list = (InstrumentLocate *)(shmad + (INT_MAX / 8 + 16) + sizeof(int));
    fep->bit = shm_inst;

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
 * Function: initInst()
 * -----------------------------------
 * smt_table 내 Instrument Locate 정보
 * 공유메모리 등록
 */
InstrumentLocate *createInst(SMARTOPTION_TABLE *smt_table)
{
    FEP *fep = smt_table->fep;
    SHM_INST *shm_inst = (SHM_INST *)fep->bit;
    InstrumentLocate *smt_inst = &smt_table->instrument_locate;
    InstrumentLocate *retv;

    // Instrument Locate Code가 이미 존재하면 해당 정보 업데이트 후 리턴
    if ((retv = readInst(smt_table, smt_inst->locate_code)) != NULL)
        return (retv);

    // Set bit
    setbit(shm_inst->bit, smt_inst->locate_code);

    // Instrument Locate 정보 등록
    memcpy(&shm_inst->inst_list[*shm_inst->inst_size], smt_inst, sizeof(InstrumentLocate));
    *shm_inst->inst_size += 1;

    // bsearch를 위한 qsort
    qsort(shm_inst->inst_list, *shm_inst->inst_size, sizeof(InstrumentLocate), cmplocate);

    fep_log(fep, FL_PROGRESS, "'%s(%lu)' instrument added. Current Size:%d", smt_inst->symbol, smt_inst->locate_code, *shm_inst->inst_size);

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
    SHM_INST *shm_inst = (SHM_INST *)fep->bit;
    InstrumentLocate key, *retv;

    // Check bit.
    if (chkbit(shm_inst->bit, locate_code) == 0)
        return (NULL);

    // 인자로 받은 locate code에 해당하는 데이터 찾기
    key.locate_code = locate_code;
    retv = bsearch(&key, shm_inst->inst_list, *shm_inst->inst_size, sizeof(InstrumentLocate), cmplocate);

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
    SHM_INST *shm_inst = (SHM_INST *)fep->bit;
    InstrumentLocate *smt_inst = &smt_table->instrument_locate;
    InstrumentLocate *retv;

    // Instrument Locate Code가 존재하지 않으면 리턴
    if ((retv = readInst(smt_table, smt_inst->locate_code)) == NULL)
        return (NULL);

    // Set bit
    setbit(shm_inst->bit, smt_inst->locate_code);

    // instrument Locate 업데이트
    memcpy(retv, smt_inst, sizeof(InstrumentLocate));

    // bsearch를 위한 qsort
    qsort(shm_inst->inst_list, *shm_inst->inst_size, sizeof(InstrumentLocate), cmplocate);

    return (retv);
}

/*
 * Function: deleteInst()
 * -----------------------------------------
 * Instrument Locate 정보 삭제
 */
int deleteInst(SMARTOPTION_TABLE *smt_table, uint64_t locate_code)
{
    FEP *fep = smt_table->fep;
    SHM_INST *shm_inst = (SHM_INST *)fep->bit;
    InstrumentLocate *retv;
    char *from, *to;
    int index, size;

    // Instrument Locate Code가 존재하지 않으면 리턴
    if ((retv = readInst(smt_table, locate_code)) == NULL)
        return (0);

    // Clear bit
    clrbit(shm_inst->bit, locate_code);

    // Delete
    index = (int)(((unsigned long)retv - (unsigned long)shm_inst->inst_list) / sizeof(InstrumentLocate));
    size = (*shm_inst->inst_size - index - 1) * sizeof(InstrumentLocate);

    from = (char *)&retv[1];
    to = (char *)&retv[0];

    memmove(to, from, size);

    *shm_inst->inst_size -= 1;

    // bsearch를 위한 qsort
    qsort(shm_inst->inst_list, *shm_inst->inst_size, sizeof(InstrumentLocate), cmplocate);

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