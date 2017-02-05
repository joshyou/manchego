#include "linkedlist.h"
#include "parser.h"
#include "talloc.h"
#include "value.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct __Frame {
  Value *bindings;
  struct __Frame *parent;
};
typedef struct __Frame Frame;

// forward declarations

void printResult(Value *result, bool printNewLine, bool first, bool last,
                 bool isInnerList);

Value *eval(Value *expr, Frame *frame);

Value *makeBinding(Value *symbol, Value *val);

void evaluationError() { texit(1); }

void bind(char *name, Value *(*function)(Value *), Frame *frame) {
  Value *pName = makeNull();
  pName->type = SYMBOL_TYPE;
  pName->s = name;

  Value *value = talloc(sizeof(Value));
  value->type = PRIMITIVE_TYPE;
  value->pf = function;

  frame->bindings = cons(makeBinding(pName, value), frame->bindings);
}

// PRIMITIVE FUNCTIONS
Value *primitiveNot(Value *args) {
  Value *returnVal = makeNull();
  returnVal->type = BOOL_TYPE;
  if (args->type == CONS_TYPE) {
    if (car(args)->type == BOOL_TYPE) {
      if (car(args)->i == 0) {
        // if the bool value is false, return true
        returnVal->i = 1;
      } else {
        // if the bool value is true, return false
        returnVal->i = 0;
      }
    } else {
      // if not a bool, return false
      returnVal->i = 0;
    }
  } else {
    printf("EVALUATION ERROR: invalid argument for not\n");
    evaluationError();
  }
  return returnVal;
}

Value *primitiveZero(Value *args) {
  Value *returnVal = makeNull();
  returnVal->type = BOOL_TYPE;
  returnVal->i = 0;
  if (args->type == CONS_TYPE) {
    if (car(args)->type == INT_TYPE) {
      if (car(args)->i == 0) {
        returnVal->i = 1;
      }
    } else if (car(args)->type == DOUBLE_TYPE) {
      if (car(args)->d == 0.0) {
        returnVal->i = 1;
      }
    } else {
      printf("EVALUATION ERROR: zero? only takes numerals\n");
      evaluationError();
    }
  } else {
    printf("EVALUATION ERROR: zero? only takes numerals\n");
    evaluationError();
  }
  return returnVal;
}

/*
 * (+ num1 num2 ...)
 * implements + for scheme. Returns an INT_TYPE only if both inputs are
 * INT_TYPEs. Otherwise returns a DOUBLE_TYPE
 */
Value *primitiveAdd(Value *args) {
  if (length(args) < 2) {
    printf("EVALUATION ERROR: + takes at least 2 arguments\n");
    evaluationError();
  }
  Value *tempArgs = args;
  bool containsDouble = false;
  double sum = 0;

  while (tempArgs->type != NULL_TYPE) {
    if (car(tempArgs)->type == INT_TYPE) {
      sum = sum + car(tempArgs)->i;
    } else if (car(tempArgs)->type == DOUBLE_TYPE) {
      sum = sum + car(tempArgs)->d;
      containsDouble = true;
    } else {
      printf("EVALUATIION ERROR: non-numeral argument in +.\n");
      evaluationError();
    }
    tempArgs = cdr(tempArgs);
  }

  Value *sumVal = makeNull();
  if (containsDouble) {
    sumVal->type = DOUBLE_TYPE;
    sumVal->d = sum;
  } else {
    sumVal->type = INT_TYPE;
    sumVal->i = (int)sum;
  }
  return sumVal;
}

Value *primitiveMinus(Value *args) {
  if (length(args) < 2) {
    printf("EVALUATION ERROR: - takes at least 2 arguments\n");
    evaluationError();
  }
  Value *firstNum = car(args);
  Value *subtractBy;
  if (length(args) > 2) {
    subtractBy = primitiveAdd(cdr(args));
  } else {
    subtractBy = car(cdr(args));
  }
  Value *result = makeNull();
  if (firstNum->type == INT_TYPE) {
    if (subtractBy->type == INT_TYPE) {
      result->type = INT_TYPE;
      result->i = firstNum->i - subtractBy->i;
    } else if (subtractBy->type == DOUBLE_TYPE) {
      result->type = DOUBLE_TYPE;
      result->d = firstNum->i - subtractBy->d;
    } else {
      printf("EVALUATION ERROR: non-numeral in minus\n");
      evaluationError();
    }
  } else if (firstNum->type == DOUBLE_TYPE) {
    if (subtractBy->type == INT_TYPE) {
      result->type = DOUBLE_TYPE;
      result->d = firstNum->d - subtractBy->i;
    } else if (subtractBy->type == DOUBLE_TYPE) {
      result->type = DOUBLE_TYPE;
      result->d = firstNum->d - subtractBy->d;
    } else {
      printf("EVALUATION ERROR: non-numeral in minus\n");
      evaluationError();
    }
  } else {
    printf("EVALUATION ERROR: non-numeral in minus\n");
    evaluationError();
  }
  return result;
}

Value *primitiveMult(Value *args) {
  if (length(args) < 2) {
    printf("EVALUATION ERROR: expected at least 2 arguments for *\n");
    evaluationError();
  }
  Value *tempArgs = args;
  bool containsDouble = false;
  double prod = 1;

  while (tempArgs->type != NULL_TYPE) {
    if (car(tempArgs)->type == INT_TYPE) {
      prod = prod * car(tempArgs)->i;
      tempArgs = cdr(tempArgs);
    } else if (car(tempArgs)->type == DOUBLE_TYPE) {
      prod = prod * car(tempArgs)->d;
      tempArgs = cdr(tempArgs);
      containsDouble = true;
    } else {
      printf("EVALUATIION ERROR: * only takes ints and doubles.\n");
      evaluationError();
    }
  }

  if (containsDouble) {
    Value *prodVal = makeNull();
    prodVal->type = DOUBLE_TYPE;
    prodVal->d = prod;
    return prodVal;
  } else {
    Value *prodVal = makeNull();
    prodVal->type = INT_TYPE;
    prodVal->i = (int)prod;
    return prodVal;
  }
}

Value *primitiveDivide(Value *args) {
  if (length(args) < 2) {
    printf("EVALUATION ERROR: expected at least 2 arguments for /\n");
    evaluationError();
  }
  Value *firstNum = car(args);
  Value *divideBy;
  if (length(args) > 2) {
    divideBy = primitiveMult(cdr(args));
  } else {
    divideBy = car(cdr(args));
  }
  if (divideBy->type == INT_TYPE) {
    if (divideBy->i == 0) {
      printf("EVALUATION ERROR: You cannot divide by zero\n");
      evaluationError();
    }
  } else if (divideBy->type == DOUBLE_TYPE) {
    if (divideBy->d == 0.0) {
      printf("EVALUATION ERROR: You cannot divide by zero\n");
      evaluationError();
    }
  }
  Value *result = makeNull();

  if (firstNum->type == INT_TYPE) {
    if (divideBy->type == INT_TYPE) {
      if ((firstNum->i % divideBy->i) == 0) {

        result->type = INT_TYPE;
        result->i = firstNum->i / divideBy->i;
      } else {

        result->type = DOUBLE_TYPE;
        result->d = (double)firstNum->i / (double)divideBy->i;
      }
    } else if (divideBy->type == DOUBLE_TYPE) {
      result->type = DOUBLE_TYPE;
      result->d = firstNum->i / divideBy->d;
    } else {
      printf("EVALUATION ERROR: non-numeral in divide\n");
      evaluationError();
    }
  } else if (firstNum->type == DOUBLE_TYPE) {
    if (divideBy->type == INT_TYPE) {
      result->type = DOUBLE_TYPE;
      result->d = firstNum->d / divideBy->i;
    } else if (divideBy->type == DOUBLE_TYPE) {
      result->type = DOUBLE_TYPE;
      result->d = firstNum->d / divideBy->d;
    } else {
      printf("EVALUATION ERROR: non-numeral in divide\n");
      evaluationError();
    }
  } else {
    printf("EVALUATION ERROR: non-numeral in divide\n");
    evaluationError();
  }

  return result;
}

Value *primitiveNull(Value *args) {
  // check that there is only one argument
  if (length(args) > 1) {
    printf("EVALUATION ERROR: null? only takes one argument.\n");
    evaluationError();
  } else if (args->type == CONS_TYPE) {
    if (car(args)->type == NULL_TYPE) {
      Value *result = makeNull();
      result->type = BOOL_TYPE;
      result->i = 1;
      return result;
    }
  }
  Value *result = makeNull();
  result->type = BOOL_TYPE;
  result->i = 0;
  return result;
}

Value *primitiveCar(Value *args) {
  Value *carReturn = NULL;
  if (length(args) > 1) {
    printf("EVALUATION ERROR: car only takes one argument.\n");
    evaluationError();
  } else if (car(args)->type == CONS_TYPE) {
    carReturn = car(car(args));
  } else {
    printf("EVALUATION ERROR: car must be called on a list\n");
    evaluationError();
  }
  return carReturn;
}

Value *primitiveCdr(Value *args) {
  Value *cdrReturn = NULL;
  if (length(args) > 1) {
    printf("EVALUATION ERROR: cdr only takes one argument.\n");
    evaluationError();
  } else if (car(args)->type == CONS_TYPE) {
    cdrReturn = cdr(car(args));
  } else {
    printf("EVALUATION ERROR: invalid input for cdr\n");
    evaluationError();
  }
  return cdrReturn;
}

/*
 * implements cons for Scheme
 * (cons 1 2) -> (1 . 2)
 * (cons 1 '(2)) -> (1 2)
 */
Value *primitiveCons(Value *args) {
  Value *consReturn = NULL;
  if (length(args) != 2) {
    printf("EVALUATION ERROR: cons only takes 2 arguments\n");
  } else {
    consReturn = cons(car(args), car(cdr(args)));
  }
  return consReturn;
}

/*
 * implements <= for scheme
 * for numbers only
 */
Value *primitiveLeq(Value *args) {
  // check that there are at least two args
  Value *result = makeNull();
  result->type = BOOL_TYPE;
  if (length(args) < 2) {
    evaluationError();
  } else if (length(args) > 2) {
    if (primitiveLeq(cdr(args))->i == 0) {
      result->i = 0;
      return result;
    }
  }
  Value *firstNum = car(args);
  Value *secondNum = car(cdr(args));
  if (firstNum->type == INT_TYPE) {
    if (secondNum->type == INT_TYPE) {
      if (secondNum->i < firstNum->i) {
        result->i = 0;
        return result;
      } else {
        result->i = 1;
        return result;
      }
    } else if (secondNum->type == DOUBLE_TYPE) {
      if (secondNum->d < firstNum->i) {
        result->i = 0;
        return result;
      } else {
        result->i = 1;
        return result;
      }
    } else {
      printf("<= requires integers or doubles\n");
      evaluationError();
    }
  } else if (firstNum->type == DOUBLE_TYPE) {
    if (secondNum->type == INT_TYPE) {
      if (secondNum->i < firstNum->d) {
        result->i = 0;
        return result;
      } else {
        result->i = 1;
        return result;
      }
    } else if (secondNum->type == DOUBLE_TYPE) {
      if (secondNum->d < firstNum->d) {
        result->i = 0;
        return result;
      } else {
        result->i = 1;
        return result;
      }
    } else {
      printf("<= requires integers or doubles\n");
      evaluationError();
    }
  } else {
    printf("<= requires integers or doubles\n");
    evaluationError();
  }
  if (!result->i) {
    printf("something went wrong in <=\n");
    evaluationError();
  }
  return result;
  // if (firstNum->type == DOUBLE_TYPE)
}

/*
 * checks if the given arguments represent the same objects in memory
 * i.e. same Value* objects
 */
Value *primitiveEq(Value *args) {
  Value *result = makeNull();
  if (length(args) != 2) {
    printf("EVALUATION ERROR: expected 2 arguments for eq?\n");
    evaluationError();
  } else {
    result->type = BOOL_TYPE;
    Value *first = car(args);
    Value *second = car(cdr(args));
    // compare memory addresses
    if (first == second) {
      result->i = 1;
    } else {
      result->i = 0;
    }
  }
  return result;
}

Value *primitiveEqual(Value *args) {
  Value *result = makeNull();
  result->type = BOOL_TYPE;
  // default return is false
  result->i = 0;
  if (length(args) != 2) {
    printf("EVALUATION ERROR: equal? expected 2 arguments\n");
    evaluationError();
  } else {
    Value *first = car(args);
    Value *second = car(cdr(args));
    // object types must be the same
    if (first->type == second->type) {
      switch (first->type) {
      case (INT_TYPE): {
        if (first->i == second->i) {
          result->i = 1;
        }
        break;
      }
      case (BOOL_TYPE): {
        if (first->i == second->i) {
          result->i = 1;
        }
        break;
      }
      case (DOUBLE_TYPE): {
        if (first->d == second->d) {
          result->i = 1;
        }
        break;
      }
      case (STR_TYPE): {
        if (!strcmp(first->s, second->s)) {
          result->i = 1;
        }
        break;
      }
      case (SYMBOL_TYPE): {
        result->i = primitiveEq(args)->i;
        break;
      }
      case (PRIMITIVE_TYPE): {
        if (first->pf == second->pf) {
          result->i = 1;
        }
        break;
      }
      case (CLOSURE_TYPE): {
        if ((first->cc.params == second->cc.params) &&
            (first->cc.body == second->cc.body) &&
            (first->cc.frame == second->cc.frame)) {
          result->i = 1;
        }
        break;
      }
      case (CONS_TYPE): {
        if (length(first) == length(second)) {
          result->i = 1;
          // compare lists at element level
          while (first->type != NULL_TYPE) {
            Value *argsList = makeNull();
            argsList = cons(car(first), argsList);
            argsList = cons(car(second), argsList);
            if (primitiveEqual(argsList)->i == 1) {
              first = cdr(first);
              second = cdr(second);
            } else {
              break;
            }
          }
        }
        break;
      }
      default: {
        result->i = 1;
        break;
      }
      }
    }
  }
  return result;
}

/*
 * returns a binding. Output is
 * frame->bindings looks like
 * ( (a b) (c d) )
 * (<some symbol> <some value>)
 * as a linked list
 */
Value *lookUpSymbol(Value *expr, Frame *frame) {
  if (frame->bindings->type == NULL_TYPE) {
    return NULL;
  }

  Value *fBindings = frame->bindings;
  while (fBindings->type != NULL_TYPE) {
    if (!strcmp(car(car(fBindings))->s, expr->s)) {
      return car(fBindings);
    } else {
      fBindings = cdr(fBindings);
    }
  }

  if (frame->parent) {
    return lookUpSymbol(expr, frame->parent);
  } else {
    return NULL;
  }
}

/*
 * handlles the following
 * (if test consequent alternate)
 * (if test consequent)
 */
Value *evalIf(Value *operands, Frame *frame) {
  Value *result = makeNull();
  if (length(operands) < 2 || length(operands) > 3) {
    printf("EVALUATION ERROR: invalid number of arguments for if. Expected 2 "
           "or 3\n");
  }
  Value *test = eval(car(operands), frame);
  if (test->type == BOOL_TYPE) {
    if (test->i == 1) {
      result = eval(car(cdr(operands)), frame);
    } else {
      result = eval(car(cdr(cdr(operands))), frame);
    }
  } else {
    result = eval(car(cdr(operands)), frame);
  }
  return result;
}

Value *makeBinding(Value *symbol, Value *val) {
  Value *bindingToAdd = makeNull();
  bindingToAdd = cons(val, bindingToAdd);
  bindingToAdd = cons(symbol, bindingToAdd);
  return bindingToAdd;
}

/*
 * (let ((var expr) (var expr) ...) body)
*/
Value *evalLet(Value *operands, Frame *frame) {
  if (length(operands) < 2) {
    printf("EVALUATION ERROR: let needs a list of bindings and a body\n");
    evaluationError();
  } else if (car(operands)->type != CONS_TYPE) {
    printf("EVALUATION ERROR: the first argument for let should be a list of "
           "bindings\n");
    evaluationError();
  }

  // create frame pointing to parent
  // bind variables
  // evaluate body in the created frame
  Frame *newFrame = talloc(sizeof(Frame));
  newFrame->parent = frame;
  // newBindings = ((s1 v1) (s2 v2) (s3 v3) ... )
  Value *newBindings = makeNull();
  Value *letDefs = car(operands);
  Value *curLetDef = car(letDefs);

  while (letDefs->type != NULL_TYPE) {
    curLetDef = car(letDefs);
    newBindings =
        cons(makeBinding(car(curLetDef), eval(car(cdr(curLetDef)), frame)),
             newBindings);
    letDefs = cdr(letDefs);
  }
  newFrame->bindings = newBindings;
  Value *result;
  Value *body = cdr(operands);
  while (body->type != NULL_TYPE) {
    // this way, the returned result will be the last evaluated value,
    // consistent with how DrRacket does it
    result = eval(car(body), newFrame);
    body = cdr(body);
  }

  return result;
}

/*
 * let* is same was let except it evaluates all bindings left to right
 */
Value *evalLetStar(Value *operands, Frame *frame) {
  if (length(operands) < 2) {
    printf("EVALUATION ERROR: let* needs a list of bindings and a body\n");
    evaluationError();
  } else if (car(operands)->type != CONS_TYPE) {
    printf("EVALUATION ERROR: the first argument for let* should be a list of "
           "bindings\n");
    evaluationError();
  }

  Frame *newFrame = talloc(sizeof(Frame));
  newFrame->parent = frame;
  Value *newBindings = makeNull();
  Value *letDefs = car(operands);
  Value *curLetDef = car(letDefs);
  curLetDef = car(letDefs);
  newBindings =
      cons(makeBinding(car(curLetDef), eval(car(cdr(curLetDef)), frame)),
           newBindings);
  letDefs = cdr(letDefs);
  newFrame->bindings = newBindings;

  if (letDefs->type == NULL_TYPE) {
    Value *result;
    Value *body = cdr(operands);
    while (body->type != NULL_TYPE) {
      result = eval(car(body), newFrame);
      body = cdr(body);
    }
    return result;
  }

  Value *temp = makeNull();
  Value *newOperands = cons(cons(car(letDefs), temp), cdr(operands));
  Value *result = evalLetStar(newOperands, newFrame);

  return result;
}

Value *evalLetRec(Value *operands, Frame *frame) {
  if (length(operands) < 2) {
    printf("EVALUATION ERROR: letrec needs a list of bindings and a body\n");
    evaluationError();
  } else if (car(operands)->type != CONS_TYPE) {
    printf("EVALUATION ERROR: the first argument for letrec should be a list "
           "of bindings\n");
    evaluationError();
  }

  Frame *newFrame = talloc(sizeof(Frame));
  newFrame->parent = frame;
  // newBindings = ((s1 v1) (s2 v2) (s3 v3) ... )

  Value *letDefs = car(operands);
  Value *curLetDef = car(letDefs);
  Value *emptyBindings = makeNull();

  // make empty bindings
  while (letDefs->type != NULL_TYPE) {
    curLetDef = car(letDefs);
    // binding with null as value
    emptyBindings =
        cons(makeBinding(car(curLetDef), makeNull()), emptyBindings);
    letDefs = cdr(letDefs);
  }

  newFrame->bindings = emptyBindings;
  letDefs = car(operands);

  Value *curSym;

  // evaluate bodies and re-set values for each binding
  while (letDefs->type != NULL_TYPE) {
    curLetDef = car(letDefs);
    curSym = lookUpSymbol(car(curLetDef), newFrame);

    if (curSym == NULL) {
      printf("EVALUATION ERROR: letrec failed\n");
      evaluationError();
    } else {
      curSym->c.cdr->c.car = eval(car(cdr(curLetDef)), newFrame);
      letDefs = cdr(letDefs);
    }
  }

  // Value *result = eval(car(cdr(operands)), newFrame);

  Value *result;
  Value *body = cdr(operands);
  while (body->type != NULL_TYPE) {
    result = eval(car(body), newFrame);
    body = cdr(body);
  }
  return result;
}

Value *evalQuote(Value *operands, Frame *frame) {
  if (length(operands) == 0) {
    printf("EVALUATION ERROR: quote needs an argument\n");
    evaluationError();
  }
  return car(operands);
}

/*
 * (define var expr)
 */
Value *evalDefine(Value *operands, Frame *frame) {
  if (length(operands) != 2) {
    printf("EVALUATION ERROR: define needs a variable and a body\n");
  }

  Value *lookedUp = lookUpSymbol(car(operands), frame);
  if (!lookedUp) { // symbol isn't already bound in the frame
    Value *newBindingEntry =
        makeBinding(car(operands), eval(car(cdr(operands)), frame));
    frame->bindings = cons(newBindingEntry, frame->bindings);
  } else {
    lookedUp->c.cdr = eval(car(cdr(operands)), frame);
  }
  Value *defineReturn = makeNull();
  defineReturn->type = VOID_TYPE;
  return defineReturn;
}

/*
 * (lambda (x1 x2 ...) body)
 * (lambda x body)
 * makes a closure struct
 */
Value *evalLambda(Value *operands, Frame *frame) {
  // TODO: (define f (lambda (x) (set! x 13) x))
  //(f 2) => 13
  // implement evaluation of all things in the body
  // current: only the first thing is in the body
  if (length(operands) < 2) {
    printf("EVALUATION ERROR: lambda needs a list of formal parameters and a "
           "body\n");
    evaluationError();
  }
  Value *newClosure = makeNull();
  newClosure->type = CLOSURE_TYPE;
  newClosure->cc.params = car(operands);
  // newClosure->cc.body = car(cdr(operands));
  newClosure->cc.body = cdr(operands);
  newClosure->cc.frame = frame;
  return newClosure;
}

/*
 * (and expr1 expr2 ... exprn)
 * evaluate expressions left to right
 * if at any point, an expression evaluates to false, return false
 * if all expressions evaluate to not-false, return the last value
 */
Value *evalAnd(Value *operands, Frame *frame) {
  if (operands->type == NULL_TYPE) {
    printf("EVALUATION ERROR: and takes at least one argument\n");
    evaluationError();
  }
  Value *evaluated = eval(car(operands), frame);
  if (evaluated->type == BOOL_TYPE) {
    if (evaluated->i == 0) {
      // if value is false, return false
      return evaluated;
    } else {
      if (cdr(operands)->type == NULL_TYPE) {
        // if the last expression evaluated to not-false, return that value
        return evaluated;
      } else {
        // if evaluated to not-false and there are more expressions, evaluate
        // them
        return evalAnd(cdr(operands), frame);
      }
    }
  } else if (cdr(operands)->type == NULL_TYPE) {
    // if not a bool type, then value is true. If there are more expressions,
    // return their evaluation
    return evaluated;
  } else {
    // if value is not a bool type and there are no more expressions, return
    // that value
    return evalAnd(cdr(operands), frame);
  }
}

/*
 * (or expr1 expr2 ... exprn)
 * or evaluates arguments left to right
 * if an expression evaluates to true, return true
 * if an all expressions evaluate to false, return the
 * evaluated value of the last expression
 */
Value *evalOr(Value *operands, Frame *frame) {
  if (operands->type == NULL_TYPE) {
    printf("EVALUATION ERROR: or takes at least one argument\n");
    evaluationError();
  }
  Value *evaluated = eval(car(operands), frame);
  // only way evaluated can be false is if it is a bool type
  if (evaluated->type == BOOL_TYPE) {
    if (evaluated->i == 0) {
      if (cdr(operands)->type == NULL_TYPE) {
        // if the last expression evaluated to false, return false
        return evaluated;
      } else {
        // if the current value is false, and there are more expressions to
        //  evaluate, do so
        return evalOr(cdr(operands), frame);
      }
    } else {
      // if true, return true
      return evaluated;
    }
  } else {
    // if not a bool, then the value is true, so return that value
    return evaluated;
  }
}

Value *evalSetBang(Value *operands, Frame *frame) {
  if (length(operands) < 2) {
    printf("EVALUATION ERROR: set! takes a variable and a value\n");
    evaluationError();
  }
  Value *lookedUpSymbol = lookUpSymbol(car(operands), frame);
  if (!lookedUpSymbol) {
    printf("EVALUATION ERROR: symbol not predefined for set!");
    evaluationError();
  } else {
    lookedUpSymbol->c.cdr->c.car = eval(car(cdr(operands)), frame);
  }

  Value *setBangReturn = makeNull();
  setBangReturn->type = VOID_TYPE;
  return setBangReturn;
}

/* (cond cond-clause ...)
 * cond-clause = [test-expr then-body ...] or
 *              [else then-body...] or
 *              [test-expr => proc-expr] or
 *              [test-expr]
 */
Value *evalCond(Value *operands, Frame *frame) {
  // operands = ((test body) (test body) or (test) or (else body))
  while (operands->type != NULL_TYPE) {
    if (car(car(operands))->type == SYMBOL_TYPE) {
      if (!strcmp(car(car(operands))->s, "else")) {
        return eval(car(cdr(car(operands))), frame);
      }
    }
    Value *test = eval(car(car(operands)), frame);
    if (test->type == BOOL_TYPE) {
      if (test->i == 0) {
        operands = cdr(operands);
      } else if (test->i == 1) {
        if (cdr(car(operands))->type == CONS_TYPE) {
          return eval(car(cdr(car(operands))), frame);
        } else {
          return test;
        }
      }
    } else {
      if (cdr(car(operands))->type == CONS_TYPE) {
        return eval(car(cdr(car(operands))), frame);
      } else {
        return test;
      }
    }
  }
  return makeNull();
}

/*
 * for functions of the type (e1 e2 e3 ...) where e1 is not a special form
 * this also handles the distinction between
 * (lambda (x1 x2 ...) b) and (lambda x body)
 * in their closure forms
 */
Value *apply(Value *function, Value *args) {
  if (function->type == PRIMITIVE_TYPE) {
    return function->pf(args);
  } else {
    Frame *newFrame = talloc(sizeof(Frame));
    newFrame->parent = function->cc.frame;
    Value *formalParams = function->cc.params;
    if (formalParams->type == CONS_TYPE) {
      // fixed num of args
      Value *newBindings = makeNull();
      while (formalParams->type != NULL_TYPE) {
        newBindings =
            cons(makeBinding(car(formalParams), car(args)), newBindings);
        formalParams = cdr(formalParams);
        args = cdr(args);
      }
      newFrame->bindings = newBindings;
    } else {
      // any num of args
      Value *argsList = makeNull();
      // (f 1 2 3 4) --> making (1 2 3 4)
      while (args->type != NULL_TYPE) {
        argsList = cons(car(args), argsList);
        args = cdr(args);
      }
      argsList = reverse(argsList);
      Value *newBindings =
          cons(makeBinding(formalParams, argsList), makeNull());
      newFrame->bindings = newBindings;
    }
    Value *body = function->cc.body;
    Value *result;
    while (body->type != NULL_TYPE) {
      result = eval(car(body), newFrame);
      body = cdr(body);
    }
    return result;
    // return eval(function->cc.body, newFrame);
  }
}

/* function used by printResult*/
bool isImproperList(Value *list) {
  bool done = false;
  bool returnVal = false;
  while (!done) {
    if (cdr(list)->type == CONS_TYPE) {
      list = cdr(list);
    } else if (cdr(list)->type == NULL_TYPE) {
      done = true;
    } else {
      // here, the cdr is not a NULL_TYPE, meaning that the list is improper
      returnVal = true;
      done = true;
    }
  }
  return returnVal;
}

/* helper function for printResult*/
void spacedPrint(char *thingToPrint, bool first, bool last) {
  if (first && last) {
    printf("%s", thingToPrint);
  } else if (!first && !last) {
    printf("%s ", thingToPrint);
  } else if (first && !last) {
    printf("%s ", thingToPrint);
  } else if (!first && last) {
    printf("%s", thingToPrint);
  }
}

/* long and convoluted function all for the sake of printing results pretty-ly*/
void printResult(Value *result, bool printNewLine, bool first, bool last,
                 bool isInnerList) {

  switch (result->type) {
  case DOUBLE_TYPE: {
    char str[sizeof(double)];
    sprintf(str, "%f", result->d);
    spacedPrint(str, first, last);
    break;
  }
  case STR_TYPE: {
    spacedPrint(result->s, first, last);
    break;
  }
  case INT_TYPE: {
    char str[sizeof(int)];
    sprintf(str, "%i", result->i);
    spacedPrint(str, first, last);
    break;
  }
  case BOOL_TYPE: {
    if (result->i) {
      printf("#t");
    } else {
      printf("#f");
    }
    break;
  }
  case CONS_TYPE: {
    int counter = 1;
    int len = length(result);
    printf("(");
    if (isImproperList(result)) {
      bool done = false;
      while (!done) {
        if (result->type == CONS_TYPE) {
          if (counter == 1) {
            if (len == 1) {
              printResult(car(result), false, true, true, true);
            } else {
              printResult(car(result), false, true, false, true);
            }
          } else {
            if (counter == len) {
              printResult(car(result), false, false, true, true);
            } else {
              printResult(car(result), false, false, false, true);
            }
          }
          result = cdr(result);
          counter++;
        } else {
          printf(". ");
          printResult(result, false, false, true, true);
          done = true;
        }
      }
    } else {
      while (result->type != NULL_TYPE) {
        if (result->type == CONS_TYPE) {
          if (counter == 1) {
            if (len == 1) {
              printResult(car(result), false, true, true, true);
            } else {
              printResult(car(result), false, true, false, true);
            }
          } else {
            if (counter == len) {
              printResult(car(result), false, false, true, true);
            } else {
              printResult(car(result), false, false, false, true);
            }
          }
          result = cdr(result);
          counter++;
        }
      }
    }
    if (isInnerList) {
      printf(") ");
    } else {
      printf(")");
    }
    break;
  }
  case CLOSURE_TYPE: {
    printf("<procedure at %p>", result);
    break;
  }
  default: {
    printNewLine = false;
    break;
  }
  }
  if (printNewLine) {
    printf("\n");
  }
}

/* handle a list of S-expressions (i.e., a Scheme program):
    call eval on each S-expression in the top-level (global) environment,
    and print each result in turn. */
void interpret(Value *tree) {
  Frame *globalFrame = talloc(sizeof(Frame));
  globalFrame->bindings = makeNull();
  globalFrame->parent = NULL;
  bind("+", primitiveAdd, globalFrame);
  bind("-", primitiveMinus, globalFrame);
  bind("*", primitiveMult, globalFrame);
  bind("/", primitiveDivide, globalFrame);
  bind("null?", primitiveNull, globalFrame);
  bind("car", primitiveCar, globalFrame);
  bind("cdr", primitiveCdr, globalFrame);
  bind("cons", primitiveCons, globalFrame);
  bind("zero?", primitiveZero, globalFrame);
  bind("not", primitiveNot, globalFrame);
  bind("<=", primitiveLeq, globalFrame);
  bind("eq?", primitiveEq, globalFrame);
  bind("equal?", primitiveEqual, globalFrame);

  while (tree->type != NULL_TYPE) {
    printResult(eval(car(tree), globalFrame), true, true, false, false);
    tree = cdr(tree);
  }
}

/* takes a parse tree of a Scheme expression (as in the output of your parser)
     and an environment frame in which to evaluate that expression, and
   returns
     a pointer to Value representing the value */
Value *eval(Value *expr, Frame *frame) {
  Value *result = makeNull();
  switch (expr->type) {
  // TYPES THAT SHOULD NOT APPEAR IN EVALUATION
  case VOID_TYPE: {
    printf("EVALUATION ERROR: VOID_TYPE in evaluate\n");
    evaluationError();
    break;
  }
  case PTR_TYPE: {
    printf("EVALUATION ERROR: PTR_TYPE in evaluate\n");
    evaluationError();
    break;
  }
  case NULL_TYPE: {
    printf("EVALUATION ERROR: NULL_TYPE in evaluate\n");
    evaluationError();
    break;
  }
  case OPEN_TYPE: {
    printf("EVALUATION ERROR: OPEN_TYPE in evaluate\n");
    evaluationError();
    break;
  }
  case CLOSE_TYPE: {
    printf("EVALUATION ERROR: CLOSE_TYPE in evaluate\n");
    evaluationError();
    break;
  }
  case CLOSURE_TYPE: {
    printf("EVALUATION ERROR: CLOSURE_TYPE in evaluate\n");
    evaluationError();
    break;
  }
  // PRIMITIVES
  case DOUBLE_TYPE: {
    result = expr;
    break;
  }
  case STR_TYPE: {
    result = expr;
    break;
  }
  case INT_TYPE: {
    result = expr;
    break;
  }
  case BOOL_TYPE: {
    result = expr;
    break;
  }
  case PRIMITIVE_TYPE: {
    result = lookUpSymbol(expr, frame);
    if (!result) {
      printf("EVALUTATION ERROR: Primitive type not found\n");
      evaluationError();
    }
    break;
  }
  // SYMBOLS
  case SYMBOL_TYPE: {
    result = car(cdr(lookUpSymbol(expr, frame)));
    if (!result) {
      printf("EVALUATIONS ERROR: Symbol not found\n");
      evaluationError();
    }
    break;
  }
  // SUBEXPRESSIONS
  case CONS_TYPE: {
    Value *operator= car(expr);
    Value *operands = cdr(expr);
    if (!strcmp(operator->s, "if")) {
      result = evalIf(operands, frame);
    } else if (!strcmp(operator->s, "let")) {
      result = evalLet(operands, frame);
    } else if (!strcmp(operator->s, "quote")) {
      result = evalQuote(operands, frame);
    } else if (!strcmp(operator->s, "define")) {
      result = evalDefine(operands, frame);
    } else if (!strcmp(operator->s, "lambda")) {
      result = evalLambda(operands, frame);
    } else if (!strcmp(operator->s, "let*")) {
      result = evalLetStar(operands, frame);
    } else if (!strcmp(operator->s, "letrec")) {
      result = evalLetRec(operands, frame);
    } else if (!strcmp(operator->s, "and")) {
      result = evalAnd(operands, frame);
    } else if (!strcmp(operator->s, "or")) {
      result = evalOr(operands, frame);
    } else if (!strcmp(operator->s, "set!")) {
      result = evalSetBang(operands, frame);
    } else if (!strcmp(operator->s, "cond")) {
      result = evalCond(operands, frame);
    } else {
      Value *evalList = makeNull();
      while (expr->type != NULL_TYPE) {
        evalList = cons(eval(car(expr), frame), evalList);
        expr = cdr(expr);
      }
      evalList = reverse(evalList);
      result = apply(car(evalList), cdr(evalList));
    }
    break;
  }
  }
  return result;
}
