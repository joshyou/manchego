#include "linkedlist.h"
#include "talloc.h"
#include "value.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void displayTokens(Value *list);

// helper function for tokenizing symbols
Value *findSymbol(char *buffer, int tokenlen, int stringIdx, int which_type) {
  Value *token = makeNull();
  char *temp;
  if (which_type) {
    token->type = SYMBOL_TYPE;
    temp = talloc(sizeof(char) * (tokenlen + 1));
    strncpy(temp, &buffer[stringIdx - tokenlen], tokenlen);
    temp[tokenlen] = '\0';
    // printf("findSymbol: temp after strncpy: %s\n", temp);
    token->s = temp;
  } else {
    token->type = STR_TYPE;
    temp = talloc(sizeof(char) * (tokenlen));
    strncpy(temp, &buffer[stringIdx - tokenlen], tokenlen);
    char *newStr = talloc(sizeof(char) * (tokenlen));
    int i = 0;
    int j = 0;
    while (i < tokenlen) {
      // printf("%c\n", temp[i]);
      if (temp[i] != 7) {
        newStr[j] = temp[i];
        j++;
      }
      i++;
    }
    newStr[j] = '\0';
    // printf("a");
    // printf("temp in symbols: %s\n", newStr);
    token->s = newStr;
    // printf("%c\n", token->s);
  }
  // temp[tokenlen] = NULL;

  return token;
}

// return a linked list list = consisting of all tokens found in stdin (the
// input
// stream in C), which your tokenize function should read in its entirety
Value *tokenize() {
  //printf("%s\n", filename);
  Value *list = makeNull();
  // list->type = CONS_TYPE;
  char *buffer = NULL;
  int read;
  // unsigned int len;
  size_t len;
  //printf("%s\n", filename);
  FILE *input = fopen("tmpFile.txt", "r");
  read = getline(&buffer, &len, input);
  // printf("298347523489750234895702348795=====%i", read);
  int stringIdx;
  int tokenlen;
  bool decPoint;

  // until there are no more lines to read, read lines
  //  the read lines are stored in buffer
  while (-1 != read) {
    //printf("in the while loop\n");
    // printf("len: %i\n", len);
    stringIdx = 0;
    // iterate through string and tokenize
    while ('\0' != buffer[stringIdx]) {
      //printf("in this while loop\n");
      tokenlen = 0;
      // open paren
      if (buffer[stringIdx] == '(') {
        Value *token = makeNull();
        token->type = OPEN_TYPE;
        token->s = "(";
        // list = cons(list,token);
        list = cons(token, list);
        // display(list);
        stringIdx++;
        // printf("%s\n", token->s);
        // close paren
      } else if (buffer[stringIdx] == ')') {
        Value *token = makeNull();
        token->type = CLOSE_TYPE;
        token->s = ")";
        // list = cons(list,token);
        list = cons(token, list);
        stringIdx++;
        // printf("%s\n", token->s);
        // integers and doubles with support for optional +/- prefixes
        //  245.32.3455 is an error. program texits.
        //  also handles + and - as special symbols
      } else if (strchr("+-1234567890", buffer[stringIdx]) != NULL) {
        tokenlen = 0;
        tokenlen++;
        stringIdx++;
        decPoint = false;
        Value *token = makeNull();

        // check for just + or - sign, which are symbols
        int prev = buffer[stringIdx - 1];
        int q = buffer[stringIdx];
        bool is_symbol = false;
        if (prev == 45) {
          if (q == 40 || q == 41 || q == 32 || q == 10) {
            // token = makeNull();
            token->type = SYMBOL_TYPE;
            token->s = "-";
            // printf("%s\n", token->s);
            is_symbol = true;
          }
        }

        if (prev == 43) {
          if (q == 40 || q == 41 || q == 32 || q == 10) {
            // token = makeNull();
            token->type = SYMBOL_TYPE;
            token->s = "+";
            // token->s = buffer[stringIdx - 1];
            // printf("%s\n", token->s);
            is_symbol = true;
          }
        }

        // while (strchr(" )\n", buffer[stringIdx]) == NULL) {
        while (strchr("1234567890.", buffer[stringIdx]) != NULL) {
          //printf("in number while loop\n");
          if (strchr(".", buffer[stringIdx]) != NULL) {
            if (decPoint) {

              printf("ERROR: multiple decimal points\n");

              // printf("len: %i\n", len);
              texit(1);

            } else {
              decPoint = true;
            }
          }
          tokenlen++;
          stringIdx++;
          if (buffer[stringIdx] == '\0') {
            // printf("breaks");
            // stringIdx++;
            break;
          }
        }

        int v = buffer[stringIdx];

        if (v != 40 && v != 41 && v != 32 && v != 10) {
          // You can end an int with a space, newline, (, or )
          printf("ERROR: CANT END A NUMBER WITH THE CHAR: %c\n",
                 buffer[stringIdx]);
          texit(1);
        }

        if (!is_symbol) {
          // token = makeNull();
          char temp[tokenlen + 1];
          strncpy(temp, &buffer[stringIdx - tokenlen], tokenlen);
          temp[tokenlen] = '\0';
          if (decPoint) {
            token->type = DOUBLE_TYPE;
            token->d = atof(temp);
            // printf("%s\n", temp);
            // printf("%f\n", token->d);
          } else {
            token->type = INT_TYPE;
            token->i = atoi(temp);
            // printf("%s\n", temp);
            // printf("%i\n", token->i);
          }
        }
        // strncpy( token->s, &buffer[stringIdx - tokenlen], tokenlen);
        // token->s[tokenlen] = NULL;
        // list = cons(list,token);
        list = cons(token, list);
        // display(list);
        // printf("%i\n", token->i);
        // if(token->type == DOUBLE_TYPE){
        // printf("hello2\n");
        //}
        // decPoint = false;
        // booleans
      } else if (buffer[stringIdx] == '#') {
        if (buffer[stringIdx + 1] == 't') {
          Value *token = makeNull();
          token->type = BOOL_TYPE;
          token->i = 1;
          stringIdx = stringIdx + 2;
          list = cons(token, list);
        } else if (buffer[stringIdx + 1] == 'f') {
          Value *token = makeNull();
          token->type = BOOL_TYPE;
          token->i = 0;
          stringIdx = stringIdx + 2;
          list = cons(token, list);
        } else {
          printf("ERROR: INVALID BOOL");
          texit(1);
        }
        // Strings
      } else if (buffer[stringIdx] == '\"') {

        tokenlen = 1;
        stringIdx++;
        while (buffer[stringIdx] != '\"') {
          // do stuff
          if (buffer[stringIdx] == '\0') {
            printf("ERROR: UNCLOSED STRING\n");
            texit(1);
          }
          if (buffer[stringIdx] == '\\') {
            if (buffer[stringIdx + 1] == 'n') {
              buffer[stringIdx] = '\n';
              buffer[stringIdx + 1] = 7;
              stringIdx++;
              tokenlen++;
            } else if (buffer[stringIdx + 1] == 't') {
              buffer[stringIdx] = '\t';
              buffer[stringIdx + 1] = 7;
              stringIdx++;
              tokenlen++;
            } else if (buffer[stringIdx + 1] == '\\') {
              buffer[stringIdx] = '\\';
              buffer[stringIdx + 1] = 7;
              stringIdx++;
              tokenlen++;
            } else if (buffer[stringIdx + 1] == '\'') {
              buffer[stringIdx] = '\'';
              buffer[stringIdx + 1] = 7;
              stringIdx++;
              tokenlen++;
            } else if (buffer[stringIdx + 1] == '"') {
              buffer[stringIdx] = '"';
              buffer[stringIdx + 1] = 7;
              stringIdx++;
              tokenlen++;
            }
          }
          // printf("%d\n", buffer[stringIdx]);

          tokenlen++;
          stringIdx++;
        }

        stringIdx++;
        tokenlen++;
        list = cons(findSymbol(buffer, tokenlen, stringIdx, 0), list);
        // printf("that was a string, yo\n");
        // display(list);

        // symbols
      } else if (strchr("!$%%&*/"
                        ":<=>?~_^"
                        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",
                        buffer[stringIdx]) != NULL) {
        tokenlen = 0;
        tokenlen++;
        stringIdx++;
        // printf("%c\n", buffer[stringIdx]);

        while (strchr(".+-!$%%&*/"
                      ":<=>?~_^"
                      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234"
                      "567890",
                      buffer[stringIdx]) != NULL) {
          // printf("%d\n", buffer[stringIdx]);
          tokenlen++;
          stringIdx++;
        }
        /*
        Value *token = makeNull();
        token->type = SYMBOL_TYPE;
        char temp[tokenlen + 1];
        strncpy( temp, &buffer[stringIdx - tokenlen], tokenlen);
        temp[tokenlen] = NULL;
        printf("temp in symbols: %s\n", temp);
        */

        // display(list);
        // token->s = temp;
        // strncpy(token->s, &buffer[stringIdx - tokenlen], tokenlen);
        // //problem!
        // display(list);
        // token->s[tokenlen] = NULL;

        // list = cons(list,token);
        // list = cons(token,list);

        list = cons(findSymbol(buffer, tokenlen, stringIdx, 1), list);
        // printf("symbol token: %s\n", token->s);
        // display(list);
      } else if (buffer[stringIdx] == ';') {
        break;
      } else {
        stringIdx++;
      }
    }
    read = getline(&buffer, &len, input);
  }
  // this might need to go inside the outer while loop;
  // free(buffer);
  // printf("terminates?\n");
  // displayTokens(list);
  free(buffer);
  return reverse(list);
}

Value *tokenizeFile() {
  Value *list = makeNull();
  // list->type = CONS_TYPE;
  char *buffer = NULL;
  int read;
  // unsigned int len;
  size_t len;
  read = getline(&buffer, &len, stdin);
  // printf("298347523489750234895702348795=====%i", read);
  int stringIdx;
  int tokenlen;
  bool decPoint;

  // until there are no more lines to read, read lines
  //  the read lines are stored in buffer
  while (-1 != read) {
    // printf("len: %i\n", len);
    stringIdx = 0;
    // iterate through string and tokenize
    while ('\0' != buffer[stringIdx]) {
      tokenlen = 0;
      // open paren
      if (buffer[stringIdx] == '(') {
        Value *token = makeNull();
        token->type = OPEN_TYPE;
        token->s = "(";
        // list = cons(list,token);
        list = cons(token, list);
        // display(list);
        stringIdx++;
        // printf("%s\n", token->s);
        // close paren
      } else if (buffer[stringIdx] == ')') {
        Value *token = makeNull();
        token->type = CLOSE_TYPE;
        token->s = ")";
        // list = cons(list,token);
        list = cons(token, list);
        stringIdx++;
        // printf("%s\n", token->s);
        // integers and doubles with support for optional +/- prefixes
        //  245.32.3455 is an error. program texits.
        //  also handles + and - as special symbols
      } else if (strchr("+-1234567890", buffer[stringIdx]) != NULL) {
        tokenlen = 0;
        tokenlen++;
        stringIdx++;
        decPoint = false;
        Value *token = makeNull();

        // check for just + or - sign, which are symbols
        int prev = buffer[stringIdx - 1];
        int q = buffer[stringIdx];
        bool is_symbol = false;
        if (prev == 45) {
          if (q == 40 || q == 41 || q == 32 || q == 10) {
            // token = makeNull();
            token->type = SYMBOL_TYPE;
            token->s = "-";
            // printf("%s\n", token->s);
            is_symbol = true;
          }
        }

        if (prev == 43) {
          if (q == 40 || q == 41 || q == 32 || q == 10) {
            // token = makeNull();
            token->type = SYMBOL_TYPE;
            token->s = "+";
            // token->s = buffer[stringIdx - 1];
            // printf("%s\n", token->s);
            is_symbol = true;
          }
        }

        // while (strchr(" )\n", buffer[stringIdx]) == NULL) {
        while (strchr("1234567890.", buffer[stringIdx]) != NULL) {
          if (strchr(".", buffer[stringIdx]) != NULL) {
            if (decPoint) {

              printf("ERROR: multiple decimal points\n");

              // printf("len: %i\n", len);
              texit(1);

            } else {
              decPoint = true;
            }
          }
          tokenlen++;
          stringIdx++;
          if (buffer[stringIdx] == '\0') {
            // printf("breaks");
            // stringIdx++;
            break;
          }
        }

        int v = buffer[stringIdx];

        if (v != 40 && v != 41 && v != 32 && v != 10) {
          // You can end an int with a space, newline, (, or )
          printf("ERROR: CANT END A NUMBER WITH THE CHAR: %c\n",
                 buffer[stringIdx]);
          texit(1);
        }

        if (!is_symbol) {
          // token = makeNull();
          char temp[tokenlen + 1];
          strncpy(temp, &buffer[stringIdx - tokenlen], tokenlen);
          temp[tokenlen] = '\0';
          if (decPoint) {
            token->type = DOUBLE_TYPE;
            token->d = atof(temp);
            // printf("%s\n", temp);
            // printf("%f\n", token->d);
          } else {
            token->type = INT_TYPE;
            token->i = atoi(temp);
            // printf("%s\n", temp);
            // printf("%i\n", token->i);
          }
        }
        // strncpy( token->s, &buffer[stringIdx - tokenlen], tokenlen);
        // token->s[tokenlen] = NULL;
        // list = cons(list,token);
        list = cons(token, list);
        // display(list);
        // printf("%i\n", token->i);
        // if(token->type == DOUBLE_TYPE){
        // printf("hello2\n");
        //}
        // decPoint = false;
        // booleans
      } else if (buffer[stringIdx] == '#') {
        if (buffer[stringIdx + 1] == 't') {
          Value *token = makeNull();
          token->type = BOOL_TYPE;
          token->i = 1;
          stringIdx = stringIdx + 2;
          list = cons(token, list);
        } else if (buffer[stringIdx + 1] == 'f') {
          Value *token = makeNull();
          token->type = BOOL_TYPE;
          token->i = 0;
          stringIdx = stringIdx + 2;
          list = cons(token, list);
        } else {
          printf("ERROR: INVALID BOOL");
          texit(1);
        }
        // Strings
      } else if (buffer[stringIdx] == '\"') {

        tokenlen = 1;
        stringIdx++;
        while (buffer[stringIdx] != '\"') {
          // do stuff
          if (buffer[stringIdx] == '\0') {
            printf("ERROR: UNCLOSED STRING\n");
            texit(1);
          }
          if (buffer[stringIdx] == '\\') {
            if (buffer[stringIdx + 1] == 'n') {
              buffer[stringIdx] = '\n';
              buffer[stringIdx + 1] = 7;
              stringIdx++;
              tokenlen++;
            } else if (buffer[stringIdx + 1] == 't') {
              buffer[stringIdx] = '\t';
              buffer[stringIdx + 1] = 7;
              stringIdx++;
              tokenlen++;
            } else if (buffer[stringIdx + 1] == '\\') {
              buffer[stringIdx] = '\\';
              buffer[stringIdx + 1] = 7;
              stringIdx++;
              tokenlen++;
            } else if (buffer[stringIdx + 1] == '\'') {
              buffer[stringIdx] = '\'';
              buffer[stringIdx + 1] = 7;
              stringIdx++;
              tokenlen++;
            } else if (buffer[stringIdx + 1] == '"') {
              buffer[stringIdx] = '"';
              buffer[stringIdx + 1] = 7;
              stringIdx++;
              tokenlen++;
            }
          }
          // printf("%d\n", buffer[stringIdx]);

          tokenlen++;
          stringIdx++;
        }

        stringIdx++;
        tokenlen++;
        list = cons(findSymbol(buffer, tokenlen, stringIdx, 0), list);
        // printf("that was a string, yo\n");
        // display(list);

        // symbols
      } else if (strchr("!$%%&*/"
                        ":<=>?~_^"
                        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",
                        buffer[stringIdx]) != NULL) {
        tokenlen = 0;
        tokenlen++;
        stringIdx++;
        // printf("%c\n", buffer[stringIdx]);

        while (strchr(".+-!$%%&*/"
                      ":<=>?~_^"
                      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234"
                      "567890",
                      buffer[stringIdx]) != NULL) {
          // printf("%d\n", buffer[stringIdx]);
          tokenlen++;
          stringIdx++;
        }
        /*
        Value *token = makeNull();
        token->type = SYMBOL_TYPE;
        char temp[tokenlen + 1];
        strncpy( temp, &buffer[stringIdx - tokenlen], tokenlen);
        temp[tokenlen] = NULL;
        printf("temp in symbols: %s\n", temp);
        */

        // display(list);
        // token->s = temp;
        // strncpy(token->s, &buffer[stringIdx - tokenlen], tokenlen);
        // //problem!
        // display(list);
        // token->s[tokenlen] = NULL;

        // list = cons(list,token);
        // list = cons(token,list);

        list = cons(findSymbol(buffer, tokenlen, stringIdx, 1), list);
        // printf("symbol token: %s\n", token->s);
        // display(list);
      } else if (buffer[stringIdx] == ';') {
        break;
      } else {
        stringIdx++;
      }
    }
    read = getline(&buffer, &len, stdin);
  }
  // this might need to go inside the outer while loop;
  // free(buffer);
  // printf("terminates?\n");
  // displayTokens(list);
  free(buffer);
  return reverse(list);
}


// take a linked list of tokens as input, and display those tokens,
// one per line, with each token’s type.
void displayTokens(Value *list) {

  switch (list->type) {
  case PTR_TYPE:
    printf("%p:PTR\n", list->p);
    break;
  case INT_TYPE:
    printf("%d:integer\n", list->i);
    break;
  case DOUBLE_TYPE:
    printf("%f:float\n", list->d);
    break;
  case STR_TYPE:
    printf("%s:string\n", list->s);
    break;
  case OPEN_TYPE:
    printf("%s:open\n", list->s);
    break;
  case CLOSE_TYPE:
    printf("%s:close\n", list->s);
    break;
  case SYMBOL_TYPE:
    printf("%s:symbol\n", list->s);
    break;
  case BOOL_TYPE:
    printf("%i:boolean\n", list->i);
    break;
  case CONS_TYPE:
    // prints brackets when the car is itself a list
    displayTokens(list->c.car);
    displayTokens(list->c.cdr);
    // display_helper(list->c.cdr);
    break;
  default:
    break;
  }
}