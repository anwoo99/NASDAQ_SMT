#include "nassmt.h"

/*
 * Function: initInst()
 * -----------------------------------------
 * Instrument Locate Code 용 공유메모리 초기화
 */
SHM_INST *initInst(FEP *fep, int key, int clr)
{
    int shmid, shmsz;
    char *shmad;
    SHM_INST *shm_inst;

    shmsz = (INT_MAX / 8 + 16) + (sizeof(INST_MAP) * fep->xchg->room);

    if ((shmid = shmget(key, 0, 0666)) < 0)
    {
        if ((shmid = shmget(key, shmsz, 0666 | IPC_CREAT)) < 0)
            return (NULL);
    }
    shmad = shmat(shmid, (char *)0, 0);

    if (shmad == (char *)-1)
        return (NULL);
    if (clr)
        memset(shmad, 0, shmsz);

    shm_inst = (SHM_INST *)shmad;
    return (shm_inst);
}
