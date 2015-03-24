/* $Id: lexemas.h,v 1.2 2012/08/22 13:41:14 luis Exp $
 * vim: ts=4 sw=4 nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: lun jul 16 11:43:49 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *             All rights reserved.
 */

#ifndef LEXEMAS_H
#define LEXEMAS_H

static char LEXEMAS_H_RCSId[] = "\n$Id: lexemas.h,v 1.2 2012/08/22 13:41:14 luis Exp $\n";

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

main()
{
    char a[100] = "/prueba//de//cadena/";
    char b[100];
    string_dequote(a, b);
    printf("a = [%s]; b = [%s];\n", a, b);
} /* main */

#endif /* LEXEMAS_H */
/* $Id: lexemas.h,v 1.2 2012/08/22 13:41:14 luis Exp $ */
