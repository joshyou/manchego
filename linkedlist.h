/*
 * linkedlist.h
 * CS 251, Carleton College, Fall 2016
 */

#include <stdbool.h>
#include "value.h"

#ifndef _LINKEDLIST
#define _LINKEDLIST

/*
 * Create an empty list (a new Value object of type NULL_TYPE).
 */
Value *makeNull();

/*
 * Create a nonempty list (a new Value object of type CONS_TYPE).
 */
Value *cons(Value *car, Value *cdr);

/*
 * Print a representation of the contents of a linked list.
 */
void display(Value *list);

/*
 * Get the car value of a given list.  (Uses assertions to ensure that 
 * list has a car value.)
 */
Value *car(Value *list);

/*
 * Get the cdr value of a given list.  (Uses assertions to ensure that
 * list has a cdr value.)
 */
Value *cdr(Value *list);

/*
 * Test if the given value is a NULL_TYPE value.  (Uses assertions to
 * ensure that the input isn't NULL.)
 */
bool isNull(Value *value);

/*
 * Create a new linked list whose entries correspond to the given
 * list's entries, but in reverse order.  The resulting list is a
 * shallow copy of the original -- that is, list entries are not
 * copied, but rather the new list's cons cells should point to
 * precisely the items in the original list.  (Uses assertions to
 * ensure that input is indeed a list.)
 */
Value *reverse(Value *list);

/*
 * Compute the length of the given list.  (Uses assertions to
 * ensure that input is indeed a list.)
 */
int length(Value *value);

#endif
