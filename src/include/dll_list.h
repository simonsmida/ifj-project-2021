#ifndef DLL_LIST_H
#define DLL_LIST_H

#include "symtable.h"

typedef struct DLLElement {
	symtable_item_t *item;
	struct DLLElement *previousElement;
	struct DLLElement *nextElement;
} *DLLElementPtr;

typedef struct {
	DLLElementPtr firstElement;
	DLLElementPtr activeElement;
	DLLElementPtr lastElement;
} DLList;

void print_elements_of_list(DLList TL);
int dll_item_count(DLList TL);

/**
 * @brief Notifies user about invalid doubly linked list operation
 */
void DLL_Error();

/**
 * @brief Initialize linked list
 */
void DLL_Init(DLList *list);

/**
 * @brief Destroy every item of linked list
 */
void DLL_Dispose( DLList *list);

/**
 * @brief Insert new element at the start of the list
 */
void DLL_InsertFirst(DLList *list, symtable_item_t *item);

/**
 * @brief Insert new element at the end of the list
 */
void DLL_InsertLast(DLList *list, symtable_item_t *item);

/**
 * @brief Set first element to be active
 */
void DLL_First(DLList *list);

/**
 * @brief Set last element to be active
 */
void DLL_Last(DLList *list);

/**
 * @brief Get the value from the first element 
 */
void DLL_GetFirst(DLList *list, symtable_item_t **item);

/**
 * @brief Get the value from the last element 
 */
void DLL_GetLast( DLList *list, symtable_item_t **item);

/**
 * @brief Destroy first element of the list
 */
void DLL_DeleteFirst(DLList *list); 

/**
 * @brief Destroy last element of the list
 */
void DLL_DeleteLast(DLList *list);

/**
 * @brief Desktroy element after active one
 */
void DLL_DeleteAfter(DLList *list);

/**
 * @brief Destroy element before active one
 */
void DLL_DeleteBefore(DLList *list);

/**
 * @brief Insert new element after active one
 */
void DLL_InsertAfter(DLList *list, symtable_item_t *item);

/**
 * @brief Insert element before active one
 */
void DLL_InsertBefore(DLList *list, symtable_item_t *item); 

/**
 * @brief via parameter data returns value of the active element
 */
int DLL_GetValue( DLList *list, symtable_item_t **item); 

/**
 * @breif Set value of the active element
 */
void DLL_SetValue(DLList *list, symtable_item_t *item);

/**
 * @brief Shift activity to the next dll element
 */
void DLL_Next(DLList *list);

/**
 * @brief Shift activity to the previous dll element
 */
void DLL_Previous(DLList *list); 

/**
 * @brief If the dll is active returns non zero value, otherwise 0
 */
int DLL_IsActive(DLList *list); 

#endif
