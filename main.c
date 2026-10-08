/*
::::::::::::::::::::
main.c
::::::::::::::::::::
*/

#include <stdio.h>

#define MAXLINE 256

char *bufptr, inbuf[MAXLINE];

main()
{
	while (getline())
	{
		linenum();
		if (check("let"))
			dolet();
		else if (check("print"))
			doprint();
		else if (check("if"))
			doif();
		else if (check("stop"))
			dostop();
		else if (check("goto"))
			dogoto();
		else
			dolet();
	}
	printf("\n");
}

getline()
{
	int c,i;

	bufptr = inbuf;

	for (i = 0; (c = getchar()) != '\n' && c != EOF; i++)
		inbuf[i] = c;

	inbuf[i] = '\0';

	return((i == 0) ? 0 : 1);
}

linenum()
{
	printf("label ");

	while (*bufptr >= '0' && *bufptr <= '9')
		printf("%c",*bufptr++);

	printf("\n");
}
