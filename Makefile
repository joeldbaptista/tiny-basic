# bcom: compiles Tiny Basic to stack machine assembly.

CC = cc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -g
LDFLAGS =

BIN = bcom
SRC = main.c stmt.c expr.c util.c
OBJ = $(SRC:.c=.o)

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ)

$(OBJ): basic.h

.c.o:
	$(CC) $(CFLAGS) -c $<

test: $(BIN)
	for f in examples/*.bas; do \
		echo "== $$f"; \
		./$(BIN) < $$f || exit 1; \
	done

clean:
	rm -f $(BIN) $(OBJ)

.PHONY: all test clean
