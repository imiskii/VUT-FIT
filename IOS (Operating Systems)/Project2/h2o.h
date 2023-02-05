
/**
 * 
 * @file h2o.h
 * @author Michal Ľaš
 * @brief Headr of H2O program
 * @date 2022-04-25
 * 
 * DU2 IOS 2022
 */

#include <stdio.h>
#include <semaphore.h>

#ifndef H2O_H
#define H2O_H

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

/**
 * @brief Structure
 * @brief Program arguments
 * 
 */
typedef struct {
    int NO; // Number of given oxygens | only unsigned number are aloved
    int NH; // Number of given hydrogens | only unsigned number are aloved
    int TI; // Number of miliseconds that oxygen or hydrogen waits after his creation | interval: <0,1000>
    int TB; // Number of miliseconds that requires molecule creation | interval: <0,1000>
} Args_t;


/**
 * @brief Structure
 * @brief All allocated semafors
 * 
 */
typedef struct {
    sem_t *mutex;
    sem_t *barier;
    sem_t *O_queue;
    sem_t *H_queue;
    sem_t *writing;
    sem_t *O_msg;
    sem_t *H_barier;
} Sem_t;


/**
 * @brief Structure
 * @brief Array of processes pid-s and size of array
 * 
 */
typedef struct {
    pid_t *pids; // Shared array of childs processes pid-s
    size_t size;
} P_pid_t;


/**
 * @brief Structure
 * @brief All allocated shared memmory 
 * 
 */
typedef struct {
    int *hydrogen; // Number of hydrogens
    int *oxygen;   // Number of oxygens 
    int *proc_id;  // Number of current proces
    int *noM;      // Number of molecules
    int *count_H;  // Count number of H creating molecule
} Shm_t;


/**
 * @brief Load args to structure 'Args_t' and check if they are valid
 * 
 * @param argc Number of arguments
 * @param argv pointer to arguments given to program '*argv[]'
 * @param args pointer to structure 'Args_t' that will be filled
 */
void load_args(int argc, char *argv[] ,Args_t *args);


/**
 * @brief Open a file, pointer to opened file will be in 'f'
 * 
 * @param f Pointer to pointer of variable FILE
 */
void open_file(FILE **f);


/**
 * @brief Allocate semaphores for structure 'Sem_t'
 * 
 * @param semaphore Pointer to structure with semaphores
 * @return 0 if succes 1 if failed
 */
int sem_alloc(Sem_t *semaphore);


/**
 * @brief Allocate share memmory for structure 'Shm_t'
 * 
 * @param shm Pointer to structure with semaphores
 * @param pids Pointer to structure with pid-s of running processes
 * @return 0 if succes 1 if failed
 */
int share_mem_alloc(Shm_t *shm, P_pid_t *pids);


/**
 * @brief Free share memmory
 * 
 * @param shm Pointer to structure with shared memmory
 * @param pids Pointer to structure with pid-s of running processes
 */
void share_mem_free(Shm_t *shm, P_pid_t *pids);


/**
 * @brief Free semaphores
 * 
 * @param semaphore Pointer to structure with shared memmory
 */
void sem_free(Sem_t *semaphore);


/**
 * @brief Free all allocadet resources, close opened files, and kill main procces
 * 
 * @param f Pointer to FILE
 * @param semaphore Pointer to structure with semaphores
 * @param shm Pointer to structure with shared memmory
 * @param pids Pointer to structure with pid-s of running processes
 */
void myexit(FILE **f, Sem_t *semaphore, Shm_t *shm, P_pid_t *pids);


/**
 * @brief 
 * 
 * @param f Pointer to file
 * @param Hid ID of proccess
 * @param maxMolecules maximum number of molecules that can be created
 * @param args Pointer to structure with args
 * @param semaphore Pointer to structure with semaphores
 * @param shm Pointer to structure with shared memmory
 */
void hydrogen_fnc(FILE *f, int Hid, int maxMolecules, Args_t *args, Sem_t *semaphore, Shm_t *shm);


/**
 * @brief 
 * 
 * @param f Pointer to file
 * @param Oid ID of proccess
 * @param maxMolecules maximum number of molecules that can be created
 * @param args Pointer to structure with args
 * @param semaphore Pointer to with semaphores
 * @param shm Pointer to structure with shared memmory
 */
void oxygen_fnc(FILE *f, int Oid, int maxMolecules, Args_t *args, Sem_t *semaphore, Shm_t *shm);


#endif // H2O_H

/******************************************************* END OF FILE *******************************************************/