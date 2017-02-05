/*
 * linkedlist.c
 * CS 251, Carleton College, Fall 2016
 */

#include "talloc.h"
#include "value.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Create an empty list (a new Value object of type NULL_TYPE).
 */
Value *makeNull() {
  // Value *newList = malloc(sizeof(Value));
  Value *newList = talloc(sizeof(Value));
  if (newList == NULL) {
    return newList;
  } else {
    newList->type = NULL_TYPE;
    return newList;
  }
}

/*
 * Create a nonempty list (a new Value object of type CONS_TYPE).
 */
Value *cons(Value *car, Value *cdr) {
  Value *newList = makeNull();
  if (newList != NULL) {
    newList->type = CONS_TYPE;
    newList->c.car = car;
    newList->c.cdr = cdr;
    return newList;
  } else {
    return newList;
  }
}

/*
 * Recursive version of display, to avoid printing extra newlines
 */
void display_helper(Value *list) {
  switch (list->type) {
  case PTR_TYPE:
    printf("%p ", list->p);
    break;
  case INT_TYPE:
    printf("%d ", list->i);
    break;
  case DOUBLE_TYPE:
    printf("%f ", list->d);
    break;
  case STR_TYPE:
    printf("%s ", list->s);
    break;
  case OPEN_TYPE:
    printf("%s ", list->s);
    break;
  case CLOSE_TYPE:
    printf("%s ", list->s);
    break;
  case SYMBOL_TYPE:
    printf("%s ", list->s);
    break;
  case BOOL_TYPE:
    printf("%i ", list->i);
    break;
  case CLOSURE_TYPE:
    printf("<procedure at %p> ", list->p);
    break;
  case CONS_TYPE:
    // prints brackets when the car is itself a list
    if (list->c.car->type == CONS_TYPE) {
      printf("[");
      display_helper(list->c.car);
      printf("]");
    } else {
      display_helper(list->c.car);
    }
    display_helper(list->c.cdr);
    break;
  case PRIMITIVE_TYPE:
    printf("<a primitive>");
    break;
  case NULL_TYPE:
    break;
  case VOID_TYPE:
    break;
  default:
    break;
  }
}

/*
 * Print a representation of the contents of a linked list.
 */
void display(Value *list) {
  printf("[");
  display_helper(list);
  printf("]\n");
}

void printType(Value *list) {
  switch (list->type) {
  case (PTR_TYPE):
    printf("PTR_TYPE\n");
    break;
  case (INT_TYPE):
    printf("INT_TYPE\n");
    break;
  case (DOUBLE_TYPE):
    printf("DOUBLE_TYPE\n");
    break;
  case (STR_TYPE):
    printf("STR_TYPE\n");
    break;
  case (CONS_TYPE):
    printf("CONS_TYPE\n");
    break;
  case (NULL_TYPE):
    printf("NULL_TYPE\n");
    break;
  case (OPEN_TYPE):
    printf("OPEN_TYPE\n");
    break;
  case (CLOSE_TYPE):
    printf("CLOSE_TYPE\n");
    break;
  case (BOOL_TYPE):
    printf("BOOL_TYPE\n");
    break;
  case (SYMBOL_TYPE):
    printf("SYMBOL_TYPE\n");
    break;
  case (VOID_TYPE):
    printf("VOID_TYPE\n");
    break;
  case (PRIMITIVE_TYPE):
    printf("PRIMITIVE_TYPE\n");
    break;
  case (CLOSURE_TYPE):
    printf("CLOSURE_TYPE\n");
    break;
  }
}

/*
 * Get the car value of a given list.  (Uses assertions to ensure that
 * list has a car value.)
 */
Value *car(Value *list) {
  if (list->type != CONS_TYPE) {
    printf("LINKED LIST ERROR: car was called on input type: ");
    printType(list);
    printf("the input: ");
    display(list);
    texit(1);
  }
  assert(list->type == CONS_TYPE);
  return list->c.car;
}

/*
 * Get the cdr value of a given list.  (Uses assertions to ensure that
 * list has a cdr value.)
 */
Value *cdr(Value *list) {
  if (list->type != CONS_TYPE) {
    printf("LINKED LIST ERROR: cdr was called on input type: ");
    printType(list);
    printf("the input: ");
    display(list);
    texit(1);
  }
  return list->c.cdr;
}

/*
 * Test if the given value is a NULL_TYPE value.  (Uses assertions to
 * ensure that the input isn't NULL.)
 */
bool isNull(Value *value) {
  assert(value != NULL);
  if (value->type == NULL_TYPE) {
    return true;
  } else {
    return false;
  }
}

/*
 * Create a new linked list whose entries correspond to the given
 * list's entries, but in reverse order.  The resulting list is a
 * shallow copy of the original -- that is, list entries are not
 * copied, but rather the new list's cons cells should point to
 * precisely the items in the original list.  (Uses assertions to
 * ensure that input is indeed a list.)
 */
Value *reverse(Value *list) {
  assert(list->type == NULL_TYPE || list->type == CONS_TYPE);
  Value *newList = makeNull();
  while (!isNull(list)) {
    newList = cons(car(list), newList);
    list = cdr(list);
  }
  return newList;
}

/*
 * Compute the length of the given list.  (Uses assertions to
 * ensure that input is indeed a list.)
 */
int length(Value *value) {
  assert(value->type == NULL_TYPE || value->type == CONS_TYPE);
  int count = 0;
  bool done = false;
  while (!done) {
    if (value->type == CONS_TYPE) {
      count++;
      value = cdr(value);
    } else if (value->type == NULL_TYPE) {
      done = true;
    } else {
      count++;
      done = true;
    }
  }
  return count;
}
