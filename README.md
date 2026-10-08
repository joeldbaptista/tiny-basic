# Tiny Basic

I have been reading old computing books, and this time was 
"Unix and C: A tutorial introduction" by Philip Cornes. This book introduces
Unix, Unix CLI utilities, `sh` and C. In chapter 16, "C in Action", the book 
shows how to write a small basic interpreter, called Tiny Basic. This repo 
contains the code and my experiments with the code and concepts in that
chapter.

## Grammar

The next is the grammar for Tiny Basic:

```
digit = [0-9]
letter = [a-z]

number = digit+
variable = letter+
linenumber = number

addop = + | -
mulop = * | /
relop = < | <= | > | >= | == | !=

expr = exp1 (relop exp1)?
exp1 = exp2 (addop exp2)*
exp2 = exp3 (mulop exp3)*
exp3 = "-"? ("(" expr ")" | number | variable)

letphrase = ("let")? variable "=" expr
ifphrase = "if" expr "then" gotophrase
gotophrase = "goto" linenumber
stopphrase = "stop"
printphrase = "print" expr
statement = letphrase | ifphrase | gotophrase | stopphrase | printphrase
line = linenumber statement
program = (line)+
```

In `relop` I decided to use different symbols for equality and not-equality from
what the book suggested.

## Virtual machine

The virtual machine for Tiny Basic is a good'old stack machine. These are the 
instructions for the virtual machine's assembly code:

```
getnm <number> - pushes the value `number`
fetch <variable> - pushes the value of `variable`
store <variable> - pops from stack and saves the value in `variable`

add - pop the 1st, pop the 2nd, and adds the 2nd to the 1st
sub - pop the 1st, pop the 2nd, and subtracts the 1st from the 2nd
mul - pop the 1st, pop the 2nd, and multiplies the 2nd to the 1st
div - pop the 1st, pop the 2nd, and divides the 2nd by the 1st
neg - pop once, and multiplies -1 to it

tsteq - pop the 1st, pop the 2nd, and tests if 2nd == 1st
tstne - pop the 1st, pop the 2nd, and tests if 2nd != 1st
tstgt - pop the 1st, pop the 2nd, and tests if 2nd > 1st
tstlt - pop the 1st, pop the 2nd, and tests if 2nd < 1st
tstge - pop the 1st, pop the 2nd, and tests if 2nd >= 1st
tstle - pop the 1st, pop the 2nd, and tests if 2nd <= 1st

label <number> - marks current line as `number`
jump <number> - jumps to line labeled `number` if condition true
goto <number> - jumps to line labeled `number`

print - pop and print the value
stop - stop execution
```

I changed the name of some instructions but they are essentially the same.
The assembly compiles to byte code file where the variables are all static. 
This means, `store x` pops the value from the stack, and saves it in the 
static slot labeled as `x`. 
