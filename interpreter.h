#include "value.h"

struct __Frame {
  Value *bindings;
  struct __Frame *parent;
};
typedef struct __Frame Frame;

/* handle a list of S-expressions (i.e., a Scheme program):
    call eval on each S-expression in the top-level (global) environment,
    and print each result in turn. */
void interpret(Value *tree);

/* takes a parse tree of a Scheme expression (as in the output of your parser)
     and an environment frame in which to evaluate that expression, and returns
     a pointer to Value representing the value */
Value *eval(Value *expr, Frame *frame);
