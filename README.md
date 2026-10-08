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
any = any character except a newline

number = digit+
variable = letter+
label = letter+

addop = + | -
mulop = * | /
relop = < | <= | > | >= | == | !=

expr = exp1 (relop exp1)?
exp1 = exp2 (addop exp2)*
exp2 = exp3 (mulop exp3)*
exp3 = "-"? ("(" expr ")" | number | variable)

labelphrase = label ":"
letphrase = variable "=" expr
ifphrase = "if" expr "then" gotophrase
gotophrase = "goto" label
stopphrase = "stop"
printphrase = "print" expr
statement = letphrase | ifphrase | gotophrase | stopphrase | printphrase
comment = "#" any*
line = (statement | labelphrase | comment)?
program = (line)+
```

In `relop` I decided to use different symbols for equality and not-equality from
what the book suggested.

The keywords `if`, `then`, `goto`, `stop` and `print` are reserved, so they
cannot be variable or label names. But a name can start with a keyword, so
`printer` is a valid variable. Variables and labels have separate name spaces:
a variable and a label can have the same name, and they mean different things.
Spaces and tabs can separate any two tokens. The compiler ignores empty lines,
lines with only spaces and tabs, and comments. A comment is a line whose first
character other than a space or a tab is `#`.

A label marks the next statement, so a program looks like this:

```
# count down from 10 to 1
	n = 10

loop:
	print n
	n = n - 1
	if n > 0 then goto loop
	stop
```

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

label <name> - marks the next instruction as `name`
jump <name> - pops, and jumps to label `name` if the value is not 0
goto <name> - jumps to label `name`

print - pop and print the value
stop - stop execution
```

I changed the name of some instructions but they are essentially the same.
The assembly compiles to byte code file where the variables are all static. 
This means, `store x` pops the value from the stack, and saves it in the 
static slot labeled as `x`. 

## Binary layout

The assembler, `basm`, writes a byte code file with the extension `.bc`. The
file has two parts: a 12-byte header, and a code section with 5 bytes per
instruction. All multi-byte integers are little-endian, which means the least
significant byte comes first. The file stores no variable names and no labels.

The header has these fields:

| Offset | Size | Field                                   |
|--------|------|-----------------------------------------|
| 0      | 4    | magic `TBVM` (`54 42 56 4d`)            |
| 4      | 4    | number of variable slots, unsigned      |
| 8      | 4    | number of instructions, unsigned        |

Each instruction has these fields:

| Offset | Size | Field                                   |
|--------|------|-----------------------------------------|
| 0      | 1    | opcode                                  |
| 1      | 4    | operand, signed two's complement        |

The opcodes are the values of the `enum` in `vm.h`:

| Opcode | Instruction | Opcode | Instruction |
|--------|-------------|--------|-------------|
| `00`   | `getnm`     | `09`   | `tstne`     |
| `01`   | `fetch`     | `0a`   | `tstgt`     |
| `02`   | `store`     | `0b`   | `tstlt`     |
| `03`   | `add`       | `0c`   | `tstge`     |
| `04`   | `sub`       | `0d`   | `tstle`     |
| `05`   | `mul`       | `0e`   | `jump`      |
| `06`   | `div`       | `0f`   | `goto`      |
| `07`   | `neg`       | `10`   | `print`     |
| `08`   | `tsteq`     | `11`   | `stop`      |

The meaning of the operand depends on the opcode:

- `getnm`: the number to push.
- `fetch` and `store`: the index of the variable slot.
- `jump` and `goto`: the index of the target instruction. The assembler
  replaces each label name with this index, so `label` produces no bytes.
  The index equal to the number of instructions halts the program.
- every other opcode: unused, and always 0.

### Displaying the bytes

`xxd` displays the two parts of a `.bc` file. The first command prints the
header, and the second prints one instruction per line:

```
xxd -l 12 examples/countdown.bc
xxd -s 12 -c 5 examples/countdown.bc
```

`-l 12` stops after the 12 header bytes. `-s 12` skips the header, and `-c 5`
prints 5 bytes per line. The option `-c` needs its value: without it, `xxd`
takes the file name as the column count and reads standard input.

For `examples/countdown.bc` the header is:

```
00000000: 5442 564d 0100 0000 0d00 0000            TBVM........
```

So the program has 1 variable slot (`n`) and 13 instructions. These are the
first 4 instructions:

```
0000000c: 000a 0000 00  .....    getnm 10
00000011: 0200 0000 00  .....    store n (slot 0)
00000016: 0100 0000 00  .....    fetch n (slot 0)
0000001b: 1000 0000 00  .....    print
```

`xxd` groups the bytes in pairs, so `000a 0000 00` means the bytes
`00 0a 00 00 00`: opcode `00` (`getnm`) and operand `0a 00 00 00` (10).
