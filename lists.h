/* $Id: lists.h,v 1.1.1.1 2012/08/20 19:25:31 luis Exp $
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Mon Jul 18 17:21:22 CEST 2005
 *
 * Disclaimer:
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* Do not include anything BEFORE the line below, as it would not be
 * protected against double inclusion from other files
 */
#ifndef LISTS_H
#define LISTS_H

static char LISTS_H_RCSId[] = "\n$Id: lists.h,v 1.1.1.1 2012/08/20 19:25:31 luis Exp $\n";

#define LISTS_H_PACKAGE_NAME "lists"
#define LISTS_H_PACKAGE_VERSION "0.4"
#define LISTS_H_PACKAGE_STRING "lists 0.4"

/* constants */

/* types */
typedef struct LNODE_S {
		struct LNODE_S *p; /* previous element pointer */
		struct LNODE_S *n; /* next element pointer */
} LNODE_T, *LNODE_P;

/* definitions */
#ifndef OFFSETOF
#define OFFSETOF(T,f) ((int)&(((T *)0)->f))
#endif

#define LIST_INIT(d) do{(d)->p=(d)->n=0;}while(0)
#define LIST_DECLARE(d) LNODE_T d={0,0}

#define LIST_FIRST(L) ((L)->n)
#define LIST_LAST(L) ((L)->p)
#define LIST_NEXT(d) LIST_FIRST((d))
#define LIST_PREV(d) LIST_LAST((d))

#define LIST_FOREACH(i,L) for((i)=(L)->n;(i);(i)=(i)->n)
#define LIST_FORBACK(i,L) for((i)=(L)->p;(i);(i)=(i)->p)

#define LIST_ELEMENT(d,T,f) ((T *)((char *)(d)-OFFSETOF(T,f)))

#define LIST_ELEMENT_FIRST(L,T,f)   LIST_ELEMENT(LIST_FIRST((L)),T,f)
#define LIST_ELEMENT_LAST(L,T,f)    LIST_ELEMENT(LIST_LAST((L)),T,f)

#define LIST_ELEMENT_NEXT(d,T,f) LIST_ELEMENT(LIST_NEXT(&(d)->f),T,f)
#define LIST_ELEMENT_PREV(d,T,f) LIST_ELEMENT(LIST_PREV(&(d)->f),T,f)

#define LIST_FOREACH_ELEMENT(i,L,T,f) \
    for((i)=LIST_ELEMENT_FIRST((L),T,f);\
            (i);\
            (i)=LIST_ELEMENT_NEXT((i),T,f))

#define LIST_FORBACK_ELEMENT(i,L,T,f) \
    for((i)=LIST_ELEMENT_LAST((L),T,f);\
            (i);\
            (i)=LIST_ELEMENT_PREV((i),T,f))

#define LIST_EMPTY(L) (!LIST_FIRST((L)))

#define LIST_INSERT(L,d) do{(d)->n=(L)->n;(d)->p=0;(L)->n=(d);}while(0)
#define LIST_APPEND(L,d) do{(d)->p=(L)->p;(d)->n=0;(L)->p=(d);}while(0)
#define LIST_DELETE(L,d) do{\
            if((L)->n==(d))(L)->n=(d)->n;\
            if((L)->p==(d))(L)->p=(d)->p;\
            if((d)->p)(d)->p->n=(d)->n;\
            if((d)->n)(d)->n->p=(d)->p;\
        }while(0)


#endif /* LISTS_H */
/* Do not include anything AFTER the line above, as it would not be
 * protected against double inclusion from other files.
 */
/* $Id: lists.h,v 1.1.1.1 2012/08/20 19:25:31 luis Exp $ */
