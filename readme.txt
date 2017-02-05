Scheme Interpreter

At its current state, it is not able to correctly perform REPL.
The main problem with this is that we are starting from scratch at every input, meaning we lose all definitions and bindings with every line entered at the command line.
From our testing, we can reasonably conclude that all of the special forms and primitives work as specified in the scheme documentation.
There may be some edge cases that we may have overlooked.


A special garbage collector has not been implemented because we believe that it will require large scale code and logic restructuring
Currently, garbage collection done at the end of the program

Implemented Special forms
 - if
 - let
 - quote
 - define
 - lambda
 - let*
 - letrec
 - and
 - or
 - set!
 - cond

Built-in Functions
 - +
 - -
 - *
 - /
 - null?
 - car
 - cdr
 - cons
 - zero?
 - not
 - <=
 - eq?
 - equal?
