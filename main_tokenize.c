#include "value.h"
#include "talloc.h"
#include "tokenizer.h"

int main() {
    Value *list = tokenize();
    displayTokens(list);
    tfree();
}
