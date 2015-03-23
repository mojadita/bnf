/* $Id: print_symbols.c,v 1.4 2012/08/27 13:55:31 luis Exp $
 * vim: ts=4 sw=4 ai nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: mié ago 22 10:37:33 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *                 All rights reserved.
 */

#include "print_symbols.h"

static char PRINT_SYMBOLS_C_RCSId[] =
"\n$Id: print_symbols.c,v 1.4 2012/08/27 13:55:31 luis Exp $\n";

#define NCOL                                80
#define ORIG                                1
#define BS                                  128
#define LEFT_STRING                     "  "

int print_symbols(AVL_TREE t, char *tpl)
{
    return fprint_symbols(stdout, t, tpl);
} /* print_symbols */


int fprint_symbols(FILE* o, AVL_TREE t, char *tpl)
{
    AVL_ITERATOR p;
    int i = avl_tree_size(t);
    int ll = fprintf(o, LEFT_STRING);
    int ncol = ORIG + ll;
    int res = ncol;

    for (p = avl_tree_first(t); p; p = avl_iterator_next(p)) {

        char *k = avl_iterator_key(p);
        char buffer[BS];

        int n = snprintf(buffer, sizeof buffer,
                tpl, k, (--i) ? ", " : "");

        if ((ncol + n) > NCOL) {
            n = fprintf(o, "\n" LEFT_STRING);
            res += n; ncol = ll;
        } /* if */

        n = fprintf(o, "%s", buffer);
        ncol += n; res += n;
    } /* for */
    if (ncol != ORIG) {
        res += fprintf(o, "\n");
    } /* if */
    return res;
} /* fprint_symbols */

/* $Id: print_symbols.c,v 1.4 2012/08/27 13:55:31 luis Exp $ */
