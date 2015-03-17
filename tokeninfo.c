/* $Id: tokeninfo.c,v 1.3 2012/08/26 22:39:39 luis Exp $
 * vim: ts=4 sw=4 ai nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Fri Jul 20 17:00:07 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *             All rights reserved.
 */

#include <sys/types.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

#include "tokeninfo.h"

#define TABSIZE		4
#define HOME		1
#define NLINES		15

static char TOKENINFO_C_RCSId[] = "\n$Id: tokeninfo.c,v 1.3 2012/08/26 22:39:39 luis Exp $\n";

struct ti_db *init_tokeninfo()
{
    struct ti_db *res;

    assert(res = malloc(sizeof(struct ti_db)));
    res->tokens = new_avl_tree(
            (AVL_FCOMP) ti_cmp, /* fcomp */
            (AVL_FCONS) strdup, /* fcons */
            (AVL_FDEST) free,   /* fdest */
            (AVL_FPRNT) fputs); /* fprnt */
    LIST_INIT(&res->input_list);
    res->tab_size   = TABSIZE;
    res->home       = HOME;
    res->n_lines    = NLINES;

    return res;
} /* init_tokeninfo */

struct tokeninfo* add_tokeninfo(
        struct ti_db    *gd,   /* global database */
        const char      *s,    /* string of token */
        int             typ,   /* type of token */
        int             l,     /* line */
        int             c)     /* column */
{
	LNODE_P             p;
	struct ti_xref      *res;
    struct ti_item      *itm;
    AVL_ITERATOR        it;

    itm = avl_tree_get(gd->tokens, s);
    if (!itm) {
        AVL_ITERATOR it;
        assert(itm = malloc(sizeof(struct ti_item)));
        it = avl_tree_put(gd->tokens, s, itm);
        itm->str = avl_iterator_key(it);
        itm->typ = typ;
        LIST_INIT(&itm->xrefs);
    } /* if */
	assert(res = malloc(sizeof (struct ti_xref)));
    res->tinfo = itm;
    res->flags = 0; /* no flags at initialization, after... */
	res->lin = l;
	res->col = c;
	LIST_APPEND(&itm->xrefs, &res->node);

	return res;
} /* add_tokeninfo */

size_t vfprint_tokeninfo(FILE* o, const char *fmt, va_list p)
{
	int                 lin = 0;
	int                 col = HOME;
	struct tokeninfo    *t  = NULL;
	size_t              res = 0;
	int                 has_print_something
                            = 0;
	char                buff[20];
	int                 ndig = snprintf(buff, sizeof buff, "%d",
			LIST_ELEMENT(LIST_LAST(&data), struct tokeninfo, node)->lin);

	LIST_FOREACH_ELEMENT(t, &data, struct tokeninfo, node) {
		if (!lin) {
			lin = t->lin;
			res += fprintf(o, "%0*d: ", ndig, lin);
			col = HOME;
		} else while (lin < t->lin) {
			res += fprintf(o, "\n%0*d: ", ndig, ++lin);
			col = HOME;
		} /* if */
		res += fprintf(o, "%*s", t->col - col, "");
		res += fprintf(o, "%s", t->str);
		col = t->col + t->len;
		lin = t->lin;
		has_print_something = 1;
	} /* LIST_FOREACH_ELEMENT */
	t = LIST_ELEMENT(LIST_LAST(&data), struct tokeninfo, node);
	if (t) {
		col = HOME;
		res += fprintf(o,
				"\n%0.*s: %*s%*.*s\n%s> ",
				ndig, "--------------------",
				t->col - col, "",
				t->len, t->len,
"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^",
				buff);
		res += vfprintf(o, fmt, p);
	} /* if */
} /* vfprint_tokeninfo */

size_t vprint_tokeninfo(const char *fmt, va_list p)
{
	vfprint_tokeninfo(stdout, fmt, p);
} /* vprint_tokeninfo */

size_t fprint_tokeninfo(FILE* o, const char *fmt, ...)
{
	va_list p;
	va_start(p, fmt);
	vfprint_tokeninfo(o, fmt, p);
	va_end(p);
} /* fprint_tokeninfo */

size_t print_tokeninfo(const char* fmt, ...)
{
	va_list p;
	va_start(p, fmt);
	vprint_tokeninfo(fmt, p);
	va_end(p);
} /* print_tokeninfo */

/* $Id: tokeninfo.c,v 1.3 2012/08/26 22:39:39 luis Exp $ */
