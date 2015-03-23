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

struct ti_db *init_tokeninfo(struct ti_db *res)
{
    if (!res) assert(res = malloc(sizeof(struct ti_db)));

    /* initialize fields */
    assert(res->tokens = new_avl_tree( /* token database */
            (AVL_FCOMP) strcmp,        /* fcomp */
            (AVL_FCONS) strdup,        /* fcons */
            (AVL_FDEST) free,          /* fdest */
            (AVL_FPRNT) fputs));       /* fprnt */
    LIST_INIT(&res->input_list);       /* token references list */
    res->tab_size   = TABSIZE;         /* fields for printing */
    res->home       = HOME;
    res->n_lines    = NLINES;
    res->fpx        = NULL;            /* no first printable xref yet */

    return res;
} /* init_tokeninfo */

struct ti_xref* add_tokeninfo(
        struct ti_db    *gd,   /* global database */
        const char      *s,    /* string of token */
        int             typ,   /* type of token */
        int             l,     /* line */
        int             c)     /* column */
{
	LNODE_P             p;
	struct ti_xref      *res;
    struct ti_item      *itm;

    assert(gd);
    itm = avl_tree_get(gd->tokens, s);
    if (!itm) { /* it doesn't exist */
        AVL_ITERATOR it;

        assert(itm = malloc(sizeof(struct ti_item)));
        it = avl_tree_put(gd->tokens, s, itm);
        itm->db = gd;
        itm->str = (const char *) avl_iterator_key(it);
        itm->len = strlen(s);
        LIST_INIT(&itm->xrefs_list);
        itm->typ = typ;
    } /* if */

    /* go with res */
	assert(res = malloc(sizeof (struct ti_xref)));
    res->tinfo = itm;
    res->flags = 0; /* no flags at initialization, after... */
	res->lin = l;
	res->col = c;
	LIST_APPEND(&itm->xrefs_list, &res->xrefs_node);
    LIST_APPEND(&gd->input_list, &res->input_node);

    /* advance the pointer to the first printable xref, if
     * necessary */
    if (!gd->fpx) {
        gd->fpx = res; /* first xref */
    } else {
        while (gd->fpx && gd->fpx->lin + gd->n_lines < l)
            gd->fpx = LIST_ELEMENT_NEXT(gd->fpx, struct ti_xref, input_node);
    } /* if */

	return res;
} /* add_tokeninfo */

size_t vfprint_tokeninfo(
        struct ti_db    *db,
        FILE            *o,
        const char      *fmt,
        va_list         p)
{
	int                 lin = 0;
	int                 col = HOME;
	struct ti_xref      *t  = NULL;
	size_t              res = 0;
	int                 has_print_something
                            = 0;
	char                buff[20];
    struct ti_xref       *l = LIST_ELEMENT_LAST(&db->input_list, struct ti_xref, input_node);
	int                 ndig = snprintf(buff, sizeof buff, "%d", l ? l->lin : 0);

    assert(db);

    /* print the xrefs elements */
    for(t = db->fpx;
        t;
        t = LIST_ELEMENT_NEXT(t, struct ti_xref, input_node)
    ) {
		if (!lin) {
			lin = t->lin;
			res += fprintf(o, "%0*d: ", ndig, lin);
			col = db->home;
		} else while (lin < t->lin) {
			res += fprintf(o, "\n%0*d: ", ndig, ++lin);
			col = db->home;
		} /* if */
		res += fprintf(o, "%*s", t->col - col, "");
		res += fprintf(o, "%s", t->tinfo->str);
		col = t->col + t->tinfo->len;
		lin = t->lin;
		has_print_something = 1;
	} /* for */

    /* mark the last element */
	if (l) {
		col = db->home;
		res += fprintf(o,
				"\n%.*s: %*s%*.*s\n%s> ",
				ndig, "********************",
				t->col - col, "",
				(int)t->tinfo->len, (int)t->tinfo->len,
"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^",
				buff);
		res += vfprintf(o, fmt, p);
	} /* if */

    res += fprintf(o, "\n");

    return res;
} /* vfprint_tokeninfo */

size_t vprint_tokeninfo(
        struct ti_db    *db,
        const char *fmt,
        va_list p)
{
	vfprint_tokeninfo(db, stdout, fmt, p);
} /* vprint_tokeninfo */

size_t fprint_tokeninfo(
        struct ti_db *db,
        FILE* o,
        const char *fmt,
        ...)
{
	va_list p;
	va_start(p, fmt);
	vfprint_tokeninfo(db, o, fmt, p);
	va_end(p);
} /* fprint_tokeninfo */

size_t print_tokeninfo(
        struct ti_db *db,
        const char* fmt,
        ...)
{
	va_list p;
	va_start(p, fmt);
	vprint_tokeninfo(db, fmt, p);
	va_end(p);
} /* print_tokeninfo */

/* $Id: tokeninfo.c,v 1.3 2012/08/26 22:39:39 luis Exp $ */
