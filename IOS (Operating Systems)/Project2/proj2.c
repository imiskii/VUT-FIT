
/**
 * 
 * @file main.c
 * @author Michal Ľaš
 * @brief Creating H2O molecules with synchronizated proccesses
 * 
 * Program argumenst:
 * Number of given oxygens | only unsigned number are aloved
 * Number of given hydrogens | only unsigned number are aloved
 * Number of miliseconds that oxygen or hydrogen waits after his creation | interval: <0,1000>
 * Number of miliseconds that requires molecule creation | interval: <0,1000>
 * 
 * Example: ./main.c 3 5 100 100
 * 
 * @brief Inspired by book: The Little Book of Semaphores
 * @date 2022-04-25
 * 
 * DU2 IOS 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "h2o.h"


int main(int argc, char *argv[]){

    /** Loading arguments of program to structure **/

    Args_t args;
    load_args(argc, argv, &args);

    /** Open file for writing output of program **/

    FILE *f;
    open_file(&f);

    /** Initialize structures **/

    P_pid_t p_pids;
    p_pids.size = args.NH + args.NO + 1; // number of childs processes that will be created + '\0'

    Sem_t semaphore;
    Shm_t shm;

    /** Creating semaphores **/

    int error = sem_alloc(&semaphore);
    if (error == 1) myexit(&f, &semaphore, &shm, &p_pids);

    /** Creating share memmory **/

    error = share_mem_alloc(&shm, &p_pids);
    if (error == 1) myexit(&f, &semaphore, &shm, &p_pids);

    /** Initialize shared memmory **/

    *shm.hydrogen = 0;
    *shm.oxygen = 0;
    *shm.proc_id = 1;
    *shm.noM = 1;

    /** Count maximum number of H2O molecules that can be created **/
    int maxMolecules = (args.NH / 2);
    if (args.NO < maxMolecules){
        maxMolecules = args.NO;
    }

    /** Creating threads **/

    size_t index_to_pids = 0;

    for (int i = 0; i < args.NH; i++){
        pid_t pid = fork();
        if (pid == 0){ // child process
            p_pids.pids[index_to_pids] = getpid(); // add pid to pid-s list
            hydrogen_fnc(f, i+1, maxMolecules, &args, &semaphore, &shm);
            exit(EXIT_SUCCESS);
        }
        else if (pid < 0){
            fprintf(stderr, "Fork failure !\n");
            myexit(&f, &semaphore, &shm, &p_pids);
            exit(EXIT_FAILURE);
        }
        index_to_pids++;
    }
    for (int i = 0; i < args.NO; i++){
        pid_t pid = fork();
        if (pid == 0){ // child process
            p_pids.pids[index_to_pids] = getpid(); // add pid to pid-s list
            oxygen_fnc(f, i+1, maxMolecules, &args, &semaphore, &shm);
            exit(EXIT_SUCCESS);
        }
        else if (pid < 0){
            fprintf(stderr, "Fork failure !\n");
            myexit(&f, &semaphore, &shm, &p_pids);
            exit(EXIT_FAILURE);
        }
        index_to_pids++;
    }

    /** Waiting until all childs processes are finished **/

    while (wait(NULL) > 0);

    /** Free all and close file **/

    sem_free(&semaphore);
    share_mem_free(&shm, &p_pids);
    fclose(f);

    return 0;
}

/******************************************** END OF FILE ********************************************/