/**
 * @file symtable.c
 * 
 * @brief Double linked list implementation - inspired by IAL
 * @author Šimon Šmída   - xsmida03
 */

#include "include/dll_list.h"
#include "include/error.h"

#define MAXLEN 100

void print_elements_of_list(DLList TL) 
{
	// DLList TempList = TL;
	// int CurrListLength = 0;
	// printf("-----------------");
	// while ((TempList.firstElement != NULL) && (CurrListLength < MAXLEN))
	// {
	// 	printf("\n \t%s (%d)", TempList.firstElement->item->key, TempList.firstElement->item->const_var->type);
	// 	if ((TempList.firstElement == TL.activeElement) && (TL.activeElement != NULL))
	// 		printf("\t <= active element");
	// 	TempList.firstElement = TempList.firstElement->nextElement;
	// 	CurrListLength++;
	// }
	// if (CurrListLength >= MAXLEN)
	// 	printf("\nList exceeded maximum length!");
	// printf("\n-----------------\n");
}

/**
 * @brief Notifies user about invalid doubly linked list operation
 */
void DLL_Error() {
    error_message("FATAL", ERR_INTERNAL, "doubly linked list operation failure");
}

/**
 * @brief Initialize linked list
 */
void DLL_Init(DLList *list) {
    // DLL initialization - set pointers to NULL
    list->firstElement  = NULL;
    list->activeElement = NULL;
    list->lastElement   = NULL;
}

/**
 * @brief Destroy every item of linked list
 */
void DLL_Dispose( DLList *list) 
{
    DLLElementPtr curr = list->firstElement;
    DLLElementPtr next;

    // Traverse the linked list
    while (curr != NULL) {
        next = curr->nextElement;
        // Deallocate current element
        free(curr);
        // Set new current element
        curr = next;
    }

    // Set every linked list pointers to NULL
    list->firstElement  = NULL;
    list->activeElement = NULL;
    list->lastElement   = NULL;
}

/**
 * @brief Insert new element at the start of the list
 */
void DLL_InsertFirst(DLList *list, symtable_item_t *item) 
{    
    // Allocate memory for the new element
    DLLElementPtr new_el = (DLLElementPtr)malloc(sizeof(struct DLLElement));
    if (new_el == NULL) { 
        DLL_Error(); // malloc failure
        return;;
    }

    // Set information about the new element
    new_el->item = item;
    new_el->previousElement = NULL;
    new_el->nextElement = list->firstElement;

    if (list->firstElement != NULL) { // list is not empty
        list->firstElement->previousElement = new_el;
    } else { // list is empty -> new element is the last one as well
        list->lastElement = new_el;
    }

    // Set new element to be the first in the list
    list->firstElement = new_el;
}

/**
 * @brief Insert new element at the end of the list
 */
void DLL_InsertLast(DLList *list, symtable_item_t *item) 
{
    // Allocate memory for the new element
    DLLElementPtr new_el = (DLLElementPtr)malloc(sizeof(struct DLLElement));
    if (new_el == NULL) { 
        DLL_Error(); // malloc failure
        return;
    }

    // Set information about the new element
    new_el->item = item;
    new_el->previousElement = list->lastElement;
    new_el->nextElement = NULL;

    if (list->lastElement != NULL) { // list is not empty
        list->lastElement->nextElement = new_el;
    } else { // list is empty -> new element is the first one as well
        list->firstElement = new_el;
    }
    
    // Set new element to be the last in the list
    list->lastElement = new_el;    
}

/**
 * @brief Set first element to be active
 */
void DLL_First( DLList *list ) {
    list->activeElement = list->firstElement;
}

/**
 * @brief Set last element to be active
 */
void DLL_Last( DLList *list ) {
    list->activeElement = list->lastElement;
}

/**
 * @brief Get the value from the first element 
 */
void DLL_GetFirst(DLList *list, symtable_item_t **item) 
{
    if (list->firstElement == NULL) {
        DLL_Error(); // list is empty
        return;
    }
    *item = list->firstElement->item;
}

/**
 * @brief Get the value from the last element 
 */
void DLL_GetLast( DLList *list, symtable_item_t **item) 
{
    if (list->lastElement == NULL) {
        DLL_Error(); // list is empty
        return;
    }
    *item = list->lastElement->item;
}

/**
 * @brief Destroy first element of the list
 */
void DLL_DeleteFirst(DLList *list) 
{
    if (list->firstElement == NULL) {
        return; // list is empty
    }
    if (list->firstElement == list->activeElement) { // first el is active
        list->activeElement = NULL; // loss of activity
    } 
    if (list->firstElement == list->lastElement) {
        list->lastElement = NULL; // list contained only 1 element
    }

    // Store element after the first one
    DLLElementPtr temp = list->firstElement->nextElement;
    // Deallocate the first element 
    free(list->firstElement);
    // Set new first element
    list->firstElement = temp;

    if (list->firstElement != NULL) {
        // New first element does not have previous element
        list->firstElement->previousElement = NULL;
    }
}

/**
 * @brief Destroy last element of the list
 */
void DLL_DeleteLast(DLList *list) 
{
    if (list->lastElement == NULL) {
        return; // list is empty
    }
    if (list->lastElement == list->activeElement) {
        list->activeElement = NULL; // loss of activity
    }
    if (list->firstElement == list->lastElement) {
        list->firstElement = NULL; // list contained only 1 element
    }
    
    // Store element before the last one
    DLLElementPtr temp = list->lastElement->previousElement;
    // Deallocate the last element 
    free(list->lastElement);
    // Set new last element
    list->lastElement = temp;

    if (list->lastElement != NULL) {
        // New last element does not have next element
        list->lastElement->nextElement = NULL;
    }
}

/**
 * @brief Desktroy element after active one
 */
void DLL_DeleteAfter(DLList *list) 
{
    if (list->activeElement == NULL) {
        return; // list is not active
    }

    if (list->activeElement->nextElement != NULL) { // is there anything to be removed?
        DLLElementPtr temp = list->activeElement->nextElement; // ptr to the one to be deleted
        list->activeElement->nextElement = temp->nextElement; // create new connection
        if (temp == list->lastElement) { // the one to be deleted is the last one
            list->lastElement = list->activeElement; // new last element
        } else { // preserve dll properties
            temp->nextElement->previousElement = list->activeElement;
        }
        free(temp);
    }
}

/**
 * @brief Destroy element before active one
 */
void DLL_DeleteBefore(DLList *list) 
{
    if (list->activeElement == NULL) {
        return; // list is not active
    }
    if (list->activeElement->previousElement != NULL) { // is there anything to be removed?
        DLLElementPtr temp = list->activeElement->previousElement; // ptr to the one to be deleted
        list->activeElement->previousElement = temp->previousElement; // create new connection
        if (temp == list->firstElement) { // the one to be deleted is the last one
            list->firstElement = list->activeElement; // new last element
        } else { // preserve dll properties
            temp->previousElement->nextElement = list->activeElement;
        }
        free(temp);
    }
}

/**
 * @brief Insert new element after active one
 */
void DLL_InsertAfter(DLList *list, symtable_item_t *item) 
{
    if (list->activeElement == NULL) {
        return; // list has no active element
    }

    // Create new element to be inserted
    DLLElementPtr new_el = (DLLElementPtr)malloc(sizeof(struct DLLElement));
    if (new_el == NULL) {
        DLL_Error(); // malloc failure
        return;
    }
    
    // Set information about the new element
    new_el->item = item;
    new_el->previousElement = list->activeElement;
    new_el->nextElement = list->activeElement->nextElement;

    if (list->lastElement == list->activeElement) {
        // New element become new last element
        list->lastElement = new_el;
    } else { // active element is not the last one
        // Set previous element of the one after the active one 
        // to point to the new element
        DLLElementPtr temp = list->activeElement->nextElement;
        temp->previousElement = new_el;
    }
    // Insert new element after the active element
    list->activeElement->nextElement = new_el;
}

/**
 * @brief Insert element before active one
 */
void DLL_InsertBefore(DLList *list, symtable_item_t *item) 
{
    if (list->activeElement == NULL) {
        return; // list has no active element
    }

    // Create new element to be inserted
    DLLElementPtr new_el = (DLLElementPtr)malloc(sizeof(struct DLLElement));
    if (new_el == NULL) {
        DLL_Error(); // malloc failure
        return;
    }
    
    // Set information about the new element
    new_el->item = item;
    new_el->previousElement = list->activeElement->previousElement;
    new_el->nextElement = list->activeElement;

    if (list->firstElement == list->activeElement) {
        // New element become new first element
        list->firstElement = new_el;
    } else { // active element is not the first one
        // Set next element of the one before the active one 
        // to point to the new element
        DLLElementPtr temp = list->activeElement->previousElement;
        temp->nextElement = new_el;
    }
    // Insert new element before the active element
    list->activeElement->previousElement = new_el;
}

/**
 * @brief via parameter data returns value of the active element
 */
void DLL_GetValue( DLList *list, symtable_item_t **item) 
{
    if (list->activeElement == NULL) {
        printf("HEEEERE\n\n\n");
        DLL_Error(); // list has no active element
        return;
    }
    *item = list->activeElement->item;
}

/**
 * @breif Set value of the active element
 */
void DLL_SetValue(DLList *list, symtable_item_t *item) 
{
    if (list->activeElement == NULL) {
        return;
    }
    list->activeElement->item = item;
}

/**
 * @brief Shift activity to the next dll element
 */
void DLL_Next(DLList *list) 
{
    if (list->activeElement == NULL) {
        return;
    }
    // Shift the activity of an element to the next one
    list->activeElement = list->activeElement->nextElement;
}


/**
 * @brief Shift activity to the previous dll element
 */
void DLL_Previous(DLList *list) 
{
    if (list->activeElement == NULL) {
        return;
    }
    // Shift the activity of an element to the previous one
    list->activeElement = list->activeElement->previousElement;
}

/**
 * @brief If the dll is active returns non zero value, otherwise 0
 */
int DLL_IsActive(DLList *list) 
{
    return (list->activeElement != NULL);
}

