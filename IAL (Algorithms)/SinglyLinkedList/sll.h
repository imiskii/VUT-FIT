/**
 * @file sll.h
 * @author Michal Ľaš
 * @brief header file for sll.c (Singly Linked List)
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#ifndef SLL_H
#define SLL_H


/** Global variable - indicates if function called error */
extern bool error_flag;

/** Element of Singly Linked List */
typedef struct ListElement {
    int data;                           // Data
    struct ListElement *nextElement;    // Next element
} *ListElementPtr;

/** Singly Linked List */
typedef struct {
	ListElementPtr firstElement;        // Pointer to first element
    ListElementPtr activeElement;       // Pointer to current active element
} List;



/**
 * @brief Function set error flag and print error message to stderr
 * 
 */
void List_Error();


/**
 * @brief Function initialize new SLL
 * 
 * @param list pointer to list that will be initialized
 */
void List_Init(List *list);


/**
 * @brief Deletes all elements in list and set SLL to state after initialization
 * 
 * @param list pointer to list that will be disposed
 */
void List_Dispose(List *list);


/**
 * @brief Add new element on beginig of the list
 * 
 * @param list pointer to list where element will be added
 * @param data data in new created element
 */
void List_InsertFirst(List *list, int data);


/**
 * @brief Function set as active element the first element in list
 * 
 * @param list list where active alement will be set
 */
void List_First(List *list);


/**
 * @brief Function in parameter dataPtr return data of first element in list
 * 
 * @param list pointer to initialized list
 * @param dataPtr pointer to data that function will set
 */
void List_GetFirst( List *list, int *dataPtr);


/**
 * @brief Function delete first element of SLL
 * 
 * @param list pointer to initialized list
 */
void List_DeleteFirst(List *list);


/**
 * @brief Function delete element located after active element
 * 
 * @param list pointer to list where element will be deleted
 */
void List_DeleteAfter(List *list);


/**
 * @brief Function add element after active element
 * 
 * @param list pointer to initialized list
 * @param data data in new created element
 */
void List_InsertAfter(List *list, int data);


/**
 * @brief Function moves activity of list to next element
 * 
 * @param list pointer to initialized list
 */
void List_Next(List *list);


/**
 * @brief Function in parameter dataPtr return data of active element in list 
 * 
 * @param list pointer to initialized list
 * @param dataPtr pointer to data that function will set
 */
void List_GetValue( List *list, int *dataPtr);


/**
 * @brief Function rewrite data in active element of list
 * 
 * @param list pointer to initialized list
 * @param data new data that will be set to active element
 */
void List_SetValue(List *list, int data);


/**
 * @brief Function return non zero value if list is active else function return 0
 * 
 * @param List pointer to initialized list
 * @return int value
 */
int List_IsActive(List *List);


#endif  // SLL_H

/* END OF FILE */