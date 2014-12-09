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

static char TOKENINFO_C_RCSId[] = "\n$Id: tokeninfo.c,v 1.3 2012/08/26 22:39:39 luis Exp $\n";

static LIST_DECLARE(data);
static int nlines = NLINES;

void init_tokeninfo(int nl)
{
	LNODE_P p;
	nlines = nl;
	while ((p = LIST_FIRST(&data)) != NULL) {
		struct tokeninfo *t = LIST_ELEMENT(p, struct tokeninfo, node);
		LIST_DELETE(p);
		if (t->aut) {
			free(t->str);
			free(t);
		} /* if */
	} /* while */
} /* init_tokeninfo */

struct tokeninfo* add_tokeninfo(const char* s, int l, int c, int len, int aut)
{
	LNODE_P p;
	struct tokeninfo* res;
	while ((p = LIST_FIRST(&data)) != NULL) {
		struct tokeninfo* t = LIST_ELEMENT(p, struct tokeninfo, node);
		if (t->lin >= l - nlines) break;
		LIST_DELETE(p);
		if (t->aut) {
			free(t->str);
			free(t);
		} /* if */
	} /* while */
	assert(res = malloc(sizeof (struct tokeninfo)));
	res->str = strdup(s);
	res->lin = l;
	res->col = c;
	res->len = len;
	res->aut = aut;
	LIST_APPEND(&data, &res->node);
	return res;
} /* add_tokeninfo */

size_t vfprint_tokeninfo(FILE* o, const char *fmt, va_list p)
{
	int lin = 0;
	int col = HOME;
	struct tokeninfo* t;
	size_t res = 0;
	int has_print_something = 0;
	char buff[20];
	int ndig = snprintf(buff, sizeof buff, "%d",
			LIST_ELEMENT(LIST_LAST(&data),
				struct tokeninfo,
				node)->lin);

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
