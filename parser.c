/*
 * parser.c
 * CS 251, Carleton College, Fall 2016
 */

#include "linkedlist.h"
#include "talloc.h"
#include "value.h"
#include <assert.h>
#include <stdio.h>

void syntaxError() { texit(1); }

void printTreeHelper(Value *tree) {
  switch (tree->type) {
  case PTR_TYPE:
    printf("%p ", tree->p);
    break;
  case INT_TYPE:
    printf("%d ", tree->i);
    break;
  case DOUBLE_TYPE:
    printf("%f ", tree->d);
    break;
  case STR_TYPE:
    printf("%s ", tree->s);
    break;
  case OPEN_TYPE:
    printf("%s ", tree->s);
    break;
  case CLOSE_TYPE:
    printf("%s ", tree->s);
    break;
  case SYMBOL_TYPE:
    printf("%s ", tree->s);
    break;
  case BOOL_TYPE:
    if (tree->i == 0) {
      printf("#f ");
    } else {
      printf("#t ");
    }
    break;
  case CONS_TYPE:
    if (tree->c.car && tree->c.cdr) {
      if (tree->c.car->type == CONS_TYPE) {
        printf("(");
        printTreeHelper(tree->c.car);
        printf(")");
        printTreeHelper(tree->c.cdr);

      } else {
        printTreeHelper(tree->c.car);
        printTreeHelper(tree->c.cdr);
      }
    }
    break;
  default:
    break;
  }
}

/*  display a parse tree to the screen, using parentheses to denote tree
 * structure. */
void printTree(Value *tree) {
  printTreeHelper(tree);
  printf("\n");
}

Value *addToParseTree(Value *tree, int *depth, Value *token) {
  if (token->type == OPEN_TYPE) {
    tree = cons(token, tree);
    *depth = *depth + 1;
  }

  else if (token->type == CLOSE_TYPE) {
    if (*depth <= 0) {
      printf("SYNTAX ERROR: Too many close parentheses\n");
      syntaxError();
    }
    // temporarily add the close paren into the tree so that we can pop it
    Value *list = makeNull();

    Value *current = makeNull();
    current = car(tree);
    while (current->type != OPEN_TYPE) {
      if ((current->type != OPEN_TYPE) && (current->type != CLOSE_TYPE)) {
        list = cons(current, list);
      }
      tree = cdr(tree);
      current = car(tree);
    }
    tree = cdr(tree);
    tree = cons(list, tree);
    *depth = *depth - 1;

  } else {
    tree = cons(token, tree);
  }
  return tree;
}

/* take a linked list of tokens from a Scheme program
and it  return a pointer to a parse tree representing that program. */
Value *parse(Value *tokens) {
  Value *tree = makeNull();
  int depth = 0;
  Value *current = tokens;
  assert(current != NULL && "Error (parse): null pointer");
  while (current->type != NULL_TYPE) {
    Value *token = car(current);
    tree = addToParseTree(tree, &depth, token);
    current = cdr(current);
  }
  if (depth != 0) {
    printf("SYNTAX ERROR: Unclosed open parentheses\n");
    syntaxError(); // error case 4
  }
  return reverse(tree);
}
