/* basic.h: shared declarations for the Tiny Basic compiler. */

#define MAXLINE 256

/* util.c */
void die(const char *msg);
int match(const char *s);
void expect(const char *s);
void emitnum(const char *op);

/* stmt.c */
void dolet(void);
void doprint(void);
void doif(void);
void dostop(void);
void dogoto(void);

/* expr.c */
void expr(void);

/* main.c */
extern char *bufptr;
