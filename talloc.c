/*
 * talloc.c
 * CS 251, Carleton College, Fall 2016
 */

#include "value.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

Value *activeList;
Value *lastElement;

// debugging function
int len(Value *list) {
  printf("len starts\n");
  int counter = 0;
  while (list->type != NULL_TYPE) {
    counter++;
    list = list->c.cdr;
  }
  printf("len finishes\n");
  return counter;
}

/*
 * A malloc-like function that allocates memory, tracking all
 * allocated pointers in the Active List.  (You can choose your
 * implementation of the Active List, but whatever it is, your talloc
 * code should NOT call functions in linkedlist.h; instead, implement
 * any list-like behavior directly here. Otherwise you'll end up with
 * circular dependencies, since you're going to modify the linked list
 * to use talloc instead of malloc.)
 */
void *talloc(size_t size) {
  // if activeList has not yet been initialized, do so
  if (!activeList) {
    activeList = malloc(sizeof(Value));
    // the last element of the list is a NULL_TYPE
    if (activeList) {
      activeList->type = NULL_TYPE;
    }
  }

  void *returnPointer = malloc(size);
  if (returnPointer) {
    // addedNode is a new node in the active list
    Value *addedNode = malloc(sizeof(Value));
    if (addedNode) {
      addedNode->type = CONS_TYPE;
      addedNode->c.cdr = activeList;
      // ptrNode is the car of each addedNode that stores the pointer
      Value *ptrNode = malloc(sizeof(Value));
      if (ptrNode) {
        ptrNode->type = PTR_TYPE;
        ptrNode->p = returnPointer;
        addedNode->c.car = ptrNode;
        activeList = addedNode;
      } else {
        printf("TALLOC ERROR: Failed to create pointer storage\n");
        return returnPointer;
      }
    } else {
      printf("TALLOC ERROR: Failed to create new node in talloc\n");
      return returnPointer;
    }
    return returnPointer;
  } else {
    printf("TALLOC ERROR: Failed to allocate requested object\n");
    return returnPointer;
  }
}

/*
 * Free all pointers allocated by talloc, as well as whatever memory
 * you malloc'ed to create/update the Active List.
 */
void tfree() {
  Value *tempList = activeList;
  Value *tempList2;
  while (tempList->type != NULL_TYPE) {
    // frees talloc'd objects
    free(tempList->c.car->p);

    // this frees the PTR_TYPE values storage nodes
    free(tempList->c.car);

    tempList2 = tempList->c.cdr;

    // this frees the CONS_TYPE values entries in the active list
    free(tempList);

    tempList = tempList2;
  }
  free(tempList);
  // free(activeList);
  activeList = NULL;
}

/*
 * A simple two-line function to stand in the C function "exit", which
 * calls tfree() and then exit().  (You'll use this later to allow a
 * clean exit from your interpreter when you encounter an error.)
 */
void texit(int status) {
  tfree();
  exit(status);
}
