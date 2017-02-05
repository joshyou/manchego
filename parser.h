/*
 * parser.h
 * CS 251, Carleton College, Fall 2016
 */

#ifndef _PARSER
#define _PARSER

#include "value.h"

/* take a linked list of tokens from a Scheme program 
and it  return a pointer to a parse tree representing that program. */
Value *parse(Value *tokens);

/*  display a parse tree to the screen, using parentheses to denote tree structure. */
void printTree(Value *tree);

#endif