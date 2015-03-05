/* $Id: tokeninfo.h,v 1.2 2012/08/22 13:41:14 luis Exp $
 * vim: ts=4 sw=4 ai nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Fri Jul 20 17:00:07 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *             All rights reserved.
 */

#ifndef TOKENINFO_H
#define TOKENINFO_H

#include <sys/types.h>
#include <stdio.h>
#include <stdarg.h>
#include <avl.h>
#include "lists.h"


static char TOKENINFO_H_RCSId[] = "\n$Id: tokeninfo.h,v 1.2 2012/08/22 13:41:14 luis Exp $\n";

/* tokeninfo database is compound of a group of
 * struct ti_info tokens for each token type.
 * each time it appears in source file, a
 * struct ti_xref is added, chaining them all
 * so the source file can be reconstructed from
 * them.  Adding a token makes an entry in 
 * ti_tokens if it has no entry, and then appends
 * the reference to the ti_input_list and to the
 * xrefs list of the ti_info corresponding to it.
 */

struct ti_db {
    AVL_TREE        tokens; /* items in database */
    LNODE_T         input_list; /* input list */
    int             tab_size; /* tab size to compute tab positions */
    int             n_lines; /* number of lines of previous context printed */
};

struct ti_item {
	char*			str; /* token string */
	LNODE_T			xrefs; /* to link input tokens */
    int             typ; /* token type */
};

#define TI_DEFINED_HERE  (1 << 0) /* place of definition */

struct ti_xref {
    struct ti_info  *tinfo; /* reference to token class */
    int             flags; /* flags, see above */
    int             lin; /* line where this xref appears */
    int             col; /* column where this xref begins */
    LNODE_T         node; /* input list links */
};

struct ti_db *init_tokeninfo();

struct ti_xref *add_tokeninfo(
        struct ti_db *d, /* global tokeninfo database */
		const char* s, /* token string. */
        int typ, /* token type */
		int l, /* line of token */
		int c); /* column of token */

size_t vfprint_tokeninfo(
		FILE* o, /* ouput file */
		const char *fmt, /* format string of error line. */
		va_list p); /* variable argument list */

size_t vprint_tokeninfo(
		const char *fmt, /* format string */
		va_list p); /* variable argument list */

size_t fprint_tokeninfo(
        FILE* o, /* output file */
		const char *fmt, /* format string */
		...);

size_t print_tokeninfo(
		const char* fmt, /* format string */
		...);

size_t xref_tokeninfo(
        FILE *o); /* output file */

#endif /* TOKENINFO_H */

/* $Id: tokeninfo.h,v 1.2 2012/08/22 13:41:14 luis Exp $ */
