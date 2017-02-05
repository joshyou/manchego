#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "value.h"
#include "talloc.h"
#include "linkedlist.h"

int main() {
   /* When you're ready [i.e., for Part II], swap the "malloc"s for
      "talloc"s for val1, val2, and val3. */
   Value *val1 = malloc(sizeof(Value));
   val1->type = INT_TYPE;
   val1->i = 12;
   
   Value *val2 = malloc(sizeof(Value));
   val2->type = DOUBLE_TYPE;
   val2->d = 4.3;

   Value *val3 = malloc(sizeof(Value));
   val3->type = DOUBLE_TYPE;
   val3->d = 4.2;
   Value *val4 = malloc(sizeof(Value));
   val4->type = DOUBLE_TYPE;
   val4->d = 4.1;
   Value *val6 = malloc(sizeof(Value));
   val6->type = DOUBLE_TYPE;
   val6->d = 4.0;
   
   Value *head = makeNull();
   head = cons(val1, head);
   head = cons(val2, head);
   head = cons(val3, head);
   head = cons(val4, head); 
   head = cons(val6, head);

   display(head);
   display(reverse(head));

   printf("Length = %i\n", length(head));
   printf("Empty? %i\n", isNull(head));

   Value *outer = makeNull();
   outer = cons(val1, outer);
   outer = cons(head, outer);
   outer = cons(val2, outer);
   display(outer);

   /* And, when you're ready [i.e., for Part II], remove the following
      exit() call. */
   // exit(0);
   
   tfree();
   printf("I can see this.\n");
   Value *val5 = talloc(sizeof(Value));
   texit(1);
   printf("I should never see this.\n");
}
