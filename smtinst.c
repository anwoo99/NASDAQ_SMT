#include "nassmt.h"

static int cmplocate(InstrumentLocate *s1, InstrumentLocate *s2);

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

    shmsz = (sizeof(InstrumentLocate) * fep->xchg->room) + sizeof(int);

    if ((shmid = shmget(key, 0, 0666)) < 0)
    {
        if ((shmid = shmget(key, shmsz, 0666 | IPC_CREAT)) < 0)
            return (NULL);
        is_new = 1;
    }
    shmad = shmat(shmid, (char *)0, 0);

    if (shmad == (char *)-1)
        return (NULL);
    if (clr)
        memset(shmad, 0, shmsz);

    shm_inst = (SHM_INST *)shmad;

    if (is_new)
        memset(shm_inst, 0x00, shmsz);

    fep->bit = shm_inst;
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
    SHM_INST *shm_inst = fep->bit;
    InstrumentLocate *smt_inst = &smt_table->instrument_locate;
    InstrumentLocate *retv;

    // Instrument Locate Code가 이미 존재하면 해당 정보 업데이트 후 리턴
    if ((retv = readInst(smt_table, smt_inst->locate_code)) != NULL)
        return (updateInst(smt_table));

    // Instrument Locate 정보 등록
    memcpy(&shm_inst->inst_list[shm_inst->inst_size++], smt_inst, sizeof(InstrumentLocate));

    // bsearch를 위한 qsort
    qsort(shm_inst->inst_list, shm_inst->inst_size, sizeof(InstrumentLocate), cmplocate);

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
    SHM_INST *shm_inst = fep->bit;
    InstrumentLocate key;

    // 인자로 받은 locate code에 해당하는 데이터 찾기
    key.locate_code = locate_code;

    return (bsearch(&key, shm_inst->inst_list, shm_inst->inst_size, sizeof(InstrumentLocate), cmplocate));
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
    SHM_INST *shm_inst = fep->bit;
    InstrumentLocate *smt_inst = &smt_table->instrument_locate;
    InstrumentLocate *retv;

    // Instrument Locate Code가 존재하지 않으면 리턴
    if ((retv = readInst(smt_table, smt_inst->locate_code)) == NULL)
        return (NULL);

    // instrument Locate 업데이트
    memcpy(retv, smt_inst, sizeof(InstrumentLocate));

    // bsearch를 위한 qsort
    qsort(shm_inst->inst_list, shm_inst->inst_size, sizeof(InstrumentLocate), cmplocate);

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
    SHM_INST *shm_inst = fep->bit;
    InstrumentLocate *retv;
    char *from, *to;
    int index, size;

    // Instrument Locate Code가 존재하지 않으면 리턴
    if ((retv = readInst(smt_table, locate_code)) == NULL)
        return (0);

    // Delete
    index = (int)(((unsigned long)retv - (unsigned long)shm_inst->inst_list) / sizeof(InstrumentLocate));
    size = (shm_inst->inst_size - index - 1) * sizeof(InstrumentLocate);

    from = (char *)&retv[1];
    to = (char *)&retv[0];

    memmove(to, from, size);

    shm_inst->inst_size--;

    // bsearch를 위한 qsort
    qsort(shm_inst->inst_list, shm_inst->inst_size, sizeof(InstrumentLocate), cmplocate);

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

    while((retv = readInst(smt_table, inst->parent_locate_code)) != NULL)
    {
        if(retv.product_type == product_type)
            return(retv);
        
        memcpy(&inst, retv, sizeof(InstrumentLocate));
    }

    return(NULL);
}

static int cmplocate(InstrumentLocate *s1, InstrumentLocate *s2)
{
    if (s1->locate_code > s2->locate_code)
        return 1;
    else if (s1->locate_code < s2->locate_code)
        return -1;
    else
        return 0;
}