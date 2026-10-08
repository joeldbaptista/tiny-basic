# Tiny Basic: compiler (bcom), assembler (basm) and virtual machine (bvm).

CC = cc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -g
LDFLAGS =

BIN = bcom basm bvm
COMSRC = main.c stmt.c expr.c util.c
COMOBJ = $(COMSRC:.c=.o)
OBJ = $(COMOBJ) basm.o bvm.o

EXBAS = $(wildcard examples/*.bas)
EXASM = $(EXBAS:.bas=.asm)
EXBC = $(EXBAS:.bas=.bc)

all: $(BIN)

bcom: $(COMOBJ)
	$(CC) $(LDFLAGS) -o $@ $(COMOBJ)

basm: basm.o
	$(CC) $(LDFLAGS) -o $@ basm.o

bvm: bvm.o
	$(CC) $(LDFLAGS) -o $@ bvm.o

$(COMOBJ): basic.h
basm.o bvm.o: vm.h

.SUFFIXES: .c .o .bas .asm .bc

.c.o:
	$(CC) $(CFLAGS) -c $<

.bas.asm:
	./bcom < $< > $@

.asm.bc:
	./basm < $< > $@

$(EXASM): bcom
$(EXBC): basm

test: bvm $(EXBC)
	for f in $(EXBC); do \
		echo "== $$f"; \
		./bvm $$f || exit 1; \
	done

clean:
	rm -f $(BIN) $(OBJ) $(EXASM) $(EXBC)

.DELETE_ON_ERROR:
.PRECIOUS: $(EXASM)
.PHONY: all test clean
