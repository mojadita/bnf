/* $Id: bnf_cfg.c,v 1.3 2012/09/20 10:41:52 luis Exp $
 * vim: ts=8 sw=4 ai nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Sun Sep  9 12:20:31 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *                 All rights reserved.
 * There is no warranty on using this product.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "avl.h"
#include "bnf_cfg.h"

static AVL_TREE params = NULL;

static void init_config(void)
{
    extern char **environ;
    int i;

    params = new_avl_tree((AVL_FCOMP)strcmp, NULL, NULL, NULL);

    for(i = 0; environ[i]; i++) {
        char buffer[256];
        char* p = strchr(environ[i], '=');
        int l;

        if (!p) {
            fprintf(stderr,
                    "WARNING: environment variable "
                    "without equal sign '=' (%s)\n",
                    environ[i]);
            continue;
        } /* if */
        l = p++ - environ[i];
        snprintf(buffer, sizeof buffer, "%.*s",
                l, environ[i]);
        avl_tree_put(params, buffer, strdup(p));
    } /* for */
} /* init_config */

char *get_config(char *s, char *d)
{
    char *res;
    if (!params) init_config();
    res = avl_tree_get(params, s);
    return res ? res : d;
} /* get_config */

void put_config(char *s, char *v)
{
    if (!params) init_config();
    avl_tree_put(params, s, v);
} /* put_config */

size_t fprint_config_m4(FILE* o)
{
    AVL_ITERATOR p;
    size_t res = 0;

    if (!params) init_config();
    for (p = avl_tree_first(params); p; p = avl_iterator_next(p)) {
        res += fprintf(o,
                "env(`%s', `%s')dnl\n",
                avl_iterator_key(p),
                avl_iterator_data(p));
    } /* for */
    return res;
} /* fprint_config */

size_t print_config_m4(void)
{
    return fprint_config_m4(stdout);
} /* print_config_m4 */

/* $Id: bnf_cfg.c,v 1.3 2012/09/20 10:41:52 luis Exp $ */
