/**
 * @file dll.c
 * @author Michal Ľaš
 * @brief Doubly Linked List
 * 
 */

#include "dll.h"

bool error_flag;


void DLL_Error() {
	fprintf(stderr, "*ERROR* The program has performed an illegal operation.\n");
	error_flag = true;
}


void DLL_Init( DLList *list ) {
    list->firstElement = NULL;
    list->lastElement = NULL;
    list->activeElement = NULL;
}


void DLL_Dispose( DLList *list ) {

    if (list->firstElement != NULL){

        DLLElementPtr tmp;

        while (list->firstElement != NULL){
            tmp  = list->firstElement;
            list->firstElement = list->firstElement->nextElement;
            free(tmp);
        }
        list->lastElement = NULL;
        list->activeElement = NULL;
    }


}


void DLL_InsertFirst( DLList *list, int data ) {

    DLLElementPtr new = (DLLElementPtr) malloc(sizeof(struct DLLElement));

    if (new == NULL){
        DLL_Error();
    }
    else {
        new->data = data;
        new->nextElement = list->firstElement;
        new->previousElement = NULL;

        if (list->firstElement == NULL){
            list->lastElement = new;
        }
        else {
            list->firstElement->previousElement = new;
        }
        list->firstElement = new;
    }
}


void DLL_InsertLast( DLList *list, int data ) {

    DLLElementPtr new = (DLLElementPtr) malloc(sizeof(struct DLLElement));

    if (new == NULL){
        DLL_Error();
    }
    else {
        new->data = data;
        new->previousElement = list->lastElement;
        new->nextElement = NULL;

        if (list->firstElement == NULL){
            list->firstElement = new;
        }
        else {
            list->lastElement->nextElement = new;
        }
        list->lastElement = new;
    }
}


void DLL_First( DLList *list ) {
	list->activeElement = list->firstElement;
}


void DLL_Last( DLList *list ) {
	list->activeElement = list->lastElement;
}


void DLL_GetFirst( DLList *list, int *dataPtr ) {
	if (list->firstElement == NULL){
        DLL_Error();
    }
    else {
        *dataPtr = list->firstElement->data;
    }
}


void DLL_GetLast( DLList *list, int *dataPtr ) {
	if (list->lastElement == NULL){
        DLL_Error();
    }
    else {
        *dataPtr = list->lastElement->data;
    }
}


void DLL_DeleteFirst( DLList *list ) {

    if (list->firstElement != NULL){

        DLLElementPtr tmp;

        if (list->activeElement == list->firstElement){
            list->activeElement = NULL;
        }

        tmp = list->firstElement;

        if (list->firstElement->nextElement == NULL){
            list->firstElement = NULL;
            list->lastElement = NULL;
        }
        else {
            list->firstElement = list->firstElement->nextElement;
            list->firstElement->previousElement = NULL;
        }
        free(tmp);
    }
}


void DLL_DeleteLast( DLList *list ) {

    if (list->firstElement != NULL){

        DLLElementPtr tmp;

        if (list->activeElement == list->lastElement){
            list->activeElement = NULL;
        }

        tmp = list->lastElement;

        if (list->lastElement->previousElement == NULL){
            list->firstElement = NULL;
            list->lastElement = NULL;
        }
        else {
            list->lastElement = list->lastElement->previousElement;
            list->lastElement->nextElement = NULL;
        }
        free(tmp);
    }
}


void DLL_DeleteAfter( DLList *list ) {

    if (list->activeElement != NULL && list->activeElement != list->lastElement && list->firstElement != NULL){

        DLLElementPtr tmp;

        tmp = list->activeElement->nextElement;

        list->activeElement->nextElement = tmp->nextElement;

        if (tmp == list->lastElement){
            list->lastElement = list->activeElement;
        }
        else {
            tmp->nextElement->previousElement = list->activeElement;
        }
        free(tmp);
    }
}


void DLL_DeleteBefore( DLList *list ) {

    if (list->activeElement != NULL && list->activeElement != list->firstElement && list->firstElement != NULL){

        DLLElementPtr  tmp;

        tmp = list->activeElement->previousElement;

        list->activeElement->previousElement = tmp->previousElement;

        if (tmp == list->firstElement){
            list->firstElement = list->activeElement;
        }
        else {
            tmp->previousElement->nextElement = list->activeElement;
        }
        free(tmp);
    }
}


void DLL_InsertAfter( DLList *list, int data ) {

    if (list->activeElement != NULL && list->firstElement != NULL) {

        DLLElementPtr new = (DLLElementPtr) malloc(sizeof(struct DLLElement));

        if (new == NULL){
            DLL_Error();
        }
        else {
            new->data = data;
            new->previousElement = list->activeElement;
            new->nextElement = list->activeElement->nextElement;
            list->activeElement->nextElement = new;

            if (list->activeElement == list->lastElement) {
                list->lastElement = new;
            }
            else {
                new->nextElement->previousElement = new;
            }
        }
    }
}


void DLL_InsertBefore( DLList *list, int data ) {

    if (list->activeElement != NULL && list->firstElement != NULL){

        DLLElementPtr new = (DLLElementPtr) malloc(sizeof(struct DLLElement));

        if (new == NULL){
            DLL_Error();
        }
        else {
            new->data = data;
            new->nextElement = list->activeElement;
            new->previousElement = list->activeElement->previousElement;
            list->activeElement->previousElement = new;

            if (list->activeElement == list->firstElement){
                list->firstElement = new;
            }
            else {
                new->previousElement->nextElement = new;
            }
        }
    }
}


void DLL_GetValue( DLList *list, int *dataPtr ) {

    if (list->activeElement == NULL){
        DLL_Error();
    }
    else if (list->firstElement == NULL){
        return;
    }
    else {
        *dataPtr = list->activeElement->data;
    }
}


void DLL_SetValue( DLList *list, int data ) {

    if (list->activeElement != NULL) {
        list->activeElement->data = data;
    }
}


void DLL_Next( DLList *list ) {

    if (list->activeElement != NULL){
        list->activeElement = list->activeElement->nextElement;
    }
}



void DLL_Previous( DLList *list ) {

    if (list->activeElement != NULL){
        list->activeElement = list->activeElement->previousElement;
    }
}


int DLL_IsActive( DLList *list ) {
	return list->activeElement != NULL;
}

/* END OF FILE */
