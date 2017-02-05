#include "interpreter.h"
#include "parser.h"
#include "talloc.h"
#include "tokenizer.h"
#include "value.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
  printf("Please enter your scheme code.\n If you want to quit, type in ctrl + "
         "d\n");
  char *input;

  while (fgets(input, 1000, stdin) != NULL) {
    if (isatty(0) == 0) {
      Value *list = tokenizeFile();
      printf("read file\n");
      Value *tree = parse(list);
      interpret(tree);
      tfree();
      return 0;
    }
    // printf("you entered %s\n", input);
    FILE *fp;
    fp = fopen("tmpFile.txt", "w");

    fputs(input, fp); //, " ", input);
    fclose(fp);
    // printf("done writing to file\n");

    Value *list = tokenize();
    // printf("done tokenizing\n");
    Value *tree = parse(list);
    interpret(tree);
  }
  tfree();
  return 0;
}
