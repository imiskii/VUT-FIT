/**
 * @file dll.h
 * @author your name (you@domain.com)
 * @brief header file for dll.c (Doubly Linked List)
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef DLL_H
#define DLL_H


extern bool error_flag;  // Global variable - error flag

/* Element of Doubly Linked List */
typedef struct DLLElement {
	int data;                           // data
	struct DLLElement *previousElement; // pointer previous element
	struct DLLElement *nextElement;     // pointer next element
} *DLLElementPtr;

/* Doubly Linked List */
typedef struct {
	DLLElementPtr firstElement;         // pointer to first element of list
	DLLElementPtr activeElement;        // pointer to active element of list
	DLLElementPtr lastElement;          // pointer to last element of list
} DLList;



/**
 * @brief Function set error flag and print error message to stderr
 * 
 */
void DLL_Error();


/**
 * @brief Function initialize new DLL
 * 
 * @param list pointer to initialized DLL structure that will be initialized
 */
void DLL_Init(DLList *list);


/**
 * @brief Function delete all elements of list
 * 
 * @param list pointer to initialized DLL which element will be deleted
 */
void DLL_Dispose(DLList *list);


/**
 * @brief Function add new element in the begining of DLL
 * 
 * @param list pointer to initialized DLL where new element will be added
 * @param data data that will be in new element
 */
void DLL_InsertFirst(DLList *list, int data);


/**
 * @brief Function add new element to end of DLL
 * 
 * @param list pointer to DLL where new element will be added
 * @param data data that will be in new element
 */
void DLL_InsertLast(DLList *list, int data);


/**
 * @brief Function set first element as active element
 * 
 * @param list pointer to initialized DLL
 */
void DLL_First(DLList *list);


/**
 * @brief Function set last element as active element
 * 
 * @param list pointer to initialized DLL
 */
void DLL_Last(DLList *list);


/**
 * @brief Function return in parameter dataPtr value of first element in DLL
 * 
 * @param list pointer to initialized DLL
 * @param dataPtr pointer to data that function will set
 */
void DLL_GetFirst(DLList *list, int *dataPtr);


/**
 * @brief Function return in parameter dataPtr value of last element in DLL
 * 
 * @param list pointer to initialized DLL
 * @param dataPtr pointer to data that function will set
 */
void DLL_GetLast(DLList *list, int *dataPtr);


/**
 * @brief Function delete first element of DLL
 * If first element was active element then activity of list is lost
 * If list is empty nothing happend
 * 
 * @param list pointer to initialized DLL
 */
void DLL_DeleteFirst(DLList *list);


/**
 * @brief Function delete last element of DLL
 * If first element was active element then activity of list is lost
 * If list is empty nothing happend
 * 
 * @param list pointer to initialized DLL
 */
void DLL_DeleteLast(DLList *list);


/**
 * @brief Function delete element located after active element
 * If DLL have no active element or active element is last element
 * then do nothing
 * 
 * @param list pointer to initialized DLL
 */
void DLL_DeleteAfter(DLList *list);


/**
 * @brief Function delete element located before active element
 * If DLL have no active element or active element is last element
 * then do nothing
 * 
 * @param list pointer to initialized DLL
 */
void DLL_DeleteBefore(DLList *list);


/**
 * @brief Function add new element after active element
 * 
 * @param list pointer to initialized DLL
 * @param data data that will be in new element
 */
void DLL_InsertAfter(DLList *list, int data);


/**
 * @brief Function add new element before active element
 * 
 * @param list pointer to initialized DLL
 * @param data data that will be in new element
 */
void DLL_InsertBefore(DLList *list, int data);


/**
 * @brief Function return in parameter dataPtr value of active element in DLL
 * If list has no active element then call DLL_Error()
 * 
 * @param list pointer to initialized DLL
 * @param dataPtr pointer to data that function will set
 */
void DLL_GetValue(DLList *list, int *dataPtr);


/**
 * @brief Function rewrite data of active element
 * If list has no active alement then do nothing
 * 
 * @param list pointer to initialized DLL
 * @param data new data that will be set in element
 */
void DLL_SetValue(DLList *list, int data);


/**
 * @brief Function move activity of DLL to next element
 * If list has no active element do nothing
 * 
 * @param list pointer to initialized DLL
 */
void DLL_Next(DLList *list);


/**
 * @brief Function move activity of DLL to previous element
 * If list has no active element do nothing
 * 
 * @param list pointer to initialized DLL
 */
void DLL_Previous(DLList * list);


/**
 * @brief Function return non zero value if DLL has active element
 * else return 0
 * 
 * @param list pointer to initialized DLL
 * @return int non zero if DLL is active, else 0
 */
int DLL_IsActive(DLList *list);



#endif // DLL_H

/* END OF FILE */
