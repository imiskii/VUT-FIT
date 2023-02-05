/**
 * @file sll.c
 * @author Michal Ľaš
 * @brief Singly Linked List
 * 
 */


#include "sll.h"


bool error_flag;


void List_Error() {
    fprintf(stderr, "*ERROR* The program has performed an illegal operation.\n");
    error_flag = true;
}


void List_Init(List *list) {
    list->firstElement = NULL;
    list->activeElement = NULL;
}


void List_Dispose(List *list) {

    ListElementPtr tmp;

    while (list->firstElement != NULL){
        tmp = list->firstElement;
        list->firstElement = list->firstElement->nextElement;
        free(tmp);
    }
    list->activeElement = NULL;
}


void List_InsertFirst(List *list, int data) {

    ListElementPtr new = (ListElementPtr) malloc(sizeof(struct ListElement));

    if (new == NULL){
        List_Error();
    }
    else{
        new->data = data;
        new->nextElement = list->firstElement;
        list->firstElement = new;
    }
}


void List_First(List *list) {
    list->activeElement = list->firstElement;
}


void List_GetFirst(List *list, int *dataPtr) {

    if (list->firstElement == NULL) {
        List_Error();
    }
    else {
        *dataPtr = list->firstElement->data;
    }
}


void List_DeleteFirst(List *list) {

    ListElementPtr tmp;

    if (list->firstElement != NULL){
        if (list->activeElement == list->firstElement){
            list->activeElement = NULL;
        }
        tmp = list->firstElement;
        list->firstElement = list->firstElement->nextElement;
        free(tmp);
    }
}


void List_DeleteAfter(List *list) {

    ListElementPtr  tmp;

    if (list->activeElement != NULL && list->activeElement->nextElement != NULL && list->firstElement != NULL){
        tmp = list->activeElement->nextElement;
        list->activeElement->nextElement = tmp->nextElement;
        free(tmp);
    }
}


void List_InsertAfter(List *list, int data) {

    if (list->firstElement != NULL && list->activeElement != NULL){

        ListElementPtr new = (ListElementPtr) malloc(sizeof(struct ListElement));

        if (new == NULL){
            List_Error();
        }
        else {
            new->data = data;
            new->nextElement = list->activeElement->nextElement;
            list->activeElement->nextElement = new;
        }
    }


}


void List_GetValue(List *list, int *dataPtr) {

    if (list->firstElement != NULL){
        if (list->activeElement == NULL){
            List_Error();
        }
        else {
            *dataPtr = list->activeElement->data;
        }
    }
}


void List_SetValue(List *list, int data) {
    if (list->activeElement != NULL) {
        list->activeElement->data = data;
    }
}


void List_Next(List *list) {
    if (list->activeElement != NULL && list->firstElement != NULL){
        list->activeElement = list->activeElement->nextElement;
    }
}


int List_IsActive(List *list) {
    return list->activeElement != NULL;
}

/* Konec c201.c */
