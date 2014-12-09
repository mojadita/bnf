/* $Id: avl.c,v 1.3 2012/08/22 13:41:14 luis Exp $
 * vim: ts=4 sw=4 nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Wed Oct  7 17:57:51     2009
 *
 * Disclaimer:
 *  (C) 2009-2012 LUIS COLORADO SISTEMAS S.L.
 *  All rights reserved.  No part of this code can be distributed in
 *  any form, withour previous written consent of the copyright owner.
 *  This code is distributed WITHOUT ANY WARRANTY, even the implied
 *  WARRANTY OF MERCHANTABILITY or FITNESS FOR ANY PARTICULAR PURPOSE.
 *  In case of an agreed previous, written contract, the WARRANTIES
 *  applied will be the ones written in the applicable contract.
 * Nota: TRANSPORTADO A C en Mon Apr  9 16:48:29 CEST 2012
 * ===== para poder usarlo con flex/bison.
 */

#define IN_AVL_C

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>

#include "avl.h"

static char AVL_C_CVSID[] = "\n$Id: avl.c,v 1.3 2012/08/22 13:41:14 luis Exp $\n";
/* Standard include files */

/* constants */
#ifndef DEBUG
#define DEBUG 0
#endif

#ifdef DEB
#undef DEB
#endif
#if DEBUG
#define DEB(X) printf X
#else
#define DEB(X)
#endif

#ifndef PRINT_ALL
#define PRINT_ALL		0
#endif

#ifndef FALSE
#define FALSE 			0
#define TRUE			(!FALSE)
#endif

/* types */

typedef enum avl_equ {
	AVL_LFT,
	AVL_EQU,
	AVL_RGT,
} avl_equ;

struct avl_node {
	avl_equ				equi;
	struct avl_node		*parent;
	struct avl_node		*left;
	struct avl_node		*right;
	char				*key;
	void				*data;
};

struct avl_tree {
	int					sz;
	struct avl_node		*root;
	AVL_FCOMP			fcomp;
};

/* prototypes */
static char *avl_equ2str(avl_equ equ);
static struct avl_node *new_avl_node(
	const char *key,
	void *data,
	struct avl_node *prnt);
static void free_avl_node(struct avl_node *n);
static struct avl_node *avl_node_root(struct avl_node *n);
static struct avl_node *avl_node_first(struct avl_node *n);
static struct avl_node *avl_node_last(struct avl_node *n);
static struct avl_node *avl_node_prev(struct avl_node *n);
static struct avl_node *avl_node_next(struct avl_node *n);
static struct avl_node *avl_node_search(
	struct avl_node *n,
	const char *k,
	AVL_FCOMP fc,
	avl_equ *e);
static struct avl_node *avl_node_unlink(
	struct avl_node *const n,
	struct avl_node **const rt);
static int avl_node_level(struct avl_node *n);
static void avl_node_equilibrateLL(
	struct avl_node *n,
	struct avl_node **const rt);
static void avl_node_equilibrateRR(
	struct avl_node *n,
	struct avl_node **const rt);
static void avl_node_equilibrateLR(
	struct avl_node *n,
	struct avl_node **const rt);
static void avl_node_equilibrateRL(
	struct avl_node *n,
	struct avl_node **const rt);
static int avl_node_printNode(
	struct avl_node *n, FILE *o,
	const char *pfx);
static void avl_node_printL(
	struct avl_node *n, FILE *o,
	const char *pfx);
static void avl_node_printR(
	struct avl_node *n,
	FILE *o,
	const char *pfx);
static void avl_node_print(
	struct avl_node *n,
	FILE *o);

/* variables */
static char AVL_CPP_RCSId[]="\n$Id: avl.c,v 1.3 2012/08/22 13:41:14 luis Exp $\n";

/* functions */
static char *avl_equ2str(avl_equ equ)
{
	switch(equ) {
	case AVL_RGT: return "\xe2\x86\x91";
	case AVL_EQU: return "\xe2\x86\x95";
	case AVL_LFT: return "\xe2\x86\x93";
	default: return "\342\232\240";
	} /* switch */
} /* avl_equ2str */

static struct avl_node *new_avl_node(
	const char *key,
	void *data,
	struct avl_node *prnt)
{
	struct avl_node *res;
	assert(res = malloc(sizeof (struct avl_node)));

	res->parent = prnt;
	res->left = NULL;
	res->right = NULL;
	res->equi = AVL_EQU;
	res->key = strdup(key);
	res->data = data;
	return res;
} /* new_avl_node */

static void free_avl_node(struct avl_node *n)
{
	if (n->left) free_avl_node(n->left);
	if (n->right) free_avl_node(n->right);
	DEB(("free_avl_node: deleting %p\n", n));
	free(n->key);
	free(n);
} /* free_avl_node */

static struct avl_node *avl_node_root(struct avl_node *n)
{
	struct avl_node *res;

	for (res = n; res->parent; res = res->parent)
		; /* OJO: cuerpo intencionadamente vacío */
	return res;
} /* avl_node_root */

static struct avl_node *avl_node_first(struct avl_node *n)
{
	struct avl_node *res;

	for (res = n; res->left; res = res->left)
		; /* OJO: cuerpo intencionadamente vacío */
	return res;
} /* avl_node_first */

static struct avl_node *avl_node_last(struct avl_node *n)
{
	struct avl_node *res;

	for (res = n; res->right; res = res->right)
		; /* OJO: cuerpo intencionadamente vacío */
	return res;
} /* avl_node_last */

static struct avl_node *avl_node_prev(struct avl_node *n)
{
	if (n->left) return avl_node_last(n->left);
	while (n->parent && (n->parent->left == n)) n = n->parent;
	return n->parent;
} /* avl_node_prev */

static struct avl_node *avl_node_next(struct avl_node *n)
{
	if (n->right) return avl_node_first(n->right);
	while (n->parent && (n->parent->right == n)) n = n->parent;
	return n->parent;
} /* avl_node_next */

static struct avl_node *avl_node_search(
	struct avl_node *n,
	const char *k,
	AVL_FCOMP fc,
	avl_equ *e)
{
	struct avl_node *p = n;

	if (!n) return NULL;

	for (;;) {
		int cmp = fc(k, p->key); /* comparamos */

		if (cmp == 0) { /* k == p->key */
			if (e) *e = AVL_EQU;
			return p;
		} /* if */
		if (cmp < 0) { /* k < p->key */
			if (e) *e = AVL_LFT;
			if (!p->left) break;
			p = p->left;
		} else { /* k > p->key */
			if (e) *e = AVL_RGT;
			if (!p->right) break;
			p = p->right;
		} /* if */
	} /* for (;;) */
	return p;
} /* avl_node_search */

static struct avl_node *avl_node_unlink(
	struct avl_node *const n,
	struct avl_node ** const rt)
{
	struct avl_node *p = n, *q;
	char e, disminuido;
	struct avl_node *res;

	/* SI TIENE AMBOS HIJOS... DESLIGAMOS EL SIGUIENTE O EL
	 * ANTERIOR, Y LUEGO LO COLOCAMOS AQUÍ (EN LUGAR DE
	 * ESTE NODO). */
	if (p->left && p->right) {
		switch(p->equi) {
		case AVL_EQU:
		case AVL_LFT: p = avl_node_prev(p); break;
		case AVL_RGT: p = avl_node_next(p); break;
		} /* switch */
	} /* if */

	/* 1.- p APUNTA AL NODO QUE VAMOS A BORRAR.
	 * 2.- p NO TIENE AMBOS HIJOS.  (CASO CONTEMPLADO EN EL
	 *     CASO ANTERIOR), ASÍ QUE HEMOS DE ELIMINARLO COMO
	 *     SI DE UNA LISTA ENLAZADA SE TRATARA. */

	q = p->parent; /* q es el parent */

	/* lo desligamos */
	res = p;
	if (q) { /* hay parent. */
		if (q->left == p) { /* es hijo izquierdo */
			q->left = p->left ? p->left : p->right;
			if (q->left) q->left->parent = q;
			e = AVL_LFT;
		} else { /* q->right == p */
			q->right = p->right ? p->right : p->left;
			if (q->right) q->right->parent = q;
			e = AVL_RGT;
		} /* if */
	} else { /* NO HAY PARENT, debemos tocar rt. */
		*rt = p->left ? p->left : p->right;
		if (*rt) (*rt)->parent = NULL;
		p = *rt;
		e = AVL_EQU;
	} /* if */

	/* AJUSTAMOS LOS PUNTEROS DEL NODO DESLIGADO */
	res->left = res->right = res->parent = NULL;

	/* res ESTÁ LISTO PARA SER DEVUELTO, YA HA SIDO DESLIGADO, AHORA
	 * HAY QUE SUBIR POR EL ÁRBOL PARA EQUILIBRAR.
	 * AHORA DEBEMOS SUBIR POR EL ÁRBOL MIENTRAS LA ALTURA HAYA
	 * DISMINUIDO, BIEN HASTA QUE ENCONTREMOS UN NODO QUE SE
	 * EQUILIBRA O UNO QUE SE DESEQUILIBRA DEMASIADO. */
	disminuido = TRUE;
	while (disminuido && q) {
		switch (e) {
		case AVL_LFT: /* se perdió altura por la izquierda. */
			switch (q->equi) {
			case AVL_LFT: q->equi = AVL_EQU; break;
			case AVL_EQU: q->equi = AVL_RGT; disminuido = FALSE; break;
			case AVL_RGT:
				switch (q->right->equi) {
				case AVL_EQU: disminuido = FALSE;
					/* OJO: NO HAY BREAK INTENCIONADAMENTE */
				case AVL_RGT:
					/* en los borrados el equilibrado siempre hace
					 * disminuir la altura, así que no tocamos la
					 * variable disminuido. */
					avl_node_equilibrateRR(q,
						q->parent
							? q->parent->left == q
								? &q->parent->left
								: &q->parent->right
							: rt);
					break;
				case AVL_LFT:
					/* en los borrados el equilibrado siempre hace
					 * disminuir la altura, así que no tocamos la
					 * variable disminuido. */
					avl_node_equilibrateRL(q,
						q->parent
							? q->parent->right == q
								? &q->parent->right
								: &q->parent->left
							: rt);
					break;
				} /* switch */
				q = q->parent; /* para que apunte al nodo correcto. */
				break;
			} /* switch */
			break;
		case AVL_RGT: /* se perdió altura por la derecha. */
			switch(q->equi) {
			case AVL_RGT: q->equi = AVL_EQU; break;
			case AVL_EQU: q->equi = AVL_LFT; disminuido = FALSE; break;
			case AVL_LFT: /* hay que equilibrar. */
				switch (q->left->equi) {
				case AVL_EQU: disminuido = FALSE;
					/* OJO, NO HAY BREAK INTENCIONADAMENTE */
				case AVL_LFT:
					/* EN LOS BORRADOS EL EQUILIBRADO SIEMPRE HACE
					 * DISMINUIR LA ALTURA, ASÍ QUE NO TOCAMOS LA
					 * VARIABLE disminuido. */
					avl_node_equilibrateLL(q,
						q->parent
							? q->parent->right == q
								? &q->parent->right
								: &q->parent->left
							: rt);
					break;
				case AVL_RGT:
					/* EN LOS BORRADOS EL EQUILIBRADO SIEMPRE HACE
					 * DISMINUIR LA ALTURA, ASÍ QUE NO TOCAMOS LA
					 * VARIABLE disminuido. */
					avl_node_equilibrateLR(q,
						q->parent
							? q->parent->left == q
								? &q->parent->left
								: &q->parent->right
							: rt);
					break;
				} /* switch */
				q = q->parent; /* para que apunte al nodo correcto */
				break;
			} /* switch */
			break;
		} /* switch */
		e = q->parent
			? q->parent->left == q
				? AVL_LFT
				: AVL_RGT
			: AVL_EQU;
		q = q->parent;
	} /* while */

	/* si el nodo desligado no es n, eso quiere decir
	 * que n tenía dos hijos y hemos deligado el nodo
	 * siguiente o el anterior, debemos ponerlo en lugar
	 * de n, para poder borrar n. */
	if (n != res) {
		if (n->parent) {
			if (n->parent->left == n)
				n->parent->left = res;
			else
				n->parent->right = res;
		} else { /* hay que tocar *rt */
			*rt = res;
		} /* if */
		if (n->left) n->left->parent = res;
		if (n->right) n->right->parent = res;
		res->parent = n->parent;
		res->left = n->left;
		res->right = n->right;
		res->equi = n->equi;
		n->parent = n->left = n->right = NULL;
	} /* if */

	return n;
} /* avl_node_unlink */

static int avl_node_level(struct avl_node *n)
{
	int res = 0;
	while(n) {
		res++;
		n = n->parent;
	} /* while */
	return res;
} /* avl_node_level */

static void avl_node_equilibrateLL(
	struct avl_node *n,
	struct avl_node **const rt)
{
	struct avl_node *l, *r;
	assert(*rt == n);

	/* LA OPERACIÓN DE REEQUILIBRADO LL SE
	 * PRODUCE AL INSERTAR EN LA RAMA
	 * IZQUIERDA EL NODO MARCADO +++.  (EL
	 * NODO MARCADO xxx NO EXISTE) SE
	 * PRODUCE UNA DIFERENCIA EN EL NODO n
	 * DE DOS ALTURAS.
	 * TAMBIÉN SE PRODUCE AL BORRAR EN LA
	 * RAMA DERECHA EL NODO MARCADO xxx
	 * (ENTONCES EXISTE YA EL NODO MARCADO
	 * +++).
	 *
	 *                   |
	 *                +------+           ---
	 *          +-----| this |-----+
	 *          |     +------+     |
	 *        +---+              +---+   ---
	 *   +----| l |----+         |   |
	 *   |    +---+    |         |   |
	 * +---+         +---+       |   |   ---
	 * |   |         | r |       |   |
	 * |...|         |...|       |...|   ...
	 * |   |         |   |       +---+   ---
	 * |   |         |   |       |xxx|
	 * |   |         |   |       |xxx|
	 * +---+         +---+       +---+   ---
	 * |+++|
	 * |+++|
	 * +---+                             ---
	 */
	DEB(("avl_node_equilibrateLL...\n"));

	l = n->left;
	r = l->right;

	/* primero ajustamos los punteros parent */
	l->parent = n->parent;
	n->parent = l;
	if (r) r->parent = n;

	/* ahora los punteros descendentes. */
	n->left = r;
	l->right = n;

	/* por último, el puntero *rt */
	*rt = l;

	/* COMO PUEDE VERSE, AL FINAL QUEDA EL
	 * ÁRBOL EQUILIBRADO, * TANTO EN n, COMO
	 * EN l. (r NO SE TOCA).
	 *
	 *          |
	 *       +------+                    ---
	 *   +---|  l   |--------+
	 *   |   +------+        |
	 * +---+              +-----+        ---
	 * |   |           +--|this |-+
	 * |...|           |  +-----+ |
	 * |   |         +---+      +---+    ---
	 * |   |         | r |      |   |
	 * |   |         |...|      |   |    ...
	 * +---+         |   |      |   |    ---
	 * |+++|         |   |      |   |
	 * |+++|         |   |      |...|
	 * +---+         +---+      +---+    ---
	 *                          |xxx|
	 *                          |xxx|
	 *                          +---+    ---
	 */

	switch(l->equi) {
	case AVL_EQU:
		l->equi = AVL_RGT; n->equi = AVL_LFT;
		break;
	case AVL_LFT:
		l->equi = AVL_EQU; n->equi = AVL_EQU;
		break;
	} /* switch */

} /* avl_node_equilibrateLL */

static void avl_node_equilibrateRR(
	struct avl_node *n,
	struct avl_node **const rt)
{
	struct avl_node *l, *r;
	assert(*rt == n);

	/* LA OPERACIÓN DE REEQUILIBRADO RR SE
	 * PRODUCE AL INSERTAR EN LA RAMA
	 * DERECHA EL NODO MARCADO +++.  (EL
	 * NODO MARCADO xxx NO EXISTE) SE
	 * PRODUCE UNA DIFERENCIA EN n DE DOS
	 * ALTURAS.  TAMBIÉN SE PRODUCE AL
	 * BORRAR EN LA RAMA IZQUIERDA EL NODO
	 * MARCADO xxx (ENTONCES YA EXISTE EL
	 * NODO MARCADO +++).
	 *
	 *              |
	 *          +------+                 ---
	 *    +-----| this |-----+
	 *    |     +------+     |
	 *  +---+              +---+         ---
	 *  |   |         +----| r |----+
	 *  |   |         |    +---+    |
	 *  |   |       +---+         +---+  ---
	 *  |   |       | l |         |   |
	 *  |...|       |...|         |...|  ...
	 *  +---+       |   |         |   |  ---
	 *  |xxx|       |   |         |   |
	 *  |xxx|       |   |         |   |
	 *  +---+       +---+         +---+  ---
	 *                            |+++|
	 *                            |+++|
	 *                            +---+  ---
	 */
	DEB(("avl_node_equilibrateRR...\n"));

	r = n->right;
	l = r->left;

	/* PRIMERO AJUSTAMOS LOS PUNTEROS parent. */
	r->parent = n->parent;
	n->parent = r;
	if (l) l->parent = n;

	/* AHORA LOS PUNTEROS DESCENDENTES. */
	n->right = l;
	r->left = n;

	/* POR ÚLTIMO, EL PUNTERO rt. */
	*rt = r;

	/* COMO PUEDE VERSE, AL FINAL QUEDA EL
	 * ÁRBOL EQUILIBRADO, TANTO EN n, COMO
	 * EN l. (r NO SE TOCA).
	 *
	 *                       |
	 *                   +------+        ---
	 *          +--------|  r   |---+
	 *          |        +------+   |
	 *        +----+              +---+  ---
	 *     +--|this|--+           |   |
	 *     |  +----+  |           |...|
	 *   +---+      +---+         |   |  ---
	 *   |   |      | l |         |   |
	 *   |   |      |...|         |   |  ...
	 *   |   |      |   |         +---+  ---
	 *   |   |      |   |         |+++|
	 *   |...|      |   |         |+++|
	 *   +---+      +---+         +---+  ---
	 *   |xxx|
	 *   |xxx|
	 *   +---+                           ---
	 */

	switch(r->equi) {
	case AVL_EQU:
		r->equi = AVL_LFT; n->equi = AVL_RGT; break;
	case AVL_RGT:
		r->equi = AVL_EQU; n->equi = AVL_EQU; break;
	} /* switch */

} /* avl_node_equilibrateRR */

static void avl_node_equilibrateLR(
	struct avl_node *n,
	struct avl_node **const rt)
{
	struct avl_node *l, *r;
	assert(*rt == n);

	/* LA SITUACIÓN AHORA ES QUE SE INSERTA
	 * EL NODO +++ (CUANDO EL NODO xxx NO
	 * EXISTE) O BIEN SE BORRA EL NODO xxx
	 * (EXISTIENDO EL NODO +++) (SOLO EXISTE
	 * O SE INSERTA UNO DE LOS NODOS +++)
	 * EN ESTE CASO EL NODO r PASA A SER EL
	 * PADRE DE l Y n, ENLAZÁNDOSE LOS
	 * SUBÁRBOLES DE r EN LOS BRAZOS DERECHO
	 * E IZQUIERDO DE l Y n
	 * (RESPECTIVAMENTE).  EN EL CASO DEL
	 * BORRADO, ES POSIBLE QUE EXISTAN AMBOS
	 * NODOS +++, CON LO QUE r ESTARÍA
	 * EQUILIBRADO Y EL switch DEBE
	 * CONTEMPLAR LOS TRES CASOS.
	 *
	 *                       |
	 *                    +------+        ---
	 *          +---------| this |---+
	 *          |         +------+   |
	 *        +----+               +---+  ---
	 *     +--| l  |-----+         |   |
	 *     |  +----+     |         |   |
	 *   +---+         +---+       |   |  ---
	 *   |   |      +--| r |--+    |   |
	 *   |   |      |  +---+  |    |   |
	 *   |   |    +---+     +---+  +---+  ---
	 *   |   |    |   |     |   |  |xxx|        
	 *   |   |    |   |     |   |  |xxx|        
	 *   +---+    +---+     +---+  +---+  ---   
	 *            |+++|     |+++|               
	 *            |+++|     |+++|               
	 *            +---+     +---+         ---   
	 */
	DEB(("avl_node_equilibrateLR...\n"));
	l = n->left;
	r = l->right;

	/* primero ajustamos los punteros parent: */
	r->parent = n->parent;
	l->parent = r;
	n->parent = r;
	if (r->left) r->left->parent = l;
	if (r->right) r->right->parent = n;

	/* ahora los punteros descendentes: */
	n->left = r->right;
	l->right = r->left;
	r->left = l;
	r->right = n;

	/* ahora los equilibrios */
	switch(r->equi) {
	case AVL_LFT: /* nodo insertado por la izquierda de r. */
		l->equi = AVL_EQU; n->equi = AVL_RGT;
		break;
	case AVL_RGT: /* nodo insertado por la derecha de r. */
		l->equi = AVL_LFT; n->equi = AVL_EQU;
		break;
	case AVL_EQU: /* nodo borrado y r equilibrado. */
		l->equi = AVL_EQU; n->equi = AVL_EQU;
		break;
	} /* switch */
	r->equi = AVL_EQU;

	/* POR ÚLTIMO, AJUSTAMOS *rt */
	*rt = r;

	/* LA SITUACIÓN QUEDA COMO SIGUE:
	 *
	 *                   |
	 *                 +---+              ---
	 *           +-----| r |-----+           
	 *           |     +---+     |           
	 *         +----+         +----+      ---
	 *      +--| l  |-+     +-|this|-+       
	 *      |  +----+ |     | +----+ |       
	 *    +---+     +---+ +---+    +---+  ---
	 *    |   |     |   | |   |    |   |     
	 *    |   |     |   | |   |    |   |     
	 *    |   |     +---+ +---+    |   |  ---
	 *    |   |     |+++| |+++|    |   |     
	 *    |   |     |+++| |+++|    |   |     
	 *    +---+     +---+ +---+    +---+  ---
	 *                             |xxx|     
	 *                             |xxx|     
	 *                             +---+  ---
	 */
} /* avl_node_equilibrateLR */

static void avl_node_equilibrateRL(
	struct avl_node *n,
	struct avl_node **const rt)
{
	struct avl_node *r, *l;
	assert(*rt == n);

	/* LA SITUACIÓN AHORA ES QUE SE INSERTA
	 * EL NODO +++ (CUANDO EL NODO xxx NO
	 * EXISTE) O BIEN SE BORRA EL NODO xxx
	 * (EXISTIENDO EL NODO +++) (SOLO
	 * EXISTE O SE INSERTA UNO DE LOS NODOS
	 * +++) EN ESTE CASO EL NODO r PASA A
	 * SER EL PADRE DE l Y DE n, ENLAZÁNDOSE
	 * LOS SUBÁRBOLES DE r EN LOS BRAZOS
	 * DERECHO E IZQUIERDO DE l Y n
	 * (RESPECTIVAMENTE).  EN EL CASO DEL
	 * BORRADO, ES POSIBLE QUE EXISTAN AMBOS
	 * NODOS +++, CON LO QUE l ESTARÍA
	 * EQUILIBRADO Y EL SWITCH DEBE
	 * CONTEMPLAR LOS TRES CASOS.
	 *
	 *            |                                    
	 *        +------+                   ---           
	 *    +---| this |---------+                       
	 *    |   +------+         |                       
	 *  +---+               +----+       ---           
	 *  |   |         +-----|  r |--+                  
	 *  |   |         |     +----+  |                  
	 *  |   |       +---+         +---+  ---           
	 *  |   |    +--| l |--+      |   |                
	 *  |   |    |  +---+  |      |   |                
	 *  +---+  +---+     +---+    |   |  ---           
	 *  |xxx|  |   |     |   |    |   |                
	 *  |xxx|  |   |     |   |    |   |                
	 *  +---+  +---+     +---+    +---+  ---           
	 *         |+++|     |+++|                         
	 *         |+++|     |+++|                         
	 *         +---+     +---+           ---           
	 */
	DEB(("avl_node_equilibrateRL...\n"));

	r = n->right;
	l = r->left;

	/* PRIMERO AJUSTAMOS LOS PUNTEROS PARENT: */
	l->parent = n->parent;
	r->parent = l;
	n->parent = l;
	if (l->right) l->right->parent = r;
	if (l->left) l->left->parent = n;

	/* AHORA LOS PUNTEROS DESCENDENTES */
	n->right = l->left;
	r->left = l->right;
	l->right = r;
	l->left = n;

	/* AHORA LOS EQUILIBRIOS: */
	switch (l->equi) {
	case AVL_RGT: /* nodo insertado por la derecha de l. */
		r->equi = AVL_EQU; n->equi = AVL_LFT; break;
	case AVL_LFT: /* nodo insertado por la izquierda de l. */
		r->equi = AVL_RGT; n->equi = AVL_EQU; break;
	case AVL_EQU: /* nodo borrado y l equilibrado. */
		r->equi = AVL_EQU; n->equi = AVL_EQU; break;
	} /* switch */
	l->equi = AVL_EQU;

	/* POR ÚLTIMO, AJUSTAMOS rt. */
	*rt = l;

	/* LA SITUACIÓN QUEDA COMO SIGUE: 
	 *
	 *                |                      
	 *              +---+                 ---
	 *        +-----| l |-----+              
	 *        |     +---+     |              
	 *      +----+         +----+         ---
	 *    +-|this|-+     +-|  r |--+         
	 *    | +----+ |     | +----+  |         
	 *  +---+    +---+ +---+     +---+    ---
	 *  |   |    |   | |   |     |   |       
	 *  |   |    |   | |   |     |   |       
	 *  |   |    +---+ +---+     |   |    ---
	 *  |   |    |+++| |+++|     |   |       
	 *  |   |    |+++| |+++|     |   |       
	 *  +---+    +---+ +---+     +---+    ---
	 *  |xxx|                                
	 *  |xxx|                                
	 *  +---+                             ---
	 */
} /* avl_node_equilibrateRL */

static char pr_buf[256];
static int pr_n = 0;

static int avl_node_printNode(
	struct avl_node *n,
	FILE *o,
	const char *pfx)
{
	int res = 0;
	strcpy(pr_buf + pr_n, pfx);
	res += fprintf(o,
		"%s"
		" eq=%s;"
		" (%s);"
#if PRINT_ALL
		" [%p];"
		" p=%p;"
		" l=%p;"
		" r=%p;"
		" d=%p;"
#endif
		"\n",
		pr_buf,
		avl_equ2str(n->equi),
		n->key,
#if PRINT_ALL
		n,
		n->parent,
		n->left,
		n->right,
		n->data,
#endif
		NULL);
	pr_buf[pr_n] = '\0';
	return res;
} /* avl_node_printNode */
		
static void avl_node_printL(
	struct avl_node *n,
	FILE *o,
	const char *prf)
{
	int l = pr_n;
	strcpy(pr_buf + pr_n, prf);
	pr_n += strlen(prf);

	/* SE HA RECURRIDO A CADENAS CON CARACTERES UTF-8
	 * PARA REPRESENTAR EL ÁRBOL Y SUS RAMAS. */
	if (n->right) avl_node_printR(n->right, o, "\xe2\x94\x82");
	avl_node_printNode(n, o, n->right
		? (n->left
			? "\xe2\x95\xb0\xe2\x94\xbc"
			: "\xe2\x95\xb0\xe2\x94\xb4")
		: (n->left
			? "\xe2\x95\xb0\xe2\x94\xac"
			: "\xe2\x95\xb0\xe2\x94\x80"));
	/* llamada recursiva del lado izquierdo */
	if (n->left) avl_node_printL(n->left, o, " ");
	pr_n = l;
	pr_buf[l] = '\0';
} /* avl_node_printL */

static void avl_node_printR(
	struct avl_node *n,
	FILE *o,
	const char *prf)
{
	int l = pr_n;
	strcpy(pr_buf + pr_n, prf);
	pr_n += strlen(prf);

	/* SE HA RECURRIDO A CADENAS CON CARACTERES UTF-8
	 * PARA REPRESENTAR EL ÁRBOL Y SUS RAMAS. */
	if (n->right) avl_node_printR(n->right, o, " ");
	avl_node_printNode(n, o, n->right
		? (n->left
			? "\xe2\x95\xad\xe2\x94\xbc"
			: "\xe2\x95\xad\xe2\x94\xb4")
		: (n->left
			? "\xe2\x95\xad\xe2\x94\xac"
			: "\xe2\x95\xad\xe2\x94\x80"));
	/* llamada recursiva del lado izquierdo */
	if (n->left)
		avl_node_printL(n->left, o,
			"\xe2\x94\x82");
	pr_n = l;
	pr_buf[l] = '\0';
} /* avl_node_printR */

static void avl_node_print(struct avl_node *n, FILE *o)
{
	/* SE HA RECURRIDO A CADENAS CON CARACTERES UTF-8
	 * PARA REPRESENTAR EL ÁRBOL Y SUS RAMAS. */
	if (n->right) avl_node_printR(n->right, o, "");
	avl_node_printNode(n, o, n->right
		? (n->left
			? "\xe2\x94\xbc"
			: "\xe2\x94\xb4")
		: (n->left
			? "\xe2\x94\xac"
			: "\xe2\x94\x80"));
	if (n->left) avl_node_printL(n->left, o, "");
} /* avl_node_print */

AVL_TREE new_avl_tree(AVL_FCOMP fc)
{
	AVL_TREE res;
	assert(res = malloc(sizeof (AVL_TREE)));
	res->sz = 0;
	res->root = NULL;
	res->fcomp = fc;
	return res;
} /* new_avl_tree */

void avl_tree_clear(AVL_TREE t)
{
	free_avl_node(t->root);
	t->root = NULL;
	t->sz = 0;
} /* avl_tree_clear */

void free_avl_tree(AVL_TREE t)
{
	avl_tree_clear(t);
	free(t);
} /* free_avl_tree */

AVL_ITERATOR avl_tree_first(AVL_TREE t)
{
	return t->root
		? avl_node_first(t->root)
		: NULL;
} /* avl_tree_first */

AVL_ITERATOR avl_tree_last(AVL_TREE t)
{
	return t->root
		? avl_node_last(t->root)
		: NULL;
} /* avl_tree_last */

AVL_ITERATOR avl_tree_atkey(
	AVL_TREE t,
	const char *k,
	AVL_MT ex)
{
	avl_equ e;
	AVL_ITERATOR n;
	if (!t->root) return NULL;
	n = avl_node_search(t->root, k, t->fcomp, &e);
	switch(ex) {
	case MT_L: switch(e) {
		case AVL_RGT: return n;
		case AVL_EQU:
		case AVL_LFT: return avl_node_prev(n);
		} break;
	case MT_LE: switch(e) {
		case AVL_RGT:
		case AVL_EQU: return n;
		case AVL_LFT: return avl_node_prev(n);
		} break;
	case MT_EQ: switch(e) {
		case AVL_EQU: return n;
		} break;
	case MT_GE: switch(e) {
		case AVL_RGT: return avl_node_next(n);
		case AVL_EQU:
		case AVL_LFT: return n;
		} break;
	case MT_G: switch(e) {
		case AVL_RGT:
		case AVL_EQU: return avl_node_next(n);
		case AVL_LFT: return n;
		} break;
	} /* switch */
	return NULL;
} /* avl_tree_atkey */

AVL_ITERATOR avl_tree_put(
	AVL_TREE t,
	const char *k,
	void *d)
{
	avl_equ e;
	int crecido;
	struct avl_node *p;
	AVL_ITERATOR res;

	if (!t->root) {
		/* PRIMER NODO EN EL ÁRBOL */
		t->root = new_avl_node(k, d, NULL);
		t->sz++; return t->root;
	} /* if */

	p = avl_node_search(t->root, k, t->fcomp, &e);

	switch(e) {
	case AVL_EQU: p->data = d; return p;
	case AVL_LFT: p->left = new_avl_node(k, d, p);
		t->sz++; p = p->left; break;
	case AVL_RGT: p->right = new_avl_node(k, d, p);
		t->sz++; p = p->right; break;
	} /* switch */

	res = p;

	for (crecido = TRUE; crecido && p->parent; p = p->parent) {
		/* p APUNTA AL NODO QUE ESTAMOS CONSIDERANDO,
		 * q APUNTA AL PADRE Y
		 * r APUNTA AL HIJO DERECHO DE p CUANDO p ES HIJO IZQUIERDO
		 * Y AL HIJO IZQUIERDO DE p CUANDO p ES HIJO DERECHO. */

		struct avl_node *q = p->parent;

		if (q->left == p) { /* p es hijo izquierdo de q. */
			/* ha crecido por la izquierda (p es hijo izquierdo) */
			switch (q->equi) {
			case AVL_EQU: /* pequeño desequilibrio a la izq. */
				q->equi = AVL_LFT; break;
			case AVL_RGT: /* reequilibrio casual tras la inserción. */
				q->equi = AVL_EQU; crecido = FALSE; break;
			case AVL_LFT: /* gran desequilibrio a izquierdas. Hay que equilibrar. */
				switch(p->equi) {
				case AVL_LFT: /* crecimiento izquierda-izquierda */
					avl_node_equilibrateLL(q, q->parent
						? (q->parent->left == q)
							? &q->parent->left
							: &q->parent->right
						: &t->root);
					break;
				case AVL_RGT: /* crecimiento izquierda-derecha */
					avl_node_equilibrateLR(q, q->parent
						? (q->parent->left == q)
							? &q->parent->left
							: &q->parent->right
						: &t->root);
					break;
				} /* switch */
				crecido = FALSE; /* el árbol no ha crecido a partir de aquí. */
				break;
			} /* switch */
		} else { /* q->right == p */
			/* creció por la derecha (p es hijo derecho) */
			switch (q->equi) {
			case AVL_EQU: /* pequeño desequilibrio a la derecha. */
				q->equi = AVL_RGT; break;
			case AVL_LFT: /* reequilibrio casual tras la inserción. */
				q->equi = AVL_EQU; crecido = FALSE; break;
			case AVL_RGT: /* gran desequilibrio a derechas.  Hay que reequilibrar. */
				switch(p->equi) {
				case AVL_RGT: /* crecimiento derecha-derecha */
					avl_node_equilibrateRR(q, q->parent
						? (q->parent->right == q)
							? &q->parent->right
							: &q->parent->left
						: &t->root);
					break;
				case AVL_LFT: /* crecimiento derecha-izquierda */
					avl_node_equilibrateRL(q, q->parent
						? (q->parent->right == q)
							? &q->parent->right
							: &q->parent->left
						: &t->root);
					break;
				} /* switch */
				crecido = FALSE; /* el árbol no ha crecido a partir de aquí */
				break;
			} /* switch */
		} /* if */
	} /* for */
	return res;
} /* avl_tree_put */

int avl_tree_del(AVL_TREE t, const char *k)
{
	avl_equ e;
	struct avl_node *p;

	if (!t->root) return FALSE;
	p = avl_node_search(t->root, k, t->fcomp, &e);
	if (e != AVL_EQU) return FALSE; /* no existe, no borramos nada. */

	p = avl_node_unlink(p, &t->root); /* lo desligamos y lo borramos. */
	free_avl_node(p);
	t->sz--;

	return TRUE;
} /* avl_tree_del */

void avl_iterator_del(
	AVL_ITERATOR i,
	AVL_TREE t)
{
	struct avl_node *p;
	if (!t->root) return;
	if (!i) return;

	p = avl_node_unlink(i, &t->root);
	free_avl_node(p);
	t->sz--;
} /* avl_iterator_del */

int avl_tree_empty(AVL_TREE t)
{
	return !t->root;
} /* avl_tree_empty */

int avl_tree_size(struct avl_tree *t)
{
	return t->sz;
} /* avl_tree_size */

int avl_tree_has(AVL_TREE t, const char *k)
{
	avl_equ e;

	if (!t->root) return FALSE;
	avl_node_search(t->root, k, t->fcomp, &e);

	return e == AVL_EQU;
} /* avl_tree_has */

void *avl_tree_get(AVL_TREE t, const char *k)
{
	avl_equ e;
	struct avl_node *p;

	if (!t->root) return NULL;
	p = avl_node_search(t->root, k, t->fcomp, &e);

	return (e == AVL_EQU)
		? p->data
		: NULL;
} /* avl_tree_get */

void avl_tree_print(AVL_TREE t, FILE *o)
{
	if (t->root)
	avl_node_print(t->root, o);
} /* avl_tree_print */

AVL_ITERATOR avl_iterator_next(AVL_ITERATOR i)
{
	if (!i) return NULL;
	return avl_node_next(i);
} /* avl_iterator_next */

AVL_ITERATOR avl_iterator_prev(AVL_ITERATOR i)
{
	if (!i) return NULL;
	return avl_node_prev(i);
} /* avl_iterator_prev */

char *avl_iterator_key(AVL_ITERATOR i)
{
	return i->key;
} /* avl_iterator_key */

void *avl_iterator_data(AVL_ITERATOR i)
{
	return i->data;
} /* avl_iterator_data */

/* $Id: avl.c,v 1.3 2012/08/22 13:41:14 luis Exp $ */
