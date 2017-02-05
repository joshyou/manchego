/*
 * value.h
 * CS 251, Carleton College, Fall 2016
 */

#ifndef _VALUE
#define _VALUE

   // #include "interpreter.h"



typedef enum {
   PTR_TYPE,
   INT_TYPE,
   DOUBLE_TYPE,
   STR_TYPE,
   CONS_TYPE,
   NULL_TYPE,
   OPEN_TYPE,
   CLOSE_TYPE,
   BOOL_TYPE,
   SYMBOL_TYPE,
   VOID_TYPE,
   PRIMITIVE_TYPE,
   CLOSURE_TYPE
} valueType;

struct __Value {
   valueType type;
   union {
      void *p;
      int i;
      double d;
      char *s;
      struct ConsCell {
	     struct __Value *car;
	     struct __Value *cdr;
      } c;
      struct ClosureCell {
         struct __Value *params;
         struct __Value *body;
         struct __Frame *frame;
      } cc;
      /* A pointer to a primitive function (of the appropriate type).
      * Note: pf is the variable name for the function pointer. */
      struct __Value *(*pf)(struct __Value *);
   };
};

typedef struct __Value Value;

#endif
