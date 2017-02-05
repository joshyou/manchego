#include <stdio.h>
#include "value.h"
#include "parser.h"
#include "linkedlist.h"
#include "tokenizer.h"
#include "talloc.h"


int main() {
	Value *list = tokenize();
	Value *tree = parse(list);
	printTree(tree);
	tfree();
	return 0;
}