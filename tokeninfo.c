/* $Id: tokeninfo.c,v 1.3 2012/08/26 22:39:39 luis Exp $
 * vim: ts=4 sw=4 ai nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Fri Jul 20 17:00:07 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *             All rights reserved.
 */

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

#include "bnf.h"
#include "tokeninfo.h"
#include "bnf_pparser.h"

#define TABSIZE     4
#define HOME        1
#define NLINES      15

static char TOKENINFO_C_RCSId[] = "\n$Id: tokeninfo.c,v 1.3 2012/08/26 22:39:39 luis Exp $\n";

struct ti_db *init_tokeninfo(struct ti_db *res)
{
    if (!res) assert(res = malloc(sizeof(struct ti_db)));

    /* initialize fields */
    assert(res->tokens = new_avl_tree( /* token database */
            (AVL_FCOMP) strcmp,        /* fcomp */
            (AVL_FCONS) NULL,          /* fcons */
            (AVL_FDEST) NULL,          /* fdest */
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
        assert(itm = malloc(sizeof(struct ti_item)));
        itm->db = gd;
        itm->str = strdup(s);

        switch (typ) {
        case IDENT: case STRING:
            avl_tree_put(gd->tokens, itm->str, itm);
            break;
        } /* switch */

        itm->len = strlen(itm->str);
        LIST_INIT(&itm->xrefs_list);
        itm->typ = typ;

    } /* if */

    /* go with res */
    assert(res = malloc(sizeof (struct ti_xref)));
    res->tinfo = itm;
    res->flags = 0; /* no flags at initialization, after... */
    res->lin = l;
    res->col = c;
    LIST_APPEND(&gd->input_list, &res->input_node);
    LIST_APPEND(&itm->xrefs_list, &res->xrefs_node);

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
    struct ti_xref       *l = LIST_ELEMENT_LAST(
            &db->input_list, struct ti_xref, input_node);
    int                 ndig = snprintf(buff, sizeof buff, "%d", l ? l->lin : 0);

    assert(db);

    /* print the xrefs elements */
    for(t = db->fpx;
        t;
        t = LIST_ELEMENT_NEXT(t, struct ti_xref, input_node)
    ) {
        const char *esc1;
        const char *esc2 = "\033[0m";

        switch (t->tinfo->typ) {
        case COMMENT:        esc1 = "\033[34m";   break;
        case IDENT:          esc1 = t->flags & TI_DEFINED_HERE
                                ? "\033[33m"
                                : "\033[37m";
                             break;
        case STRING:         esc1 = "\033[35m"; break;
        case ';':
        case COLON_COLON_EQ: esc1 = "\033[31m"; break;
        case '|':
        case '[': case ']':
        case '{': case '}':
        case '(': case ')':  esc1 = "\033[32m"; break;
        default:             esc1 = "";           break;
        } /* switch */

        if (!lin) {
            lin = t->lin;
            res += fprintf(o, "%0*d: ", ndig, lin); fflush(o);
            col = db->home;
        } else while (lin < t->lin) {
            res += fprintf(o, "\n%0*d: ", ndig, ++lin); fflush(o);
            col = db->home;
        } /* if */
        res += fprintf(o, "%*s", t->col - col, ""); fflush(o);
        res += fprintf(o, "%s%s%s", esc1, t->tinfo->str, esc2); fflush(o);
        col = t->col + t->tinfo->len;
        lin = t->lin;
        has_print_something = 1;
    } /* for */

    /* mark the last element */
    if (l) {
        col = db->home;
        res += fprintf(o,
                "\n%.*s: %*s%*.*s\n%s> ",
                ndig, "####################",
                l->col - col, "",
                (int)l->tinfo->len, (int)l->tinfo->len,
"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^",
                buff); fflush(o);
        res += vfprintf(o, fmt, p); fflush(o);
    } /* if */

    return res;
} /* vfprint_tokeninfo */

size_t vprint_tokeninfo(
        struct ti_db    *db,
        const char      *fmt,
        va_list         p)
{
    vfprint_tokeninfo(db, stdout, fmt, p);
} /* vprint_tokeninfo */

size_t fprint_tokeninfo(
        struct ti_db    *db,
        FILE            *o,
        const char      *fmt,
        ...)
{
    va_list p;
    va_start(p, fmt);
    vfprint_tokeninfo(db, o, fmt, p);
    va_end(p);
} /* fprint_tokeninfo */

size_t print_tokeninfo(
        struct ti_db    *db,
        const char      *fmt,
        ...)
{
    va_list p;
    va_start(p, fmt);
    vprint_tokeninfo(db, fmt, p);
    va_end(p);
} /* print_tokeninfo */

size_t xref_tokeninfo(
        struct ti_db    *db,    /* tokeninfo db. */
        FILE            *o,     /* output file */
        const char      *n)     /* output filename */
{
    size_t res = 0;
    AVL_ITERATOR i;

    assert(db);

    for (   i = avl_tree_first(db->tokens);
            i;
            i = avl_iterator_next(i))
    {
        struct ti_item *it = (struct ti_item *) avl_iterator_data(i);
        struct ti_xref *xr;
        char *sn = "UNKNOWN";

        switch(it->typ) {
        case STRING: sn = "STRING"; break;
        case IDENT: sn = "IDENTIFIER"; break;
        } /* switch */

        LIST_FOREACH_ELEMENT(
                xr,
                &it->xrefs_list,
                struct ti_xref,
                xrefs_node)
        {
            char *isdef = xr->flags & TI_DEFINED_HERE ? "*" : "";
            res += fprintf(o,"%s:%d:%d:%s%s <%s>\n",
                    n, xr->lin, xr->col, sn, isdef, it->str);
        } /* LIST_FOREACH_ELEMENT */
    } /* for */

    return res;
} /* xref_tokeninfo */

/* $Id: tokeninfo.c,v 1.3 2012/08/26 22:39:39 luis Exp $ */
