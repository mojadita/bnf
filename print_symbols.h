/* $Id: print_symbols.h,v 1.3 2012/08/26 22:39:39 luis Exp $
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: mié ago 22 10:40:47 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *                 All rights reserved.
 */
#ifndef PRINT_SYMBOLS_H
#define PRINT_SYMBOLS_H

#include <stdio.h>
#include "avl.h"

static char PRINT_SYMBOLS_H_RCSId[] = "\n$Id: print_symbols.h,v 1.3 2012/08/26 22:39:39 luis Exp $\n";

int print_symbols(AVL_TREE t, char *tpl);
int fprint_symbols(FILE* o, AVL_TREE t, char *tpl);

#endif /* PRINT_SYMBOLS_H */
/* $Id: print_symbols.h,v 1.3 2012/08/26 22:39:39 luis Exp $ */
