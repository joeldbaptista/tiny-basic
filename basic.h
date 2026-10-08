/* basic.h: shared declarations for the Tiny Basic compiler. */

#define MAXLINE 256

/* util.c */
void skipspace(void);
void die(const char *fmt, ...);
int match(const char *s);
void expect(const char *s);
int keyword(const char *s);
void expectkw(const char *s);
int name(char **p);
void emitnum(const char *op);

/* stmt.c */
void dolabel(const char *p, int n);
void dolet(const char *p, int n);
void doprint(void);
void doif(void);
void dostop(void);
void dogoto(void);

/* expr.c */
void expr(void);

/* main.c */
extern char *bufptr;
extern int lineno;
