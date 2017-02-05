#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "value.h"
#include "linkedlist.h"
#include "talloc.h"

// return a linked list consisting of all tokens found in stdin (the input
// stream in C), which your tokenize function should read in its entirety
Value *tokenize();
Value *tokenizeFile();

// take a linked list of tokens as input, and display those tokens,
// one per line, with each token’s type.
void displayTokens(Value *list);
