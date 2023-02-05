
/**
 * 
 * @file h2o.c
 * @author Michal Ľaš
 * @brief Dfinition of functions for program H2O
 * @date 2022-04-25
 * 
 * DU2 IOS 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <signal.h>
#include "h2o.h"


void load_args(int argc, char *argv[] ,Args_t *args){

    /** Check number of arguments **/

    if (argc != 5){
        fprintf(stderr, "Invalid number of program arguments !\n");
        exit(EXIT_FAILURE);
    }
    else {
        unsigned short i = 1; // index to argv
        char *sptr = NULL;    //string part of argv[i]
        
        /** Check if there is only integers values **/

        for (;i < argc; i++){
            strtol(argv[i], &sptr, 10);
            if (*sptr != 0 || (strcmp(argv[i], "") == 0)) {
                fprintf(stderr, "Invalid program argument !\n");
                exit(EXIT_FAILURE);
            }
        }

        i = 1;

        /** Fill structure **/

        args->NO = atoi(argv[i++]); // i == 1
        args->NH = atoi(argv[i++]); // i == 2
        args->TI = atoi(argv[i++]); // i == 3
        args->TB = atoi(argv[i]);   // i == 4

        /** Check if there are valid values **/

        if ((args->NO <= 0) ||
            (args->NH <= 0) ||
            (args->TI < 0)  || (args->TI > 1000) ||
            (args->TB < 0)  || (args->TB > 1000)){
                fprintf(stderr, "Invalid program argument !\n");
                exit(EXIT_FAILURE);
        }
    }
}


void open_file(FILE **f){
    *f = fopen("proj2.out", "w");
    if (*f == NULL){
        fprintf(stderr, "Output file opening failed !\n");
        exit(EXIT_FAILURE);
    }
    setbuf(*f, NULL);
}


int sem_alloc(Sem_t *semaphore){

    if ((semaphore->mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED ||
        (semaphore->barier = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED ||
        (semaphore->O_queue = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED ||
        (semaphore->H_queue = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED ||
        (semaphore->writing = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED ||
        (semaphore->O_msg = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED ||
        (semaphore->H_barier = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED ||
        sem_init(semaphore->mutex, 1, 1) == -1 ||
        sem_init(semaphore->barier, 1, 0) == -1 ||
        sem_init(semaphore->O_queue, 1, 0) == -1 ||
        sem_init(semaphore->H_queue, 1, 0) == -1 ||
        sem_init(semaphore->writing, 1, 1) == -1 ||
        sem_init(semaphore->O_msg, 1, 0) == -1 ||
        sem_init(semaphore->H_barier, 1, 0) == -1 ) {
            fprintf(stderr, "Error during creatig semaphors !\n");
            return 1;
        }
    return 0;
}


int share_mem_alloc(Shm_t *shm, P_pid_t *pids){

    if ((shm->oxygen = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED ||
        (shm->hydrogen = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED ||
        (shm->proc_id = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED ||
        (shm->noM = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED ||
        (shm->count_H = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED ||
        (pids->pids = mmap(NULL, (sizeof(pid_t) * pids->size), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED ) {
            fprintf(stderr, "Error during creating shared memory !\n");
            return 1;
        }
    return 0;
}


void sem_free(Sem_t *semaphore){
    sem_destroy(semaphore->mutex);
    sem_destroy(semaphore->barier);
    sem_destroy(semaphore->O_queue);
    sem_destroy(semaphore->H_queue);
    sem_destroy(semaphore->writing);
    sem_destroy(semaphore->O_msg);
    sem_destroy(semaphore->H_barier);
}


void share_mem_free(Shm_t *shm, P_pid_t *pids){
    munmap(shm->oxygen, sizeof(int));
    munmap(shm->hydrogen, sizeof(int));
    munmap(shm->proc_id, sizeof(int));
    munmap(shm->noM, sizeof(int));
    munmap(shm->count_H, sizeof(int));
    munmap(pids->pids, (sizeof(pid_t) * pids->size));
}


void myexit(FILE **f, Sem_t *semaphore, Shm_t *shm, P_pid_t *pids){
    size_t i = 0; // index to list of processes
    while (pids->pids[i] != '\0'){
        kill(pids->pids[i], SIGINT);
        i++;
    }
    sem_free(semaphore);
    share_mem_free(shm, pids);
    fclose(*f);
    exit(EXIT_FAILURE);
}


void hydrogen_fnc(FILE *f, int Hid, int maxMolecules, Args_t *args, Sem_t *semaphore, Shm_t *shm){

    /** Hydrogen started **/

    sem_wait(semaphore->writing);
    fprintf(f, "%d: H %d: started\n", *shm->proc_id, Hid);
    fflush(f);
    (*shm->proc_id)++;
    sem_post(semaphore->writing);

    usleep((random() % (args->TI + 1)) * 1000);

    /** Hydrogen added to queue **/

    sem_wait(semaphore->writing);
    fprintf(f, "%d: H %d: going to queue\n", *shm->proc_id, Hid);
    fflush(f);
    (*shm->proc_id)++;
    sem_post(semaphore->writing);

    /** Wait until there is 1 oxygen and 2 hydrogens **/

    sem_wait(semaphore->mutex);
    (*shm->hydrogen)++;

    if ((*shm->hydrogen >= 2) && (*shm->oxygen >= 1)){
        *shm->hydrogen -= 2;
        sem_post(semaphore->H_queue);
        sem_post(semaphore->H_queue);
        *shm->oxygen -= 1;
        sem_post(semaphore->O_queue);
    }
    else{
        sem_post(semaphore->mutex);
    }

    /** If no molecule will be created **/
    if (maxMolecules == 0){
        sem_post(semaphore->H_queue);
    }
    
    sem_wait(semaphore->H_queue);

    /** Check if another mollecule can be created **/

    if ((maxMolecules < *shm->noM) || (maxMolecules == 0)){
        sem_wait(semaphore->writing);
        fprintf(f, "%d: H %d: not enough O or H\n", *shm->proc_id, Hid);
        fflush(f);
        (*shm->proc_id)++;
        sem_post(semaphore->writing);

        sem_post(semaphore->mutex);
        return;
    }

    /** Creating molecule **/

    sem_wait(semaphore->writing);
    fprintf(f, "%d: H %d: creating molecule %d\n", *shm->proc_id, Hid, *shm->noM);
    fflush(f);
    (*shm->proc_id)++;
    sem_post(semaphore->writing);

    /** Wait on both H **/

    (*shm->count_H)++;
    if (*shm->count_H == 2){
        (*shm->count_H) = 0;
        sem_post(semaphore->H_barier);
        sem_post(semaphore->H_barier);
    }

    sem_wait(semaphore->H_barier);

    /** Wait on message from oxygen **/

    sem_wait(semaphore->O_msg);

    /** Molecule had been created **/

    sem_wait(semaphore->writing);
    fprintf(f, "%d: H %d: molecule %d created\n", *shm->proc_id, Hid, *shm->noM);
    fflush(f);
    (*shm->proc_id)++;
    sem_post(semaphore->writing);

    sem_post(semaphore->barier);
}


void oxygen_fnc(FILE *f, int Oid, int maxMolecules, Args_t *args, Sem_t *semaphore, Shm_t *shm){

    /** Oxygen started **/

    sem_wait(semaphore->writing);
    fprintf(f, "%d: O %d: started\n", *shm->proc_id, Oid);
    fflush(f);
    (*shm->proc_id)++;
    sem_post(semaphore->writing);

    usleep((random() % (args->TI + 1)) * 1000);

    /** Oxygen added to queue **/

    sem_wait(semaphore->writing);
    fprintf(f, "%d: O %d: going to queue\n", *shm->proc_id, Oid);
    fflush(f);
    (*shm->proc_id)++;
    sem_post(semaphore->writing);

    /** Wait until there is 1 oxygen and 2 hydrogens **/

    sem_wait(semaphore->mutex);
    (*shm->oxygen)++;


    if (*shm->hydrogen >= 2){
        *shm->hydrogen -= 2;
        sem_post(semaphore->H_queue);
        sem_post(semaphore->H_queue);
        *shm->oxygen -= 1;
        sem_post(semaphore->O_queue);
    }
    else {
        sem_post(semaphore->mutex);
    }

    /** If no molecule will be created **/
    if (maxMolecules == 0){
        sem_post(semaphore->O_queue);
    }

    sem_wait(semaphore->O_queue);

    /** Check if another mollecule can be created **/

    if ((maxMolecules < *shm->noM) || (maxMolecules == 0)){
        sem_wait(semaphore->writing);
        fprintf(f, "%d: O %d: not enough H\n", *shm->proc_id, Oid);
        fflush(f);
        (*shm->proc_id)++;
        sem_post(semaphore->writing);

        sem_post(semaphore->mutex);
        return;
    }

    /** Creating molecule **/

    sem_wait(semaphore->writing);
    fprintf(f, "%d: O %d: creating molecule %d\n", *shm->proc_id, Oid, *shm->noM);
    fflush(f);
    (*shm->proc_id)++;
    sem_post(semaphore->writing);

    usleep((random() % (args->TB + 1))  * 1000);

    /** Send message to hydrogens **/

    sem_post(semaphore->O_msg);
    sem_post(semaphore->O_msg);

    
    /** Wait until hydrogens had finished **/

    sem_wait(semaphore->barier);
    sem_wait(semaphore->barier);

    /** Molecule had been created **/

    sem_wait(semaphore->writing);
    fprintf(f, "%d: O %d: molecule %d created\n", *shm->proc_id, Oid, *shm->noM);
    fflush(f);
    (*shm->proc_id)++;
    sem_post(semaphore->writing);

    /** Increment number of mulecule counter **/

    (*shm->noM)++;

    /** Check if it was last molecule that can be created                **/
    /** If yes, then open semaphores for remaining hydrogens and oxygens **/

    if (maxMolecules < *shm->noM){
        for (int i = 0; i < (args->NO - maxMolecules); i++){
            sem_post(semaphore->O_queue);
        }
        for (int i = 0; i < (args->NH - (maxMolecules*2)); i++){
            sem_post(semaphore->H_queue);
        }
    }

    sem_post(semaphore->mutex);
}


/**************************************** END OF FILE ****************************************/