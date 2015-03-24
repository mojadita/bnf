/* $Id: lexemas.c,v 1.2 2012/08/22 13:41:14 luis Exp $
 * vim: ts=4 sw=4 nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: lun jul 16 11:43:49 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *             All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char LEXEMAS_C_RCSId[] = "\n$Id: lexemas.c,v 1.2 2012/08/22 13:41:14 luis Exp $\n";

void string_dequote(char *si, char *so)
{
    char c = *si++; /* first char is quoting */

    for (;;) {
        if (*si == c) *si++;
        if (!*si) break;
        *so++ = *si++;
    } /* for */
    *so = *si;
} /* string_dequote */

void test(const char *s)
{
    static char a[100];
    static char b[100];
    static int i = 1;
    printf("test #%d\n", i);
    memset(a, 'A', sizeof a); a[sizeof a - 1] = 0;
    memset(b, 'B', sizeof b); b[sizeof b - 1] = 0;
    printf("a = [%s];\nb = [%s];\n", a, b);
    strcpy(a, s);
    string_dequote(a, a);
    printf("a = [%s]; b = [%s];\n", a, b);
    printf("fin test #%d\n", i++);
}

main()
{
    char linea[1000];
    while(fgets(linea, sizeof linea, stdin)) {
        test(strtok(linea, "\n"));
    } /* while */
} /* main */

/* $Id: lexemas.c,v 1.2 2012/08/22 13:41:14 luis Exp $ */
