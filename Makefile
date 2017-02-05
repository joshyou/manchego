CC = clang
CFLAGS = -g

LSRCS = linkedlist.c main_linkedlist.c talloc.c
TSRCS = main_tokenize.c tokenizer.c talloc.c linkedlist.c
PSRCS = main_parse.c parser.c talloc.c linkedlist.c tokenizer.c
ISRCS = main.c interpreter.c talloc.c tokenizer.c parser.c linkedlist.c


LHDRS = linkedlist.h value.h talloc.h
THDRS = tokenizer.h value.h talloc.h linkedlist.h
PHDRS = parser.h value.h talloc.h linkedlist.h tokenizer.h
IHDRS = interpreter.h talloc.h tokenizer.h parser.h value.h linkedlist.h

LOBJS = $(LSRCS:.c=.o)
TOBJS = $(TSRCS:.c=.o)
POBJS = $(PSRCS:.c=.o)
IOBJS = $(ISRCS:.c=.o)

linkedlist: $(LOBJS)
	$(CC)  $(CFLAGS) $^  -o $@

tokenizer: $(TOBJS)
	$(CC) $(CFLAGS) $^ -o $@


parser: $(POBJS)
	$(CC) $(CFLAGS) $^ -o $@

interpreter: $(IOBJS)
	$(CC) $(CFLAGS) $^ -o $@

memtest:
	valgrind --leak-check=full --show-leak-kinds=all ./parser

%.o : %.c $(HDRS)
	$(CC)  $(CFLAGS) -c $<  -o $@

clean:
	rm *.o

lclean:
	rm *.o
	rm linkedlist

tclean:
	rm *.o
	rm tokenizer

pclean:
	rm *.o
	rm parser

iclean:
	rm *.o
	rm interpreter
