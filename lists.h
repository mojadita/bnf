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
#define LISTS_H_PACKAGE_VERSION "0.3"
#define LISTS_H_PACKAGE_STRING "lists 0.3"

/* constants */

/* types */
typedef struct LNODE_S {
		struct LNODE_S *prev;
		struct LNODE_S *next;
} LNODE_T, *LNODE_P;

/* definitions */
#ifndef OFFSETOF
#define OFFSETOF(X, f) ((int)&(((X *)0)->f))
#endif

#define LIST_OFFSET(X, f) ((X *)0)->f
#define LIST_INIT(X) {(X)->prev = (X)->next = (X);}
#define LIST_DECLARE(X) LNODE_T X = { .next = &X, .prev = &X }

#define LIST_FIRST(X) (LIST_EMPTY(X) ? NULL : (X)->next)
#define LIST_LAST(X) (LIST_EMPTY(X) ? NULL : (X)->prev)
#define LIST_NEXT(X) LIST_FIRST(X)
#define LIST_PREV(X) LIST_LAST(X)

#define LIST_ATEOL(p, L) ((p)==(L))
#define LIST_FOREACH(p, L) for(p=(L)->next;p!=(L);p=p->next)
#define LIST_FORBACK(p, L) for(p=(L)->prev;p!=(L);p=p->prev)

#define LIST_FOREACH_ELEMENT(p,L,T,f) for(p=LIST_ELEMENT((L)->next,T,f);p&&(&p->node!=(L));p=LIST_ELEMENT(p->f.next,T,f))
#define LIST_FORBACK_ELEMENT(p,L,T,f) for(p=LIST_ELEMENT((L)->prev,T,f);p&&(&p->node!=(L));p=LIST_ELEMENT(p->f.prev,T,f))

#define LIST_EMPTY(L) ((L)->next == (L))

#define LIST_INSERT(L, p) { (p)->next = (L)->next; (p)->prev = (L); (L)->next = (p); (p)->next->prev = (p); }
#define LIST_APPEND(L, p) { (p)->prev = (L)->prev; (p)->next = (L); (L)->prev = (p); (p)->prev->next = (p); }
#define LIST_DELETE(p) { (p)->next->prev = (p)->prev; (p)->prev->next = (p)->next; }

#define LIST_ELEMENT(p, T, f) ((p) ? (T *)((char *)(p) - OFFSETOF(T, f)): NULL)

#endif /* LISTS_H */
/* Do not include anything AFTER the line above, as it would not be
 * protected against double inclusion from other files.
 */

/* $Id: lists.h,v 1.1.1.1 2012/08/20 19:25:31 luis Exp $ */
