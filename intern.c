/* $Id: intern.c,v 1.1 2014/09/09 20:22:07 luis Exp $
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: sáb ago 23 11:50:37 EEST 2014
 * Disclaimer: (C) 2014 LUIS COLORADO SISTEMAS S.L.U.  All rights reserved.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "intern.h"

#ifndef DEBUG
#define DEBUG   0
#endif

#ifndef D
#define D(x) __FILE__":%d:%s:" x, __LINE__, __func__
#endif

AVL_TREE intern_strings = NULL;

const char *intern(const char *s)
{
    const char *res = "<<invalid>>";
    AVL_ITERATOR i;

    if (!intern_strings) {
        assert(intern_strings = new_avl_tree(
            (AVL_FCOMP) strcmp,
            (AVL_FCONS) strdup,
            (AVL_FDEST) free,
            (AVL_FPRNT) fputs));
    } /* if */

    /* THE FOLLOWING SAVES A ROUNDTRIP TO FIND THE 
     * KEY AGAIN */
    i = avl_tree_put(intern_strings, s, res);
    avl_iterator_set_data(i, res = avl_iterator_key(i));

    return res;
} /* intern */
    
/* $Id: intern.c,v 1.1 2014/09/09 20:22:07 luis Exp $ */
