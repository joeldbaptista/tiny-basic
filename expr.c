/*
::::::::::::::::::::
expr.c
::::::::::::::::::::
*/

#include <stdio.h>

#define MAXLINE 256

extern char *bufptr, inbuf[MAXLINE];

expr()
{
	exp1();

	for (;;)
		if (check("=")) {
			exp1(); printf("tsteq\n"); }
		else if (check("<>")) {
			exp1(); printf("tstne\n"); }
		else if (check("<=")) {
			exp1(); printf("tstle\n"); }
		else if (check(">=")) {
			exp1(); printf("tstge\n"); }
		else if (check("<")) {
			exp1(); printf("tstlt\n"); }
		else if (check(">")) {
			exp1(); printf("tstgt\n"); }
		else
			return;
}

exp1()
{
	exp2();

	for (;;)
		if (check("+")) {
			exp2(); printf("addn\n"); }
		else if (check("-")) {
			exp2(); printf("subn\n"); }
		else
			return;
}

exp2()
{
	exp3();

	for (;;)
		if (check("*")) {
			exp3(); printf("mult\n"); }
		else if (check("/")) {
			exp3(); printf("divn\n"); }
		else
			return;
}

/*
::::::::::::::::::::::
expr.c - cont...
::::::::::::::::::::::
*/

exp3()
{
	int s;

	s = (check("-")) ? -1 : 1;
	if (check("("))
	{
		expr();
		scan(")");
	}
	else
		if (*bufptr >= '0' && *bufptr <= '9')
			getnum();
		else
			getvar();
	if (s == -1)
		printf("minus\n");
}

getnum()
{
	printf("getnm ");

	while (*bufptr >= '0' && *bufptr <= '9')
		printf("%c",*bufptr++);

	printf("\n");
}

getvar()
{
	if (*bufptr < 'a' || *bufptr > 'z') {
		printf("Syntax error\n");
		exit(1);
	}

	printf("fetch ");

	while (*bufptr >= 'a' && *bufptr <= 'z')
		printf("%c",*bufptr++);

	printf("\n");
}
