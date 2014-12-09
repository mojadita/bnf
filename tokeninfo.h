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
#include "lists.h"

#define TABSIZE		4
#define HOME		1
#define NLINES		15

static char TOKENINFO_H_RCSId[] = "\n$Id: tokeninfo.h,v 1.2 2012/08/22 13:41:14 luis Exp $\n";

struct tokeninfo {
	char*			str; /* string of token */
	int				lin; /* line of token in input file */
	int				col; /* col of token in input file */
	int				len; /* length of token */
	int				aut; /* if token is automatic. */
	LNODE_T			node; /* to link tokens */
};

void init_tokeninfo(
		int nl); /* number of lines of context to save */

struct tokeninfo* add_tokeninfo(
		const char* s, /* token string. */
		int l, /* line of token */
		int c, /* column of token */
		int len, /* length of token */
		int aut); /* auto: if this flag is true, erase
					 the token once the number of lines has
					 passed. */

size_t vfprint_tokeninfo(
		FILE* o, /* ouput file */
		const char *fmt, /* format string of error line. */
		va_list p); /* variable argument list */

size_t vprint_tokeninfo(
		const char *fmt, /* format string */
		va_list p); /* variable argument list */

size_t fprint_tokeninfo(FILE* o,
		const char *fmt, /* format string */
		...);

size_t print_tokeninfo(
		const char* fmt, /* format string */
		...);

#endif /* TOKENINFO_H */

/* $Id: tokeninfo.h,v 1.2 2012/08/22 13:41:14 luis Exp $ */
