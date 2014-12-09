/* $Id: print_strtable.h,v 1.1 2012/08/24 13:31:58 luis Exp $
 * vim: sw=4 ts=4 ai nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: vie ago 24 09:58:11 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *                 All rights reserved.
 */

#ifndef PRINT_TABSYM_H
#define PRINT_TABSYM_H

#include <stdio.h>
#include <stdarg.h>

#include "avl.h"

int print_tabsym(AVL_TREE t, char *fmt, ...);
int vprint_tabsym(AVL_TREE t, char *fmt, va_list v);
int fprint_tabsym(FILE* f, AVL_TREE t, char *fmt, ...);
int vfprint_tabsym(FILE* f, AVL_TREE t, char *fmt, va_list v);

#endif /* PRINT_TABSYM_H */
/* $Id: print_strtable.h,v 1.1 2012/08/24 13:31:58 luis Exp $ */
