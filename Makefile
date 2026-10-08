FLAGS=-Wimplicit-int

bcom:	main.o stmt.o expr.o util.o
	cc $(FLAGS) main.o stmt.o expr.o util.o -o bcom

main.o:	main.c
	cc -c main.c

stmt.o:	stmt.c
	cc -c stmt.c

expr.o:	expr.c
	cc -c expr.c

util.o:	util.c
	cc -c util.c
