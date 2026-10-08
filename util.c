/*
::::::::::::::::::::
util.c
::::::::::::::::::::
*/

#include <stdio.h>

#define MAXLINE 256

extern char *bufptr, inbuf[MAXLINE];

check(ptr)
char *ptr;
{
	char *tptr;

	while (*bufptr == ' ')
		bufptr++;

	tptr = bufptr;

	while (*ptr != '\0' && *ptr == *tptr)
	{
		ptr++;
		tptr++;
	}

	if (*ptr != '\0')
		return(0);
	else
	{
		bufptr = tptr;
		while (*bufptr == ' ')
			bufptr++;
		return(1);
	}
}

scan(ptr)
char *ptr;
{
	if (check(ptr))
		return(1);

	printf("Syntax error\n");
	exit(1);
}
