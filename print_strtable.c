/* $Id: print_strtable.c,v 1.1 2012/08/24 13:31:58 luis Exp $
 * vim: sw=4 ts=4 ai nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: vie ago 24 09:58:11 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *                 All rights reserved.
 */

#include "print_strtable.h"

int print_tabsym(AVL_TREE t, char *fmt, ...)
{
	va_list v;
	int res;
	va_start(v, fmt);
	res = vprint_tabsym(t, fmt, v);
	va_end(v);
	return res;
} /* print_tabsym */

int fprint_tabsym(FILE* f, AVL_TREE t, char *fmt, ...)
{
	va_list v;
	int res;
	va_start(v, fmt);
	res	= vfprint_tabsym(f, t, fmt, v);
	va_end(v);
	return res;
} /* fprint_tabsym */

int vprint_tabsym(AVL_TREE t, char *fmt, va_list v)
{
	return vfprint_tabsym(stdout, t, fmt, v);
} /* vprint_tabsym */

int vfprint_tabsym(FILE* f, AVL_TREE t, char *fmt, va_list v)
{
	AVL_ITERATOR p;
	int res = 0;

	res += vfprintf(f, fmt, v);
	for (p = avl_tree_first(t); p; p = avl_iterator_next(p)) {
		res += printf("%35s => %-35s\n",
				avl_iterator_key(p),
				avl_iterator_data(p));
	} /* for */

	return res;
} /* print_tabsym */

/* $Id: print_strtable.c,v 1.1 2012/08/24 13:31:58 luis Exp $ */
