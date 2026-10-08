/*
::::::::::::::::::::
stmt.c
::::::::::::::::::::
*/

#include <stdio.h>

#define MAXLINE 256

extern char *bufptr, inbuf[MAXLINE];

doprint()
{
	expr();
	printf("print\n");
}

dolet()
{
	char *varptr;

	varptr = bufptr;
	while (*bufptr++ != '=');
	*(bufptr-1) = '\0';
	expr();
	printf("store %s\n",varptr);
}

doif()
{
	expr();
	scan("then");
	printf("jump ");

	while (*bufptr >= '0' && *bufptr <= '9')
		printf("%c",*bufptr++);

	printf("\n");
}

dostop()
{
	printf("stop\n");
}

dogoto()
{
	printf("goto ");

	while (*bufptr >= '0' && *bufptr <= '9')
		printf("%c",*bufptr++);

	printf("\n");
}
